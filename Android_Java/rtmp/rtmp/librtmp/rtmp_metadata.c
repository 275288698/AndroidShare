

#include "rtmp_metadata.h"

char * put_bytes(char *c,char *bytes,int len)
{
	memcpy(c,bytes,len);
	return c + len;
}

char * put_byte( char *output, uint8_t nVal )    
{    
	output[0] = nVal;    
	return output+1;    
}  

char * put_be16(char *output, uint16_t nVal )    
{    
	output[1] = nVal & 0xff;    
	output[0] = nVal >> 8;    
	return output+2;    
}  

char * put_be24(char *output,uint32_t nVal )    
{    
	output[2] = nVal & 0xff;    
	output[1] = nVal >> 8;    
	output[0] = nVal >> 16;    
	return output+3;    
}    
char * put_be32(char *output, uint32_t nVal )    
{    
	output[3] = nVal & 0xff;    
	output[2] = nVal >> 8;    
	output[1] = nVal >> 16;    
	output[0] = nVal >> 24;    
	return output+4;    
}    
char *  put_be64( char *output, uint64_t nVal )    
{    
	output=put_be32( output, nVal >> 32 );    
	output=put_be32( output, nVal );    
	return output;    
}  

char * put_amf_bool(char *c, uint8_t b)
{
	*c++ = AMF_BOOLEAN;
	{
		unsigned char *ci, *co;    
		ci = (unsigned char *)&b;    
		co = (unsigned char *)c;
		co[0] = ci[0];
	}
	return c + 1;
}


char * put_amf_string( char *c, const char *str )    
{    
	uint16_t len = strlen( str );    
	c=put_be16( c, len );    
	memcpy(c,str,len);    
	return c+len;    
}    
char * put_amf_double( char *c, double d )    
{    
	*c++ = AMF_NUMBER;  /* type: Number */    
	{    
		unsigned char *ci, *co;    
		ci = (unsigned char *)&d;    
		co = (unsigned char *)c;    
		co[0] = ci[7];    
		co[1] = ci[6];    
		co[2] = ci[5];    
		co[3] = ci[4];    
		co[4] = ci[3];    
		co[5] = ci[2];    
		co[6] = ci[1];    
		co[7] = ci[0];    
	}    
	return c+8;    
} 


//////////////////////////////////////
int SendMetadataPacket(RTMP *pRtmp ,RTMPMetadata* lpMetaData)
{
	if(lpMetaData == NULL)  
	{  
		return FALSE;  
	}
	
	RTMPPacket *pPacket = NULL;
	pPacket = (RTMPPacket *)calloc(1, RTMP_HEAD_SIZE + 1024);
	pPacket->m_body = (char *)pPacket + RTMP_HEAD_SIZE;
	
	char * p = (char *)pPacket->m_body;
	p = put_byte(p, AMF_STRING );
	p = put_amf_string(p, "@setDataFrame");
	
	p = put_byte(p, AMF_STRING );
	p = put_amf_string(p, "onMetaData");
	
	p = put_byte(p, AMF_OBJECT );
	p = put_amf_string(p, "copyright");
	p = put_byte(p, AMF_STRING );
	p = put_amf_string(p, "pingan.com");
	
	p = put_amf_string(p, "hasVideo");
	p = put_amf_bool(p, lpMetaData->bHasVideo);
	
	p = put_amf_string(p, "hasAudio");
	p = put_amf_bool(p, lpMetaData->bHasAudio);
	
	p = put_amf_string(p, "width");
	p = put_amf_double(p, lpMetaData->nWidth);
	
	p = put_amf_string(p, "height");
	p = put_amf_double(p, lpMetaData->nHeight);
	
	p = put_amf_string(p, "audiocodecid");
	p = put_amf_double(p, FLV_CODECID_AAC);
	
	p = put_amf_string(p, "audiosamplerate");
	p = put_amf_double(p, lpMetaData->nAudioSampleRate);
	
	p = put_amf_string(p, "stereo");
	p = put_amf_bool(p, lpMetaData->nAudioChannels == 2);
	
	p = put_amf_string(p, "framerate");
	p = put_amf_double(p, lpMetaData->nFrameRate);
	
	p = put_amf_string(p, "videocodecid");
	p = put_amf_double(p, FLV_CODECID_H264);
	
	p = put_amf_string(p, "");
	p = put_byte(p, AMF_OBJECT_END);
	int index = p-pPacket->m_body;
	//SendPacket(RTMP_PACKET_TYPE_INFO,(unsigned char*)body,p-body,0);
	
	pPacket->m_nBodySize = index;
	pPacket->m_nTimeStamp = 0;
	pPacket->m_hasAbsTimestamp = 0;
	pPacket->m_packetType = RTMP_PACKET_TYPE_INFO;
	pPacket->m_nChannel = 0x04;
	
	pPacket->m_nInfoField2 = pRtmp->m_stream_id;
	
	pPacket->m_headerType = RTMP_PACKET_SIZE_LARGE;	

	RTMP_SendPacket(pRtmp,pPacket,0);

	free(pPacket);
	
	return TRUE;
}

