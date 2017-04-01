#include "rtmp_buffer.h"
#include <time.h>
#include "rtmp_ios.h"
#include <sys/time.h>
/*
分配缓冲区
*/
RTMPBuffer* RTMP_Alloc_Buffer()
{
	//printf("---RTMP_Alloc_Buffer-----\n");
	return calloc(1,sizeof(RTMPBuffer));

}

/*初始化缓冲区*/
int RTMP_Init_Buffer(RTMPBuffer *pBuffer)
{

	pBuffer->pBufferNode=NULL;
	pBuffer->pLastNode=NULL;
	pBuffer->pAacConfig=0xAF;
	pBuffer->iBeginTime=0;
	pBuffer->pPacketConfig=(RTMPPacket *)calloc(1,RTMP_HEAD_SIZE+1024);	
	pBuffer->pPacketConfig->m_body=(char *)pBuffer->pPacketConfig+RTMP_HEAD_SIZE;
	pBuffer->packet_count=0;
	rtmp_mutex_init(&pBuffer->lock);
    //pBuffer->buffer_state=0;
	
	
	return TRUE;
}
/*
释放缓冲区
*/
int RTMP_Free_Buffer(RTMPBuffer *pBuffer)
{
	//printf("-----RTMP_Free_Buffer ------\n");
	if(pBuffer!=NULL)
	{
		//释放缓冲区链表
        pthread_mutex_lock(&pBuffer->lock);
		RTMPBuffer_Node *tmp;
		while(pBuffer->pBufferNode)
		{
			tmp=pBuffer->pBufferNode;
			pBuffer->pBufferNode=pBuffer->pBufferNode->next;
			RTMP_Free_BufferNode(tmp);
            
		}
		pBuffer->pBufferNode=NULL;
		pBuffer->pLastNode=NULL;
		
		RTMP_Free_SPS_PPS(&pBuffer->pSPSPPSData);

		if(pBuffer->pPacketConfig)
		{
            free(pBuffer->pPacketConfig);
			pBuffer->pPacketConfig=NULL;
		}
		pthread_mutex_unlock(&pBuffer->lock);
		rtmp_mutex_destroy(&pBuffer->lock);
		
		free(pBuffer);
	}
	pBuffer=NULL;
	return TRUE;
}

/*
给buffer node分配空间
*/
RTMPBuffer_Node* RTMP_Alloc_BufferNode()
{
	//printf("----RTMP_Alloc_BufferNode -----\n");
	return calloc(1,sizeof(RTMPBuffer_Node));
}

int RTMP_Free_BufferNode(RTMPBuffer_Node *pNode)
{
	//printf("----RTMP_Free_BufferNode -----\n");
	if(pNode)
	{
		free(pNode);
	}
	pNode=NULL;
	return TRUE;
}

int RTMP_Free_SPS_PPS(RTMP_SPS_PPS *pSPSPPS)
{
	//printf("----RTMP_Free_SPS_PPS ----\n");
	if(pSPSPPS)
	{
		if(pSPSPPS->sPPS)
			free(pSPSPPS->sPPS);
		if(pSPSPPS->sSPS)
			free(pSPSPPS->sSPS);
		pSPSPPS->sPPS=NULL;
		pSPSPPS->sSPS=NULL;
		//free(pSPSPPS);
		//pSPSPPS=NULL;
	}
	
	return TRUE;
}

/*
将packet放入buffer中,packet已经分配好空间了
*/
int RTMP_Put_Buffer(RTMPBuffer *pBuffer,RTMPPacket *pPacket,int iKeyFrame)
{
	//printf("---- RTMP_Put_Buffer ----\n");
	if(!pBuffer || !pPacket)
		return FALSE;
	
	//首先分配bufferNode
	RTMPBuffer_Node *pNode=NULL;
	
	pNode=RTMP_Alloc_BufferNode();
	if(!pNode)
		return FALSE;
	//赋值
	pNode->pPacket=pPacket;
	pNode->next=NULL;
	pNode->nFrame_type=iKeyFrame;
	
	pthread_mutex_lock(&pBuffer->lock);
	//加入链表
	if(pBuffer)
	{
		if(pBuffer->pBufferNode==NULL)
		{
			pBuffer->pBufferNode=pNode;
			pBuffer->pLastNode=pNode;
		}
		else
		{
			if(pBuffer->pLastNode)
			{
				pBuffer->pLastNode->next=pNode;
				pBuffer->pLastNode=pNode;
			}
			else // need discuss
			{
				pBuffer->pLastNode=pNode;
				pBuffer->pLastNode->next=pNode;
			}
		}
		//更新最后一个I帧的时间
		if(iKeyFrame)
		{
			pBuffer->iLast_i_frame_time=pPacket->m_nTimeStamp;
		}
			
		pBuffer->packet_count++;
		pBuffer->iLast_frame_time=pPacket->m_nTimeStamp;
		
		pthread_mutex_unlock(&pBuffer->lock);
	}
	else
	{
		if(pNode)
		{
			free(pNode);
			pNode=NULL;
		}
		pthread_mutex_unlock(&pBuffer->lock);
		return FALSE;
	}
	
	return TRUE;
}

/*
从buffer中读取一个packet
*/
RTMPBuffer_Node* RTMP_Get_Buffer(RTMPBuffer *pBuffer)
{
	//printf("---- RTMP_Get_Buffer ----\n");
	if(!pBuffer || !pBuffer->pBufferNode)
		return NULL;
	RTMPBuffer_Node *pNode=NULL;
	
	pthread_mutex_lock(&pBuffer->lock);
	
	pNode=pBuffer->pBufferNode;
    
	pBuffer->pBufferNode=pBuffer->pBufferNode->next;
	if(pNode)
    {
        pBuffer->packet_count--;
        pBuffer->iBeginTime=pNode->pPacket->m_nTimeStamp;
    }
    
	pthread_mutex_unlock(&pBuffer->lock);
	
	return pNode;

}

uint32_t RTMP_Get_Last_Time(RTMPBuffer *pBuffer)
{
	if(pBuffer==NULL)
		return -1;
	uint32_t iTime=0;
	pthread_mutex_lock(&pBuffer->lock);
	iTime=pBuffer->iLast_i_frame_time;
	pthread_mutex_unlock(&pBuffer->lock);
	return iTime;

}

int RTMP_Send_Sequence(RTMP *pRtmp,RTMPBuffer *pBuffer,uint32_t iTime)
{
	//get sequence
	pthread_mutex_lock(&pBuffer->lock);
	RTMPPacket *pPacket=pBuffer->pPacketConfig;
	pthread_mutex_unlock(&pBuffer->lock);
	pPacket->m_nTimeStamp=iTime;
	if(!RTMP_SendPacket(pRtmp,pPacket,0))
	{
		printf(" send sequence fail \n");
		return FALSE;
	}

	return TRUE;
}

int InitBufferConfig(BufferConfig *pConfig,int max_delay,int wait_time,int discard_type,int send_window,int retrytime)
{
	RTMP * pRTMP = NULL;
	RTMPBuffer *pVideoBuffer = NULL;
	RTMPBuffer *pAudioBuffer = NULL;

	//建立 RTMP 连接
	pRTMP = RTMP_Alloc();
    
	//video buffer
	pVideoBuffer = (RTMPBuffer *)calloc(1,sizeof(RTMPBuffer));
	RTMP_Init_Buffer(pVideoBuffer);
	pAudioBuffer = (RTMPBuffer *)calloc(1,sizeof(RTMPBuffer));
	RTMP_Init_Buffer(pAudioBuffer);


	pConfig->pRTMP = pRTMP;
    
	pConfig->pVideoBuffer = pVideoBuffer;
	pConfig->pAudioBuffer = pAudioBuffer;
	pConfig->state = init_status;
    
    
	pConfig->iMaxDelay = max_delay;
	pConfig->iWaitTime = wait_time;
	pConfig->send_video_type = discard_type;
	pConfig->iReSend_Window_Time = send_window;
    pConfig->iMaxRetryTime = retrytime;
    
    pConfig->min_bitrate_status = 0;
    
    rtmp_mutex_init(&pConfig->lock_min_bitrate_status);
    rtmp_mutex_init(&pConfig->lock);
    rtmp_mutex_init(&pConfig->lock_net_change);
 
    

	return TRUE;
}

void FreeBufferConfig(BufferConfig *pConfig)
{
	if(pConfig!=NULL)
	{
        rtmp_mutex_destroy(&pConfig->lock);
        rtmp_mutex_destroy(&pConfig->lock_min_bitrate_status);
        rtmp_mutex_destroy(&pConfig->lock_net_change);
        
		if(pConfig->pVideoBuffer!=NULL)
		{
           
			RTMP_Free_Buffer(pConfig->pVideoBuffer);
			pConfig->pVideoBuffer=NULL;
            
		}
		if(pConfig->pAudioBuffer)
		{
           
			RTMP_Free_Buffer(pConfig->pAudioBuffer);
			pConfig->pAudioBuffer=NULL;
            
		}
        
        
        if(pConfig->pRTMP)
        {
            free_portal(pConfig->pRTMP);
            
            free(pConfig->pRTMP);
            pConfig->pRTMP = NULL;
        }
		free(pConfig);
		pConfig=NULL;
	}
}

int GetBufferCurrentTime(BufferConfig *pConfig)
{
	int iCurrentTime=0;
	if(pConfig->pVideoBuffer)
		iCurrentTime=RTMP_GetTime_m(pConfig->pVideoBuffer->t_start);
	else if(pConfig->pAudioBuffer)
		iCurrentTime=RTMP_GetTime_m(pConfig->pAudioBuffer->t_start);
	return iCurrentTime;
}
void SetBufferStartTime(BufferConfig *pConfig)
{
	struct timeval tCurrentTime;
	gettimeofday(&tCurrentTime, NULL);

	if(pConfig->pVideoBuffer)
		pConfig->pVideoBuffer->t_start=tCurrentTime;
	if(pConfig->pAudioBuffer)
		pConfig->pAudioBuffer->t_start=tCurrentTime;
}



