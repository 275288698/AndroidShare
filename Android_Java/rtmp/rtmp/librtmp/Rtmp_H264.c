/*
add by zongchunli 

用于将H264裸数据打包成RTMP Packet
*/

#include "Rtmp_H264.h"

/*
对SPS帧进行解析，获得所需要的参数,width,height,framerate
*/

int RTMP_Decode_SPS(unsigned char *sVideoData,RTMP_SPS_PPS *sSps)
{

	return TRUE;
}

/*
将SPS，PPS数据组装成packet
*/
int RTMP_Packet_SPS_PPS(RTMP* pRtmp,RTMP_SPS_PPS sSPSPPS,uint32_t nTime,RTMPPacket *pPacket)
{
	unsigned char *body = (unsigned char *)pPacket->m_body;
	int i = 0;

	body[i++] = 0x17; // 1:keyframe 7:AVCVIDEOPACKET
	body[i++] = 0x00; // 0:AVC sequence header

	body[i++] = 0x00; // Composion Time
	body[i++] = 0x00; // Composion Time
	body[i++] = 0x00; // Composion Time

	/*AVCDecoderConfigurationRecord*/
	body[i++] = 0x01; // 版本号，1
	body[i++] = sSPSPPS.sSPS[1]; // sps[1]
	body[i++] = sSPSPPS.sSPS[2]; // sps[2]
	body[i++] = sSPSPPS.sSPS[3]; // sps[3]
	body[i++] = 0xff; // 6bit：reserved，111111；2bit：lengthSizeMinusOne，一般为3

	/* SPS */
	body[i++] = 0xe1; // 3bit：reserved，111；5bit：numOfSequenceParameterSets，一般为1
	body[i++] = (sSPSPPS.nSPSLen >> 8) & 0xff;
	body[i++] = sSPSPPS.nSPSLen & 0xff;	
	memcpy(&body[i], sSPSPPS.sSPS, sSPSPPS.nSPSLen);
	i += sSPSPPS.nSPSLen;

	/* PPS */
	body[i++] = 0x01; // numOfPictureParameterSets，一般为1
	body[i++] = (sSPSPPS.nPPSLen >> 8) & 0xff;
	body[i++] = (sSPSPPS.nPPSLen) & 0xff;	
	memcpy(&body[i], sSPSPPS.sPPS, sSPSPPS.nPPSLen);
	i +=  sSPSPPS.nPPSLen;

	//构建packet字段
	pPacket->m_packetType = RTMP_PACKET_TYPE_VIDEO;	
	pPacket->m_nBodySize = i;	
	pPacket->m_nChannel = 0x04;	
	pPacket->m_nTimeStamp = nTime;	
	pPacket->m_hasAbsTimestamp = 0;	
	pPacket->m_headerType = RTMP_PACKET_SIZE_LARGE;
	//printf(" stream id: %d \n",pRtmp->m_stream_id);
	pPacket->m_nInfoField2 = pRtmp->m_stream_id;
	//pPacket->m_nInfoField2 = 0;
	
	return TRUE;
}

/*
将H264 NAL 组装成packet
*/

int RTMP_Packet_NAL(RTMP* pRtmp, unsigned char *sVideoData, int size, uint32_t pts, uint32_t dts, int iKey, RTMPPacket *pPacket)
{
	unsigned char *body = (unsigned char *)pPacket->m_body;
	int i=0;

	if (iKey) //I frame
	{
		body[i++] = 0x17; // 1:keyframe  7:AVC
	}
	else//P frame
	{
		body[i++] = 0x27; // 2:interframe  7:AVC
	}
	
	body[i++] = 0x01; // 1: AVC NALU
	uint32_t cts = pts - dts;
	body[i++] = (cts >> 16) & 0xff;
	body[i++] = (cts >> 8) & 0xff;
	body[i++] = cts & 0xff;
		
	// NALU size 
#if !defined(_LINUX_IOS_)
	body[i++] = size >> 24 & 0xff;
	body[i++] = size >> 16 & 0xff;
	body[i++] = size >> 8 & 0xff;
	body[i++] = size & 0xff;
#endif
		
	// copy NALU data to packet		
	memcpy(&body[i], sVideoData, size);
	//填充packet字段
	pPacket->m_nBodySize = i + size;
	pPacket->m_nTimeStamp = dts;
	pPacket->m_hasAbsTimestamp = 0;
	pPacket->m_packetType = RTMP_PACKET_TYPE_VIDEO;
	pPacket->m_nChannel = 0x04;
	//printf(" stream id: %d \n",pRtmp->m_stream_id);
	pPacket->m_nInfoField2 = pRtmp->m_stream_id;
	//pPacket->m_nInfoField2=0;
	pPacket->m_headerType = RTMP_PACKET_SIZE_LARGE;	
	
	return TRUE;
}


/*
将H264裸数据打包成packet，病放入buffer中
sVideoData :H264 裸数据，一个NAL数据

*/
int RTMP_Packet_H264(RTMP* pRtmp, unsigned char *sVideoData, int size, uint32_t pts, uint32_t dts, RTMPBuffer *pVideoBuffer)
{
	//printf("---- RTMP_Packet_H264 ----\n");
	//没有数据，或者，传入的数据小于11,或者缓冲区为空
	
	if(sVideoData ==NULL || size==0 || pVideoBuffer==NULL)
	{
		return FALSE;
	}
	
#if defined(_LINUX_IOS_)
	sVideoData += 4;
	size -= 4;
#endif

	int iKeyFrame=0;
	
	RTMPPacket *pPacket=NULL;
	
	
	//获取本NAL的类型。7: sps 8:pps 5:关键帧 1:非关键帧
	int nal_type;
	nal_type=sVideoData[0]&0x1F; 
	
	//SPS,只有SPS时不能建立一个packet放入buffer中，必须跟PPS一起
	if(nal_type==7)
	{
		//printf(" SPS packet \n");
		pVideoBuffer->pSPSPPSData.nSPSLen=size;
		if(pVideoBuffer->pSPSPPSData.sPPS!=NULL)
		{
			free(pVideoBuffer->pSPSPPSData.sSPS);
			
		}
		pVideoBuffer->pSPSPPSData.sSPS=NULL;
		pVideoBuffer->pSPSPPSData.sSPS=(unsigned char *)malloc(size);
        if(!pVideoBuffer->pSPSPPSData.sSPS)
            return FALSE;
		memcpy(pVideoBuffer->pSPSPPSData.sSPS,sVideoData,size);
		pVideoBuffer->pSPSPPSData.nSPSLen=size;
		//decode SPS
		
		RTMP_Decode_SPS(sVideoData,&pVideoBuffer->pSPSPPSData);
		//SPS不需要组装Packet，直接返回
		return TRUE;
	}
	//PPS
	else if(nal_type==8)
	{
		//printf(" PPS packet \n");
		iKeyFrame=TRUE;
		pVideoBuffer->pSPSPPSData.nPPSLen=size;
		if(pVideoBuffer->pSPSPPSData.sPPS!=NULL)
		{
			free(pVideoBuffer->pSPSPPSData.sPPS);
		}
		pVideoBuffer->pSPSPPSData.sPPS=NULL;
		pVideoBuffer->pSPSPPSData.sPPS=(unsigned char *)malloc(size);
        if(!pVideoBuffer->pSPSPPSData.sPPS)
            return FALSE;
		memcpy(pVideoBuffer->pSPSPPSData.sPPS,sVideoData,size);
		pVideoBuffer->pSPSPPSData.nPPSLen=size;
		//判断一下，如果SPS和PPS同时有数据，则组装packet
		if(pVideoBuffer->pSPSPPSData.nSPSLen!=0 && pVideoBuffer->pSPSPPSData.nPPSLen!=0)
		{
		
			pPacket = (RTMPPacket *)calloc(1,RTMP_HEAD_SIZE+100+pVideoBuffer->pSPSPPSData.nSPSLen+pVideoBuffer->pSPSPPSData.nPPSLen);
            
            if(!pPacket)
            {
                return FALSE;
            }
			
			memset(pPacket,0,RTMP_HEAD_SIZE+100+pVideoBuffer->pSPSPPSData.nSPSLen+pVideoBuffer->pSPSPPSData.nPPSLen);	
			
			pPacket->m_body = (char *)pPacket + RTMP_HEAD_SIZE;
			
			
			if(!RTMP_Packet_SPS_PPS(pRtmp,pVideoBuffer->pSPSPPSData,pts,pPacket))
				return FALSE;

			//将video sequence config拷贝到buffer中
			
			pthread_mutex_lock(&pVideoBuffer->lock);
			if(pVideoBuffer->pPacketConfig)
				free(pVideoBuffer->pPacketConfig);
			pVideoBuffer->pPacketConfig=(RTMPPacket *)calloc(1,RTMP_HEAD_SIZE+100+pVideoBuffer->pSPSPPSData.nSPSLen+pVideoBuffer->pSPSPPSData.nPPSLen);
            if(!pVideoBuffer->pPacketConfig)
            {
                return FALSE;
            }
			memcpy(pVideoBuffer->pPacketConfig,pPacket,RTMP_HEAD_SIZE+100+pVideoBuffer->pSPSPPSData.nSPSLen+pVideoBuffer->pSPSPPSData.nPPSLen);
			pVideoBuffer->pPacketConfig->m_body=(char *)pVideoBuffer->pPacketConfig+RTMP_HEAD_SIZE;
			pthread_mutex_unlock(&pVideoBuffer->lock);
		}
		
	}
	else 
	{
		//printf("  NAL packet \n");
		
		iKeyFrame=(nal_type==5)?TRUE:FALSE;
		
		pPacket = (RTMPPacket *)calloc(1,RTMP_HEAD_SIZE+size+9);
        if(!pPacket)
        {
            return FALSE;
        }
		memset(pPacket,0,RTMP_HEAD_SIZE);
		
		pPacket->m_body = (char *)pPacket + RTMP_HEAD_SIZE;

#if defined(_LINUX_IOS_)
		if (!RTMP_Packet_NAL(pRtmp, sVideoData - 4, size + 4, pts, dts, iKeyFrame, pPacket))
#else
		if (!RTMP_Packet_NAL(pRtmp, sVideoData, size, pts, dts, iKeyFrame, pPacket))
#endif
			return FALSE;
	}

	//给buffer node分配空间，加入buffer中
	
	if(!RTMP_Put_Buffer(pVideoBuffer,pPacket,iKeyFrame))
    {
        if(pPacket)
        {
            free(pPacket);
            pPacket=NULL;
        }
		return FALSE;
    }

	
	
	return TRUE;
	
}



