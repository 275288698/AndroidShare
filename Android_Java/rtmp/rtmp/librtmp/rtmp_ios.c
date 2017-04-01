
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "librtmp/rtmp_buffer.h"
#include "librtmp/log.h"
#include "librtmp/rtmp_aac.h"
#include "librtmp/Rtmp_H264.h"
#include "librtmp/rtmp_protocol.h"
#include "librtmp/rtmp_metadata.h"
#include "librtmp/rtmp_thread.h"
#include "StasticEvent.h"
#include "librtmp/rtmp.h"
#include "rtmp_ios.h"


void (*g_func_rtmpSetProc)(char *, char *);
char *(* g_func_rtmpGetProc)(char *);
void (*g_func_rtmpSetStatValue)(char *, char *);


#if defined(_LINUX_IOS_)
LIBRTMP *pLibrtmp;
#endif


int connectRtmp(LIBRTMP *pLibrtmp)
{
    if(!pLibrtmp || !pLibrtmp->pConfig || !pLibrtmp->pConfig->pRTMP)
        return FALSE;
    BufferConfig *pConfig = pLibrtmp->pConfig;
    char *sUrl = pLibrtmp->sUrl;
    
    if(pConfig->pRTMP)
    {
        RTMP_Log(RTMP_LOGINFO,"rtmp connect begin.....\n");
        
        char rtmp_url[500] = {'\0'};
        RTMP_Init(pConfig->pRTMP);
        pConfig->pRTMP->m_os = 2;
		
        pLibrtmp->pConfig->pRTMP->interrupt_callback.callback=isInterrupted;
        pLibrtmp->pConfig->pRTMP->interrupt_callback.opaque=pLibrtmp->pConfig;
		
        if (strstr(sUrl, "rtmp://gb.push.live.kankan.com") != NULL)
        {
            memcpy(&pConfig->pRTMP->access,pLibrtmp->pAccess,sizeof(RTMPAccess));
            
            if(!RTMP_Request_Access(pConfig->pRTMP,sUrl,rtmp_url))
            {
                printf("request push access fail ,%s \n",rtmp_url);
                
                pthread_mutex_lock(&pConfig->lock);
                pConfig->state = connect_fail;
                pLibrtmp->RTMPSetStatus(pConfig->state);
                pConfig->bKeyFramePutted = FALSE;
                pthread_mutex_unlock(&pConfig->lock);
                return FALSE;
            }
            
            memcpy(pLibrtmp->pAccess,&pConfig->pRTMP->access,sizeof(RTMPAccess));
        }
        else
        {
            memcpy(rtmp_url,sUrl,strlen(sUrl));//add for no key
        }
        memcpy(pLibrtmp->pAccess->src_url,sUrl,strlen(sUrl));
        
        RTMP_Log(RTMP_LOGINFO,"real rtmp url:%s \n",rtmp_url);
        
        int ret = RTMP_Connect_kk(pConfig->pRTMP,rtmp_url);
        
        printf("rtmp connect end..... %d \n",pConfig->pRTMP->m_sb.sb_socket);
        if(ret == FALSE)
        {
            pthread_mutex_lock(&pConfig->lock);
            pConfig->state = connect_fail;
            pConfig->bKeyFramePutted = FALSE;
            pLibrtmp->RTMPSetStatus(pConfig->state);//yiqin todo
            pthread_mutex_unlock(&pConfig->lock);
        }
        else
        {
            pthread_mutex_lock(&pConfig->lock);
            pConfig->state = connect_success;
            pLibrtmp->RTMPSetStatus(pConfig->state);//todo yiqin
            pConfig->bKeyFramePutted = FALSE;
            RTMP_Log(RTMP_LOGINFO," change state to : connect success \n");
            pthread_mutex_unlock(&pConfig->lock);
            ret = RTMP_SendChunkSize(pConfig->pRTMP);
            if(ret == FALSE)
                return ret;
            
            ret=SendMetadataPacket(pConfig->pRTMP,pLibrtmp->pMetaData);
            if(ret == FALSE)
                return ret;
            
            printf("send metadata :%d \n",ret);
           
            ret = RTMP_AAC_SequenceHeader(pConfig->pRTMP,pLibrtmp->pAacConfig,pConfig->pAudioBuffer);
            if(ret == FALSE)
                return ret;
            
            printf("==== put aac sequence header .ret:%d",ret);
            
            SetBufferStartTime(pConfig);
            
        }
        
        return ret;
    }
    else
        return FALSE;
}

void Librtmp_FreeBufferConfig(LIBRTMP *pLibrtmp)
{
    if(!pLibrtmp)
        return;
    
    FreeBufferConfig(pLibrtmp->pConfig);
    free(pLibrtmp->pMetaData);
    free(pLibrtmp->pAacConfig);
    free(pLibrtmp->pAccess);
    pLibrtmp->pConfig=NULL;
    pLibrtmp->pMetaData=NULL;
    pLibrtmp->pAacConfig=NULL;
    pLibrtmp->pAccess=NULL;
}

int StartSend(LIBRTMP *pLibrtmp)
{
    if(!pLibrtmp)
        return FALSE;
    
    printf("===RTMP :JNI  start send BEGIN====");
    BufferConfig *pConfig = pLibrtmp->pConfig;
    int ret = TRUE;
    if((pConfig->state) == connect_success)
    {
        pthread_mutex_lock(&pConfig->lock);
        pConfig->state = send_status;
        pLibrtmp->RTMPSetStatus(pConfig->state);
        RTMP_Log(RTMP_LOGINFO," change state to : send data \n");
       
        pthread_mutex_unlock(&pConfig->lock);
        
        ret = Buffer_Control(pLibrtmp->pMetaData, pConfig,pLibrtmp->pAccess,pLibrtmp->RTMPSetStatus);
        
        pthread_mutex_lock(&pConfig->lock);
        if(pConfig->state != stop_send)
        {
            pLibrtmp->RTMPSetStatus(stop_send_abnormal);
            pConfig->bKeyFramePutted = FALSE;
            RTMP_Log(RTMP_LOGINFO," change state to : reconnect fail \n");
        }
        pConfig->state = disconnect_status;
        pthread_mutex_unlock(&pConfig->lock);
        printf("==== RTMP:JNI START SEND END ,ret:%d",ret);
        return ret;
    }
    else
        return FALSE;
}


void StopSend(LIBRTMP *pLibrtmp)
{
    if(!pLibrtmp || !pLibrtmp->pConfig)
        return;
    
    BufferConfig *pConfig = pLibrtmp->pConfig;
    if(pConfig)
    {
        pthread_mutex_lock(&pConfig->lock);
        if(pConfig->state == send_status)
            pConfig->state = stop_send;
        else
            pConfig->state = disconnect_status;
        
        RTMP_Log(RTMP_LOGINFO," push : change state to : stop \n");
        pthread_mutex_unlock(&pConfig->lock);
    
        printf("rtmp stop send,state:%d \n",pConfig->state);
    }
}


void *start_send(void *pLibrtmp)
{
    LIBRTMP *librtmp = (LIBRTMP *)pLibrtmp;
    
    if(!librtmp || !librtmp->pConfig)
        return NULL;
    
    int ret = 0;
    ret = connectRtmp(librtmp);
    if(ret == 1)
        StartSend(librtmp);
    //close socket
    if(librtmp->pConfig->pRTMP)
    {
        RTMP_Close_kk(librtmp->pConfig->pRTMP);
        
    }
    pthread_mutex_lock(&librtmp->pConfig->lock);
    
    librtmp->pConfig->state = disconnect_status;
    
    pthread_mutex_unlock(&librtmp->pConfig->lock);
    
    //wait for speed thread stop,when state == 3 speed thread stop
    pthread_join(librtmp->speed_thread,NULL);
    
    Librtmp_FreeBufferConfig(pLibrtmp);
    
    librtmp->RTMPSetStatus(disconnect_status);
    
    RTMP_Log(RTMP_LOGINFO," change state to : disconnect \n");
    
    if(librtmp && librtmp->relievebind)
        librtmp->relievebind();
    
    return NULL;
}
#define PACKET_LOSS_MIN 0
#define PACKET_LOSS_MAX 30
// calculate drop/send
int change_bitrate(int drop_data,int send_data)
{
    if((send_data+drop_data) == 0)
        return 0;
    float packet_loss=(float)(drop_data*100/(send_data+drop_data));
    
    RTMP_Log(RTMP_LOGINFO,"push : drop/send+drop %f \n",packet_loss);
    
    if(packet_loss <= PACKET_LOSS_MIN)
        return 1;
    else if(packet_loss >= PACKET_LOSS_MAX )
        return -1;
    else
        return 0;
}
#define TIME_INTERVAL 10

void *speed_calculate(void *pLibrtmp)
{
	LIBRTMP *librtmp=(LIBRTMP *)pLibrtmp;
	if(!librtmp || !librtmp->pConfig || !librtmp->pConfig->pRTMP)
		return NULL;
	
	RTMP *pRtmp=librtmp->pConfig->pRTMP;
	int state_rtmp = init_status;
	int send_data = 0;
	int drop_data = 0;
	int want_bitrate = 0;
	int time_interval = 0;
	int pre_bitrate = -1;
	int new_bitrate = -1;
	int last_drop_data = -1;
	int stable_drop_data_times = 0;
	int stable_min_bitrate_times = 0;
	int stable_max_bitrate_times = 0;
    int sleep_time = 0;
	
    RTMP_Log(RTMP_LOGINFO," speed thread begin\n");
    
    while (1)
	{
        //read state from bufferconfig
        pthread_mutex_lock(&librtmp->pConfig->lock);
        state_rtmp = librtmp->pConfig->state;
        pthread_mutex_unlock(&librtmp->pConfig->lock);
        
        if(state_rtmp == disconnect_status)
            break;
        //send data
        pthread_mutex_lock(&pRtmp->lock_speed);
		send_data=pRtmp->m_send_data / 1024;
        pthread_mutex_unlock(&pRtmp->lock_speed);
        
        //drop data
        pthread_mutex_lock(&pRtmp->lock_drop);
        drop_data=pRtmp->m_drop_data / 1024;
        pthread_mutex_unlock(&pRtmp->lock_drop);
        
        //RTMP_Log(RTMP_LOGINFO," push : [before]drop : %d  last drop:%d   stable_drop_data_times:%d \n",drop_data,last_drop_data,stable_drop_data_times);
        if(drop_data == last_drop_data)
        {
            stable_drop_data_times++;
        }
        else{
            stable_drop_data_times = 0;
        }
        if(stable_drop_data_times >= 60)
		{
            pthread_mutex_lock(&pRtmp->lock_drop);
            drop_data=pRtmp->m_drop_data=0;
            pthread_mutex_unlock(&pRtmp->lock_drop);
            stable_drop_data_times = 0;
        }
        last_drop_data = drop_data;
        
        //RTMP_Log(RTMP_LOGINFO," push : [after]drop : %d  last drop:%d   stable_drop_data_times:%d \n",drop_data,last_drop_data,stable_drop_data_times);
        
        int iChangebitrate=change_bitrate(drop_data,send_data);
        
        RTMP_Log(RTMP_LOGINFO, "push: drop: %d  send:%d  iChangebitrate:%d", drop_data, send_data, iChangebitrate);
        
        if(iChangebitrate && librtmp->WantBitrate && time_interval>TIME_INTERVAL)
        {
            if(iChangebitrate == 1)
            {
                pthread_mutex_lock(&librtmp->pConfig->lock_net_change);
                if(librtmp->pConfig->net_change == 1)
                {
					librtmp->pConfig->net_change = 0;
                    new_bitrate=librtmp->WantBitrate(iChangebitrate);
                }
                pthread_mutex_unlock(&librtmp->pConfig->lock_net_change);
            }
            else
            {
                pthread_mutex_lock(&librtmp->pConfig->lock_net_change);
                librtmp->pConfig->net_change = 0;
                pthread_mutex_unlock(&librtmp->pConfig->lock_net_change);
                
                new_bitrate=librtmp->WantBitrate(iChangebitrate);
            }
            
            if((iChangebitrate == -1)&& (pre_bitrate == new_bitrate) && (new_bitrate != -1))//bitrate is min
            {
                pthread_mutex_lock(&librtmp->pConfig->lock_min_bitrate_status);
                librtmp->pConfig->min_bitrate_status=1;
                stable_min_bitrate_times++;
                pthread_mutex_unlock(&librtmp->pConfig->lock_min_bitrate_status);
            }
            else
            {
                pthread_mutex_lock(&librtmp->pConfig->lock_min_bitrate_status);
                librtmp->pConfig->min_bitrate_status = 0;
                stable_min_bitrate_times = 0;
                pthread_mutex_unlock(&librtmp->pConfig->lock_min_bitrate_status);
            }
			
			if ((iChangebitrate == 1) && (pre_bitrate == new_bitrate))
			{
				stable_max_bitrate_times++;
			}
			else
			{
				stable_max_bitrate_times = 0;
			}
			
            pre_bitrate=new_bitrate;
			
            if(stable_min_bitrate_times >= 6)
            {
                librtmp->RTMPSetStatus(network_poor);
                stable_min_bitrate_times = 0;
                RTMP_Log(RTMP_LOGINFO, "push: set network poor");
            }
			
			if (stable_max_bitrate_times >= 12)
			{
				librtmp->RTMPSetStatus(network_good);
				stable_max_bitrate_times = 0;
				RTMP_Log(RTMP_LOGINFO, "push: set network good");
			}
				
            time_interval=-1;
            
            //send data
            pthread_mutex_lock(&pRtmp->lock_speed);
            pRtmp->m_send_data=0;
            pthread_mutex_unlock(&pRtmp->lock_speed);
            
            //drop data
            pthread_mutex_lock(&pRtmp->lock_drop);
            pRtmp->m_drop_data=0;
            pthread_mutex_unlock(&pRtmp->lock_drop);
        }
        sleep_time = 1000;
        while (sleep_time > 0 && state_rtmp != disconnect_status)
        {
            pthread_mutex_lock(&librtmp->pConfig->lock);//yiqin todo
            state_rtmp = librtmp->pConfig->state;
            pthread_mutex_unlock(&librtmp->pConfig->lock);
            usleep(10 * 1000);
            sleep_time -= 10;
        }
        time_interval++;
    }
    
    RTMP_Log(RTMP_LOGINFO," speed thread stop");
    return NULL;
}

#if defined(_LINUX_IOS_)
int Librtmp_Stop()
#else
int Librtmp_Stop(LIBRTMP *pLibrtmp)
#endif
{
    if(!pLibrtmp)
        return FALSE;
    
    StopSend(pLibrtmp);
    return TRUE;
}

int Librtmp_InitBufferConfig(LIBRTMP *pLibrtmp, int max_delay, int wait_time, int discard_type, int send_window, int retrytime)
{   
    if (pLibrtmp == NULL)
        return FALSE;

    if ((pLibrtmp->pAacConfig = (RTMP_AAC_ASC *)calloc(1, sizeof(RTMP_AAC_ASC))) == NULL)
        return FALSE;
    
    if ((pLibrtmp->pMetaData = (RTMPMetadata *)calloc(1, sizeof(RTMPMetadata))) == NULL)
        return FALSE;
    
    if ((pLibrtmp->pConfig = (BufferConfig *)calloc(1, sizeof(BufferConfig))) == NULL)
        return FALSE;
    
    if ((pLibrtmp->pAccess = (RTMPAccess *)calloc(1, sizeof(RTMPAccess))) == NULL)
        return FALSE;
    
    InitBufferConfig(pLibrtmp->pConfig, max_delay, wait_time, discard_type, send_window, retrytime);

    return TRUE;
}

void Librtmp_SetParam(LIBRTMP *pLibrtmp, RTMP_METADATA *pParam)
{
    if(!pLibrtmp)
        return;
    
    RTMPMetadata *pMetaData=pLibrtmp->pMetaData;
    RTMP_AAC_ASC *pAacConfig=pLibrtmp->pAacConfig;
    
    //printf("librtmp audio: %d %d %d %d \n,",audio_channel,audio_sample,audio_samplesize,profile);
    RTMP_Set_AAC_SequenceHeader(pAacConfig,pParam->audio_channel,pParam->audio_sample,pParam->audio_samplesize,pParam->profile);
    
    
    pMetaData->bHasAudio=pParam->hasaudio;
    pMetaData->bHasVideo=pParam->hasvideo;
    pMetaData->nAudioChannels=pParam->audio_channel;
    pMetaData->nAudioSampleRate=pParam->audio_sample;
    pMetaData->nAudioSampleSize=pParam->audio_samplesize;
    pMetaData->nFrameRate=pParam->video_framerate;
    pMetaData->nHeight=pParam->video_height;
    pMetaData->nWidth=pParam->video_width;
    pMetaData->nVideoDataRate=pParam->bitrate;
}

int Librtmp_SetPropertiesCallback(void (*SetProp)(char *, char *), char *(* GetProp)(char *))
{
    g_func_rtmpSetProc = SetProp;
    g_func_rtmpGetProc = GetProp;
	return TRUE;
}

int Librtmp_SetValueCallback(void (*SetStatValue)(char *, char *))
{
	g_func_rtmpSetStatValue = SetStatValue;
	return TRUE;
}

#if defined(_LINUX_IOS_)
int Librtmp_start(char *sUrl, int max_delay, int wait_time, int discard_type, int send_window, int retrytime,
    void (*SetStatus)(int), RTMP_METADATA *pMetadata, void (*relievebind)(), int (*WantBitrate)(int))
#else
int Librtmp_start(LIBRTMP *pLibrtmp, char *sUrl, int max_delay, int wait_time, int discard_type, int send_window, int retrytime,
    void (*SetStatus)(int), RTMP_METADATA *pMetadata, void (*relievebind)(), int (*WantBitrate)(int))
#endif
{
#if defined(_LINUX_IOS_)
	pLibrtmp = (LIBRTMP *)calloc(1, sizeof(LIBRTMP));
#endif
    if(!pLibrtmp)
        return FALSE;
    RTMP_Log(RTMP_LOGINFO, "push : Librtmp_start begin");
    if(!Librtmp_InitBufferConfig(pLibrtmp, max_delay, wait_time, discard_type, send_window, retrytime))
    {
        RTMP_Log(RTMP_LOGINFO, "push : Librtmp_start init buffer config fail");
        return FALSE;
    }
    
    memcpy(pLibrtmp->sUrl, sUrl, strlen(sUrl));
   
    Librtmp_SetParam(pLibrtmp, pMetadata);
   
    pLibrtmp->RTMPSetStatus = SetStatus;
    pLibrtmp->relievebind = relievebind;
    pLibrtmp->WantBitrate = WantBitrate;
    int ret = pthread_create(&pLibrtmp->work_thread, NULL, start_send, pLibrtmp);//0:success
    
    if(ret)
    {
        RTMP_Log(RTMP_LOGINFO," create work thread fail \n");
        return FALSE;
    }
   
//    ret = pthread_create(&pLibrtmp->speed_thread, NULL, speed_calculate, pLibrtmp);//0:success
    
    if(ret)
    {
        RTMP_Log(RTMP_LOGINFO," create speed thread fail \n");
        return FALSE;
    }
    
    return TRUE;
}

#if defined(_LINUX_IOS_)
int Librtmp_PutVideoBuffer(char *data, int size, unsigned int pts, unsigned int dts)
#else
int Librtmp_PutVideoBuffer(LIBRTMP *pLibrtmp, char *data, int size, int time)
#endif
{
    if(!pLibrtmp)
        return FALSE;
    
    BufferConfig *pConfig = pLibrtmp->pConfig;
    if (!pConfig || !pConfig->pVideoBuffer || !pConfig->pRTMP)
        return FALSE;
    
    pthread_mutex_lock(&pConfig->lock);
    int status = pConfig->state;
    pthread_mutex_unlock(&pConfig->lock);

    int ret = FALSE;
	if (status == send_status)
	{
#if defined(_LINUX_IOS_)
		ret = RTMP_Packet_H264(pConfig->pRTMP, data, size, pts, dts, pConfig->pVideoBuffer);
#else
		ret = RTMP_Packet_H264(pConfig->pRTMP, data, size, time, time, pConfig->pVideoBuffer);
#endif
	}
	else
	{
		ret = FALSE;
	}
    
    return ret;
}

#if defined(_LINUX_IOS_)
int Librtmp_PutAudioBuffer(char * data, int size, int time)
#else
int Librtmp_PutAudioBuffer(LIBRTMP *pLibrtmp, char * data, int size, int time)
#endif
{
    if(!pLibrtmp)
        return FALSE;
    
    BufferConfig *pConfig=pLibrtmp->pConfig;
    
    if(!pConfig || !pConfig->pAudioBuffer || !pConfig->pRTMP)
        return FALSE;
    
    pthread_mutex_lock(&pConfig->lock);
    
    int status = pConfig->state;
    
    pthread_mutex_unlock(&pConfig->lock);
    int ret = 0;
    
    if (status == send_status)
    {
        
        ret = RTMP_Packet_AAC(pConfig->pRTMP,data,size,time,pConfig->pAudioBuffer);
        //printf("~~~ put audio packet into buffer  %d\n",ret);
    }
    
    if (status != send_status)
        ret = FALSE;

    return ret;
}

#if defined(_LINUX_IOS_)
int Librtmp_GetRTMPState()
#else
int Librtmp_GetRTMPState(LIBRTMP *pLibrtmp)
#endif
{
    if(!pLibrtmp || !pLibrtmp->pConfig)
        return FALSE;
    
    pthread_mutex_lock(&pLibrtmp->pConfig->lock);
    int status = pLibrtmp->pConfig->state;
    pthread_mutex_unlock(&pLibrtmp->pConfig->lock);
    return status;
}

int Librtmp_GetBufferCurrentTime(LIBRTMP *pLibrtmp)
{
    if(!pLibrtmp || !pLibrtmp->pConfig)
        return FALSE;
    
    return GetBufferCurrentTime(pLibrtmp->pConfig);
}

#if defined(_LINUX_IOS_)
void Librtmp_SetNetChange()
#else
void Librtmp_SetNetChange(LIBRTMP *pLibrtmp)
#endif
{
    if (pLibrtmp && pLibrtmp->pConfig)
    {
        pthread_mutex_lock(&pLibrtmp->pConfig->lock_net_change);
        pLibrtmp->pConfig->net_change = 1;
        pthread_mutex_unlock(&pLibrtmp->pConfig->lock_net_change);
        
        RTMP_Log(RTMP_LOGINFO," network is changed \n");
    }
}
