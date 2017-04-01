
/*
add by zongchunli@kankan.com

*/

#include <stdio.h>
#include <stdlib.h>

#include "rtmp.h"
#include "rtmp_aac.h"
#include "rtmp_buffer.h"
#include "Rtmp_H264.h"
#include "amf.h"

#include "thread.h"


int Buffer_Control(RTMPMetadata *pMetadata, BufferConfig *pConfig,RTMPAccess *pAccess,void (*ChangeState)(int));

void Set_Max_Delay(BufferConfig *pConfig,uint32_t iDelayTime);

void Set_Wait_Time(BufferConfig *pConfig,uint32_t iWaitTime);


