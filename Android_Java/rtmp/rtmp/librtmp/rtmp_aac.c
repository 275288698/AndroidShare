
#include "rtmp_aac.h"


/*
用于将收到的aac裸数据根据rtmp协议的格式打包进packet中，放入audio buffer中

sAudioData:aac裸数据
pAudioBuffer: audio缓冲区
*/
int RTMP_Packet_AAC(RTMP* pRtmp, unsigned char *sAudioData, int size, uint32_t nFrameTime, RTMPBuffer *pAudioBuffer)
{
	RTMPPacket *pPacket = NULL;
	pPacket = (RTMPPacket *)malloc(RTMP_HEAD_SIZE + size + 2);
	if (!pPacket)
		return FALSE;
	
	memset(pPacket, 0, RTMP_HEAD_SIZE + size + 2);
	pPacket->m_body = (char *)pPacket + RTMP_HEAD_SIZE;
	
	char *body = pPacket->m_body;
	int i = 0;
	body[i++] = pAudioBuffer->pAacConfig;
	body[i++] = 0x01;
	memcpy(&body[i], sAudioData, size);

	//填充packet字段
	pPacket->m_nBodySize = i + size;
	pPacket->m_nTimeStamp = nFrameTime;
	pPacket->m_hasAbsTimestamp = 0;
	pPacket->m_packetType = RTMP_PACKET_TYPE_AUDIO;
	pPacket->m_nChannel = 0x04;
	pPacket->m_nInfoField2 = pRtmp->m_stream_id;
	pPacket->m_headerType = RTMP_PACKET_SIZE_LARGE;
	
	//将packet放入buffer中
	if (!RTMP_Put_Buffer(pAudioBuffer, pPacket, 1))
	{
		if (pPacket)
		{
			free(pPacket);
			pPacket = NULL;
		}
		return FALSE;
	}
	
	return TRUE;
}

/*
将AAC 的sequence header 封装进packet
*/
int RTMP_AAC_SequenceHeader(RTMP* pRtmp, RTMP_AAC_ASC *pAacConfig, RTMPBuffer *pAudioBuffer)
{
	RTMPPacket *pPacket = NULL;
	pPacket = (RTMPPacket *)malloc(RTMP_HEAD_SIZE + 1024);
    if (!pPacket)
        return FALSE;
	
	memset(pPacket, 0, RTMP_HEAD_SIZE + 10);
	pPacket->m_body = (char *)pPacket + RTMP_HEAD_SIZE;
	
	char *body = pPacket->m_body;

	int i = 0;
	uint8_t audio_sequence = 0;
	audio_sequence |= ((pAacConfig->nSoundFormat << 4) & 0xF0);
	audio_sequence |= ((pAacConfig->nSoundRate << 2) & 0x0C);
	audio_sequence |= ((pAacConfig->nSoundSize << 1) & 0x02);
	audio_sequence |= (pAacConfig->nSoundType & 0x01);

	pAudioBuffer->pAacConfig = audio_sequence;

	body[i++] = audio_sequence;
	body[i++] = 0x00;

	//audio specification config
	uint16_t audio_specific_config=0;
	audio_specific_config |= ((pAacConfig->nSoundProfile << 11) & 0xF800);
	audio_specific_config |= ((pAacConfig->nSoundFrequencyIndex << 7) & 0x0780);
	audio_specific_config |= ((pAacConfig->nChannelConfiguration << 3) & 0x78);
	audio_specific_config |= 0 & 0x07;

	body[i++] = (audio_specific_config >> 8) & 0xFF;
	body[i++] = audio_specific_config & 0xFF;

	//填充packet字段
	pPacket->m_nBodySize = i;
	pPacket->m_nTimeStamp = 0;
	pPacket->m_hasAbsTimestamp = 0;
	pPacket->m_packetType = RTMP_PACKET_TYPE_AUDIO;
	pPacket->m_nChannel = 0x04;
	pPacket->m_nInfoField2 = pRtmp->m_stream_id;
	pPacket->m_headerType = RTMP_PACKET_SIZE_LARGE;

	//将video sequence config拷贝到buffer中
	memset(pAudioBuffer->pPacketConfig, 0x00, RTMP_HEAD_SIZE + 1024);
	pthread_mutex_lock(&pAudioBuffer->lock);
	memcpy(pAudioBuffer->pPacketConfig, pPacket, RTMP_HEAD_SIZE + 1024);
	pAudioBuffer->pPacketConfig->m_body= (char *)pAudioBuffer->pPacketConfig + RTMP_HEAD_SIZE;
	pthread_mutex_unlock(&pAudioBuffer->lock);

	//将packet放入buffer中
    printf("audio sh size:%d  %d  %d  %d\n", pPacket->m_nBodySize, audio_sequence, (audio_specific_config >> 8) & 0xFF, audio_specific_config & 0xFF);
    RTMP_SendPacket(pRtmp, pPacket, 0);
	if (!RTMP_Put_Buffer(pAudioBuffer, pPacket, 1))
    {
        if (pPacket)
        {
            free(pPacket);
            pPacket = NULL;
        }
		return FALSE;
    }

	return TRUE;
}

void RTMP_Set_AAC_SequenceHeader(RTMP_AAC_ASC *pAacConfig, int channel, int FrequencyIndex, int samplesize, int profile)
{
	memset(pAacConfig, 0x00, sizeof(RTMP_AAC_ASC));
	pAacConfig->nChannelConfiguration = channel;
	pAacConfig->nSoundType = channel - 1;
	pAacConfig->nSoundProfile = profile;
	pAacConfig->nSoundFormat = 10;
	
	switch(FrequencyIndex)
	{
		case 96000:
			pAacConfig->nSoundFrequencyIndex = 0;
			pAacConfig->nSoundRate = 3;
			break;
		case 88200:
			pAacConfig->nSoundFrequencyIndex = 1;
			pAacConfig->nSoundRate = 3;
			break;
		case 64000:
			pAacConfig->nSoundFrequencyIndex = 2;
			pAacConfig->nSoundRate = 3;
			break;
		case 48000:
			pAacConfig->nSoundFrequencyIndex = 3;
			pAacConfig->nSoundRate = 3;
			break;
		case 44100:
			pAacConfig->nSoundFrequencyIndex = 4;
			pAacConfig->nSoundRate = 3;
			break;
		case 32000:
			pAacConfig->nSoundFrequencyIndex = 5;
			pAacConfig->nSoundRate = 2;
			break;
		case 24000:
			pAacConfig->nSoundFrequencyIndex = 6;
			pAacConfig->nSoundRate = 1;
			break;
		case 22050:
			pAacConfig->nSoundFrequencyIndex = 7;
			pAacConfig->nSoundRate = 0;
			break;
		case 16000:
			pAacConfig->nSoundFrequencyIndex = 8;
			pAacConfig->nSoundRate = 0;
			break;

		default:
			pAacConfig->nSoundFrequencyIndex = 4;
			pAacConfig->nSoundRate = 3;
			break;
	}
	
	switch(samplesize)
	{
		case 8:
			pAacConfig->nSoundSize = 0;
			break;
		case 16:
			pAacConfig->nSoundSize = 1;
			break;
		default:
			pAacConfig->nSoundSize = 1;
			break;
	}
}
