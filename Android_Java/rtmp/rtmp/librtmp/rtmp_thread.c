
#include "rtmp_thread.h"
#include "rtmp_ios.h"
#include "log.h"
#include "StasticEvent.h"


void Set_Wait_Time(BufferConfig *pConfig,uint32_t iWaitTime)
{
	pConfig->iWaitTime=iWaitTime;
}

void Set_Max_Delay(BufferConfig *pConfig,uint32_t iDelayTime)
{
	pConfig->iMaxDelay=iDelayTime;
}

/*
 缓冲控制线程
*/

int Buffer_Control(RTMPMetadata *pMetadata, BufferConfig *pConfig,RTMPAccess *pAccess,void (*ChangeState)(int))
{
	RTMPBuffer_Node *pVideoNode = NULL;
	RTMPBuffer_Node *pAudioNode = NULL;
	RTMPPacket *pVideoPacket = NULL;
	RTMPPacket *pAudioPacket = NULL;
	RTMPPacket *pSendPacket = NULL;
	uint32_t iCurrentTime = 0;//当前系统时间
	uint32_t iLastTime = 0;//缓冲区最后一个I帧的时间
	uint32_t iWaitTime = 0;//缓冲区数据等待时间
	RTMPBuffer *pVideoBuffer = pConfig->pVideoBuffer;
	RTMPBuffer *pAudioBuffer = pConfig->pAudioBuffer;
	RTMP *pRtmp = pConfig->pRTMP;
	int iSendVideo = 1;//发不发送video packet.0:不发送  1: 发送
	uint32_t iLastDelay_Time = 0;//最近一次到达最大延迟的系统时间
	int iSendFrameType = 1;//当前 send packet的类型。0: P frame 。  1: I frame
	
	// 指针判空
	if (!pRtmp || !pVideoBuffer || !pAudioBuffer)
	{
		return FALSE;
	}
	
	pVideoNode = RTMP_Get_Buffer(pVideoBuffer);
	pAudioNode = RTMP_Get_Buffer(pAudioBuffer);
	
	int state_rtmp = send_status;
	
	int drop_data = 0;
	
	int min_bitrate_status = 0;
	//发送数据
	while (TRUE)
	{
		//drop data
		pthread_mutex_lock(&pRtmp->lock_drop);
		pRtmp->m_drop_data += drop_data;
		pthread_mutex_unlock(&pRtmp->lock_drop);
		
		drop_data = 0;
		
		// video buffer 和audio buffer 都为空，sleep 100毫秒等待缓冲区数据，10秒后还没有数据则退出
		iWaitTime = 0;
		while (!pVideoNode && !pAudioNode && iWaitTime<pConfig->iWaitTime)
		{
			pthread_mutex_lock(&pConfig->lock);
			state_rtmp = pConfig->state;
			pthread_mutex_unlock(&pConfig->lock);
			
			if (state_rtmp != send_status)
				break;
			
			msleep(10);
			pVideoNode = RTMP_Get_Buffer(pVideoBuffer);
			pAudioNode = RTMP_Get_Buffer(pAudioBuffer);
			iWaitTime += 10;
			//RTMP_Log(RTMP_LOGINFO,"push : **** buffer is no data to send ,pConfig->state:%d ****\n",pConfig->state);
			continue;
		}
		if (iWaitTime >= pConfig->iWaitTime)
		{
			RTMP_Log(RTMP_LOGINFO,"*** buffer is null ***\n");
			break;
		}
		else
			iWaitTime = 0;
		
		pthread_mutex_lock(&pConfig->lock);
		state_rtmp = pConfig->state;
		pthread_mutex_unlock(&pConfig->lock);
		
		if (state_rtmp != send_status)
			break;
		
		if (pVideoNode)
		{
			pVideoPacket = pVideoNode->pPacket;
		}
		else
			pVideoPacket = NULL;
		
		if (pAudioNode)
		{
			pAudioPacket = pAudioNode->pPacket;
		}
		else
			pAudioPacket=NULL;
		
		if (pVideoPacket && pAudioPacket)
		{
			if (pVideoPacket->m_nTimeStamp < pAudioPacket->m_nTimeStamp)
			{
				pSendPacket = pVideoPacket;
				iSendFrameType = pVideoNode->nFrame_type;
				//RTMP_Free_BufferNode(pVideoNode);
				if (pVideoNode)
					free(pVideoNode);
				pVideoNode = NULL;
				pVideoNode = RTMP_Get_Buffer(pVideoBuffer);
			}
			//audio 小于等于 video
			else
			{
				pSendPacket = pAudioPacket;
				iSendFrameType = pAudioNode->nFrame_type;
				//RTMP_Free_BufferNode(pAudioNode);
				if (pAudioNode)
					free(pAudioNode);
				pAudioNode = NULL;
				pAudioNode = RTMP_Get_Buffer(pAudioBuffer);
			}
		}
		else if (!pVideoPacket && pAudioPacket)//只有 audio packet
		{
			pSendPacket = pAudioPacket;
			iSendFrameType = pAudioNode->nFrame_type;
			//RTMP_Free_BufferNode(pAudioNode);
			if (pAudioNode)
				free(pAudioNode);
			pAudioNode = NULL;
			pAudioNode = RTMP_Get_Buffer(pAudioBuffer);
		}
		else if (pVideoPacket && !pAudioPacket)//只有 video packet
		{
			pSendPacket = pVideoPacket;
			iSendFrameType = pVideoNode->nFrame_type;
			//RTMP_Free_BufferNode(pVideoNode);
			if (pVideoNode)
				free(pVideoNode);
			pVideoNode = NULL;
			pVideoNode = RTMP_Get_Buffer(pVideoBuffer);
		}
		else
			continue;
		
		iCurrentTime = pVideoBuffer->iLast_frame_time > pAudioBuffer->iLast_frame_time ? pVideoBuffer->iLast_frame_time : pAudioBuffer->iLast_frame_time;
		
		if ((iCurrentTime) < (pConfig->iMaxDelay+pSendPacket->m_nTimeStamp))
		{
			if ((iCurrentTime-pSendPacket->m_nTimeStamp) > (pConfig->iMaxDelay/2))
			{
				if (pAudioBuffer)
				{
					if (iSendVideo == 1)
					{
						ChangeState(publish_audio_only);
					}
					iSendVideo = 0;
					iLastDelay_Time = iCurrentTime;
				}
				else //没有audio 流
				{
					iSendVideo = 1;
				}
			}
			if (pSendPacket->m_packetType == 8 || pSendPacket->m_packetType == 18)
			{
				//RTMP_Log(RTMP_LOGINFO,"~~~ SEND AUDIO  : %d \n",pSendPacket->m_nTimeStamp);
				if (!RTMP_SendPacket_reconnect(pMetadata, pConfig, pSendPacket, pAccess, ChangeState))
				{
					RTMP_Log(RTMP_LOGINFO, "push: *** send AUDIO data to server fail");
					if (pSendPacket)
					{
						free(pSendPacket);
						pSendPacket = NULL;
					}
					return FALSE;
				}
			}
			else //video
			{
				pthread_mutex_lock(&pConfig->lock_min_bitrate_status);
				min_bitrate_status = pConfig->min_bitrate_status;
				pthread_mutex_unlock(&pConfig->lock_min_bitrate_status);
				
				//RTMP_Log(RTMP_LOGINFO,"  push : min bitrate : %d \n",min_bitrate_status);
				if (min_bitrate_status == 0)
				{
					if (!RTMP_SendPacket_reconnect(pMetadata, pConfig, pSendPacket, pAccess, ChangeState))
					{
						RTMP_Log(RTMP_LOGINFO, "push: *** send VIDEO data to server fail");
						if (pSendPacket)
						{
							free(pSendPacket);
							pSendPacket = NULL;
						}
						return FALSE;
					}
				}
				else
				{
					if (pConfig->send_video_type == 1
					   || ( pConfig->send_video_type == 0 && iSendVideo == 1 ))
					{
						//RTMP_Log(RTMP_LOGINFO,"*** SEND video: type  %d  packet time: %d  \n",iSendFrameType,pSendPacket->m_nTimeStamp);
						if (!RTMP_SendPacket_reconnect(pMetadata, pConfig, pSendPacket, pAccess, ChangeState))
						{
							RTMP_Log(RTMP_LOGINFO, "push: *** send VIDEO data to server fail");
							if (pSendPacket)
							{
								free(pSendPacket);
								pSendPacket = NULL;
							}
							return FALSE;
						}
					}
					else if (iSendVideo == 0  &&  (iCurrentTime-iLastDelay_Time)>=pConfig->iMaxRetryTime) //pConfig->iMaxRetryTime
					{
						//int iLastVideoTime=RTMP_Get_Last_Time(pConfig->pVideoBuffer);
						int iLastAudioTime=RTMP_Get_Last_Time(pConfig->pAudioBuffer);
						
						if (iSendFrameType == 1)
						{
							//RTMP_Log(RTMP_LOGINFO,"### type: %d %d  packet time: %d   system time: %d \n",pSendPacket->m_packetType,iSendFrameType,pSendPacket->m_nTimeStamp,iCurrentTime);
							if (!RTMP_SendPacket_reconnect(pMetadata, pConfig, pSendPacket, pAccess, ChangeState))
							{
								if (pSendPacket)
								{
									free(pSendPacket);
									pSendPacket = NULL;
								}
								return FALSE;
							}
							
							ChangeState(publish_all);
							iSendVideo=1;
						}
						else
							drop_data += pSendPacket->m_nBodySize;
					}
					else
					{
						drop_data += pSendPacket->m_nBodySize;
					}
				}
			}
			if (pSendPacket)
				free(pSendPacket);
			pSendPacket = NULL;
		}
		else
		{
			iLastTime = RTMP_Get_Last_Time(pVideoBuffer);
			//RTMP_Log(RTMP_LOGINFO," skip frame : last i frame time :%d  packet time : %d   current time: %d  type :%d \n",iLastTime,pSendPacket->m_nTimeStamp,iCurrentTime,pSendPacket->m_packetType);
			if (iLastTime <= pSendPacket->m_nTimeStamp)
			{
				//RTMP_Log(RTMP_LOGINFO,"~~~ type: %d %d  packet time: %d   system time: %d \n",pSendPacket->m_packetType,iSendFrameType,pSendPacket->m_nTimeStamp,iCurrentTime);
				if (!RTMP_SendPacket_reconnect(pMetadata, pConfig, pSendPacket, pAccess, ChangeState))
				{
					//RTMP_Log(RTMP_LOGINFO,"*** send data to server fail \n");
					if (pSendPacket)
					{
						free(pSendPacket);
						pSendPacket = NULL;
					}
					return FALSE;
				}
				
				if (pSendPacket)
					free(pSendPacket);
				pSendPacket = NULL;
			}
			else
			{
				RTMP_Log(RTMP_LOGINFO,"push : =============== drop frame ========to  %d \n",iLastTime);
				
				int isendpacket_time = pSendPacket->m_nTimeStamp;
				while (pVideoNode)
				{
					pVideoPacket = pVideoNode->pPacket;
					if (!pVideoPacket)
						break;
					if (isendpacket_time < iLastTime)
					{
						if (pVideoPacket->m_nTimeStamp < iLastTime)
						{
							drop_data += pVideoPacket->m_nBodySize;
							if (pVideoNode)
								free(pVideoNode);
							pVideoNode = NULL;
							if (pVideoPacket)
								free(pVideoPacket);
							pVideoPacket = NULL;
							pVideoNode = RTMP_Get_Buffer(pVideoBuffer);
						}
						else
						{
							if (!RTMP_SendPacket_reconnect(pMetadata, pConfig, pVideoPacket, pAccess, ChangeState))
							{
								if (pVideoPacket)
								{
									free(pVideoPacket);
									pSendPacket = NULL;
									free(pVideoNode);
									pVideoNode = NULL;
								}
								
								return FALSE;
							}
							
							if (pVideoPacket)
								free(pVideoPacket);
							pVideoPacket = NULL;
							//pSendPacket=NULL;
							if (pVideoNode)
								free(pVideoNode);
							pVideoNode = NULL;
							pVideoNode = RTMP_Get_Buffer(pVideoBuffer);
							break;
						}
					}
					else
					{
						drop_data += pVideoPacket->m_nBodySize;
						if (pVideoNode)
							free(pVideoNode);
						pVideoNode = NULL;
						if (pVideoPacket)
							free(pVideoPacket);
						pVideoPacket = NULL;
						pVideoNode = RTMP_Get_Buffer(pVideoBuffer);
					}
				}
				
				while (pAudioNode)
				{
					pAudioPacket = pAudioNode->pPacket;
					if (!pAudioPacket)
						break;
					//LOGE_jni(" drop audio node ");
					if (isendpacket_time < iLastTime)
					{
						if (pAudioPacket->m_nTimeStamp < iLastTime)
						{
							drop_data += pAudioPacket->m_nBodySize;
							if (pAudioNode)
								free(pAudioNode);
							pAudioNode = NULL;
							if (pAudioPacket)
								free(pAudioPacket);
							pAudioPacket = NULL;
							pAudioNode = RTMP_Get_Buffer(pAudioBuffer);
						}
						else
						{
							if (!RTMP_SendPacket_reconnect(pMetadata, pConfig, pAudioPacket, pAccess, ChangeState))
							{
								if (pAudioPacket)
								{
									free(pAudioPacket);
									pAudioPacket = NULL;
									free(pAudioNode);
									pAudioNode = NULL;
								}
								return FALSE;
							}
							
							if (pAudioPacket)
								free(pAudioPacket);
							pAudioPacket = NULL;
							if (pAudioNode)
							{
								free(pAudioNode);
								pAudioNode = NULL;
							}
							pAudioNode = RTMP_Get_Buffer(pAudioBuffer);
							break;
						}
					}
					else
					{
						drop_data += pAudioPacket->m_nBodySize;
						if (pAudioNode)
							free(pAudioNode);
						pAudioNode = NULL;
						if (pAudioPacket)
							free(pAudioPacket);
						pAudioPacket = NULL;
						pAudioNode = RTMP_Get_Buffer(pAudioBuffer);
					}
				}
				//LOGE_jni("=============== drop  audio end ========to  %d \n",iLastTime);
				if (pAudioBuffer)
				{
					iSendVideo = 0;
					iLastDelay_Time = iCurrentTime;
				}
				else //没有audio 流
				{
					iSendVideo = 1;
				}
				
				if (pSendPacket)
				{
					free(pSendPacket);
					pSendPacket = NULL;
				}
				//LOGE_jni("=============== drop  end ========to  %d \n",iLastTime);
			}
		}
	}
	RTMP_Log(RTMP_LOGINFO, "**** buffer_control end  *****\n");
	
	return TRUE;
}
