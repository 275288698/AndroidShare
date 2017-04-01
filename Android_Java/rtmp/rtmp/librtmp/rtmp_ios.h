#ifndef __LIBRTMP_PUSH_H__
#define __LIBRTMP_PUSH_H__

#ifdef __cplusplus
extern "C"
{
#endif
    
#include <stdio.h>
#include <string.h>

#include "librtmp/rtmp.h"


typedef struct LIBRTMP
{
    char sUrl[500];
    BufferConfig *pConfig;
    
    RTMPMetadata *pMetaData;
    RTMP_AAC_ASC *pAacConfig;
    RTMPAccess *pAccess;
    pthread_t work_thread;
    void (*RTMPSetStatus)(int);
    void (*relievebind)();
    int (*WantBitrate)(int);
    pthread_t speed_thread;
}LIBRTMP;

typedef enum RTMPSTATUS
{
    connect_fail       = -1,
    init_status        = 0,
    connect_success    = 1,
    send_status        = 2,
    disconnect_status  = 3,
    stop_send          = 4,
    stop_send_abnormal = 5,
    reconnect_begin    = 6,
    reconnect_success  = 7,
    network_poor       = 8,
	network_good       = 9,
	publish_audio_only = 10,
	publish_all        = 11,
}RTMPSTATUS;

typedef struct RTMP_METADATA
{
    int hasaudio;
    int hasvideo;
    int audio_channel;
    int audio_sample;
    int audio_samplesize;
    int video_framerate;
    int video_width;
    int video_height;
    int profile;
    int bitrate;
}RTMP_METADATA;

int Librtmp_SetPropertiesCallback(void (*SetProp)(char *, char *), char *(* GetProp)(char *));
int Librtmp_SetValueCallback(void (*SetStatValue)(char *, char *));

#if defined(_LINUX_IOS_)
int Librtmp_start(char *sUrl, int max_delay, int wait_time, int discard_type, int send_window, int retrytime,
    void (*SetStatus)(int), RTMP_METADATA *pMetadata, void (*relievebind)(), int (*WantBitrate)(int));
#else
int Librtmp_start(LIBRTMP *pLibrtmp, char * sUrl, int max_delay, int wait_time, int discard_type, int send_window, int retrytime,
    void (*SetStatus)(int), RTMP_METADATA *pMetadata, void (*relievebind)(), int (*WantBitrate)(int));
#endif

#if defined(_LINUX_IOS_)
int Librtmp_Stop();
#else
int Librtmp_Stop(LIBRTMP *pLibrtmp);
#endif

#if defined(_LINUX_IOS_)
int Librtmp_GetBufferCurrentTime();
#else
int Librtmp_GetBufferCurrentTime(LIBRTMP *pLibrtmp);
#endif

#if defined(_LINUX_IOS_)
int Librtmp_PutVideoBuffer(char *data, int size, unsigned int pts, unsigned int dts);
#else
int Librtmp_PutVideoBuffer(LIBRTMP *pLibrtmp, char * data, int size, int time);
#endif

#if defined(_LINUX_IOS_)
int Librtmp_PutAudioBuffer(char *data, int size, int time);
#else
int Librtmp_PutAudioBuffer(LIBRTMP *pLibrtmp, char *data, int size, int time);
#endif

#if defined(_LINUX_IOS_)
void Librtmp_SetNetChange();
#else
void Librtmp_SetNetChange(LIBRTMP *pLibrtmp);
#endif

#ifdef __cplusplus
}
#endif
#endif
