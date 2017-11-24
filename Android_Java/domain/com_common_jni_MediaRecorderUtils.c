#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>
#include "JNIUtils.h"
#include "librtmp/rtmp_ios.h"
#include "com_common_jni_MediaRecorderUtils.h"
#include "yuv.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

RTMP_METADATA *pMetadata = NULL;
LIBRTMP *pConfig = NULL;
jmethodID st_method_id;
jmethodID spd_method_id;
jmethodID setValue_method_id;
jmethodID setp_method_id;
jmethodID getp_method_id;
jobject mobj;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
	JNIInit(vm);
	JNIEnv* env = NULL;
	if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		return JNI_ERR;
	}
	return JNI_VERSION_1_4;
}

void StateChange(int state) {
	jthrowable exc;
	int needsDetach = 0;
	JNIEnv* env = getJNIEnv(&needsDetach);
	if (mobj != NULL) {
		(*env)->CallStaticVoidMethod(env, mobj, st_method_id, state);
	}

	/* 打印可能的错误信息并且清除，这里不处理异常 */
	exc = (*env)->ExceptionOccurred(env);
	if (exc) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}

	if (needsDetach) {
		JNIDetachCurrentThread();
	}
}

int SpeedChange(int speed) {
	int needsDetach = 0;
	JNIEnv* env = getJNIEnv(&needsDetach);
	int result = 0;
	if (mobj != NULL) {
		result = (*env)->CallStaticIntMethod(env, mobj, spd_method_id, speed);
	}
	if (needsDetach) {
		JNIDetachCurrentThread();
	}
	return result;
}

void setValue(char *key, char *value) {
	int needsDetach = 0;
	if (mobj != NULL) {
		JNIEnv* env = getJNIEnv(&needsDetach);
		jstring jkey = strToJstring(env, key);
		jstring jvalue = strToJstring(env, value);
		(*env)->CallStaticVoidMethod(env, mobj, setValue_method_id, jkey,jvalue);
		(*env)->DeleteLocalRef(env,jkey);
		(*env)->DeleteLocalRef(env,jvalue);
	}
	if (needsDetach) {
		JNIDetachCurrentThread();
	}
}

void SetProperties(char *key, char *value) {
	int needsDetach = 0;
	if (mobj != NULL) {
		JNIEnv* env = getJNIEnv(&needsDetach);
		jstring jkey = strToJstring(env, key);
		jstring jvalue = strToJstring(env, value);
		(*env)->CallStaticVoidMethod(env, mobj, setp_method_id, jkey, jvalue);
		(*env)->DeleteLocalRef(env,jkey);
		(*env)->DeleteLocalRef(env,jvalue);
	}
	if (needsDetach) {
		JNIDetachCurrentThread();
	}
}

// 调用之后需要手动释放内存
char* GetProperties(char *key) {
	int needsDetach = 0;
	JNIEnv* env = getJNIEnv(&needsDetach);
	jstring result;
	if (mobj != NULL) {
		jstring jkey = strToJstring(env, key);
		result = (*env)->CallStaticObjectMethod(env, mobj, getp_method_id,jkey);
		(*env)->DeleteLocalRef(env,jkey);
	}
	jsize len = (*env)->GetStringUTFLength(env, result);
	char* prop = (char *) malloc(len + 1);
	Jstring2CStr(env, result, prop, len + 1);
	if (needsDetach) {
		JNIDetachCurrentThread();
	}
	return prop;
}

void uninit() {
	free(pConfig);
	pConfig = NULL;

	free(pMetadata);
	pMetadata = NULL;

	int needsDetach = 0;
	JNIEnv* env = getJNIEnv(&needsDetach);
	if (mobj != NULL) {
		(*env)->DeleteGlobalRef(env, mobj);
		mobj = NULL;
	}
	if (needsDetach) {
		JNIDetachCurrentThread();
	}
}

void init(JNIEnv* env, jobject thiz) {
	mobj = (*env)->NewGlobalRef(env, thiz);
	jclass objclass = (*env)->FindClass(env,
			"com/common/livestream/jni/MediaRecorderUtils");
	st_method_id = (*env)->GetStaticMethodID(env, objclass, "onStatusChange",
			"(I)V");
	spd_method_id = (*env)->GetStaticMethodID(env, objclass, "onSpeedChange",
			"(I)I");
	setValue_method_id = (*env)->GetStaticMethodID(env, objclass, "setValue",
			"(Ljava/lang/String;Ljava/lang/String;)V");
	setp_method_id = (*env)->GetStaticMethodID(env, objclass, "setProperties",
			"(Ljava/lang/String;Ljava/lang/String;)V");
	getp_method_id = (*env)->GetStaticMethodID(env, objclass, "getProperties",
			"(Ljava/lang/String;)Ljava/lang/String;");
	(*env)->DeleteLocalRef(env, objclass);

}

JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_test(
 		JNIEnv *env, jobject obj, jstring rtmpUrl){
	init(env,obj);
	const char *sUrl = (*env)->GetStringUTFChars(env, rtmpUrl, NULL);

	struct addrinfo hints, *res, *aip;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		char hostname[100] = { '\0' };
		sprintf(hostname, "%s", sUrl);
		char SERVICE[10] = { '\0' };
		sprintf(SERVICE, "%d", 1935);

		int error = getaddrinfo(hostname, SERVICE, &hints, &res);
		  char s1[100],s2[100];



		if (error != 0)
		{
		    sprintf(s1,"====");
		    const char* errInfo = gai_strerror(error);

		    sprintf(s2,"getaddrinfo err:%d %s  %s",error,errInfo,hostname);
		    SetProperties(s1,s2);
			return HTTPRES_LOST_CONNECTION;
		}

		int sockfd = -1;


		sprintf(s1,"====");
		 sprintf(s2,"getaddrinfo ========== :%d  ",error);
		SetProperties(s1,s2);
		struct sockaddr_in *addr;
		 char ipbuf[16];
		for (aip = res; aip != NULL; aip = aip->ai_next)
		{
			 addr = (struct sockaddr_in *)aip->ai_addr;


			sprintf(s1,"====");
			 sprintf(s2,"getaddrinfo ========== :%s  ",  inet_ntop(AF_INET,
			            &addr->sin_addr, ipbuf, 16));
			SetProperties(s1,s2);


		}

		freeaddrinfo(res);

}

JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_start(
		JNIEnv *env, jobject obj, jstring rtmpUrl, jint max_delay,
		jint wait_time, jint discard_type, jint send_window, jint retrytime,
		jint hasaudio, jint hasvideo, jint audio_channel, jint audio_sample,
		jint audio_samplesize, jint video_framerate, jint video_width,
		jint video_height, jint profile, jint video_bitrate) {
	init(env, obj);
	const char *sUrl = (*env)->GetStringUTFChars(env, rtmpUrl, NULL);
	pConfig = (LIBRTMP *) calloc(1, sizeof(LIBRTMP));
	if (!pConfig)
		return 0;
	pMetadata = (RTMP_METADATA *) calloc(1, sizeof(RTMP_METADATA));
	if (!pMetadata)
		return 0;
	pMetadata->hasaudio = hasaudio;
	pMetadata->hasvideo = hasvideo;
	pMetadata->audio_channel = audio_channel;
	pMetadata->audio_sample = audio_sample;
	pMetadata->audio_samplesize = audio_samplesize;
	pMetadata->video_framerate = video_framerate;
	pMetadata->video_width = video_width;
	pMetadata->video_height = video_height;
	pMetadata->profile = profile;
	pMetadata->bitrate = video_bitrate;
	//int ret=Librtmp_start(pConfig,sUrl,max_delay,wait_time,discard_type,send_window,retrytime,StateChange,pMetadata,uninit,SpeedChange);
	Librtmp_SetPropertiesCallback(SetProperties, GetProperties);
	int ret = Librtmp_start(pConfig, sUrl, max_delay, wait_time, discard_type,
			send_window, retrytime, StateChange, pMetadata, uninit,
			SpeedChange);
	Librtmp_SetValueCallback(setValue);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_stop(
		JNIEnv *env, jobject obj) {
	if(pConfig){
		return Librtmp_Stop(pConfig);
	}
	return 0;
}

JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_setNetChange(
		JNIEnv *env, jobject obj) {
	if(pConfig){
		Librtmp_SetNetChange(pConfig);
		return 1;
	}
	return 0;
}

JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_getBufferCurrentTime() {
	if(pConfig){
		return Librtmp_GetBufferCurrentTime(pConfig);
	}
	return 0;
}
JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_putVideoBuffer(
		JNIEnv *env, jobject obj, jbyteArray data, jint size, jint time) {
	if (!pConfig)
		return 0;
	jbyte *byte_tmp = (*env)->GetByteArrayElements(env, data, 0);
	char *data_tmp = (char *) byte_tmp;

	int ret = 0;

	ret = Librtmp_PutVideoBuffer(pConfig, data_tmp, size, time);
	/*
	 if(ret!=1)
	 putDataError(ret);
	 */
	(*env)->ReleaseByteArrayElements(env, data, byte_tmp, 0);
	//ERROR("** put video packet result : %d  size: %d  time:%d \n",ret, size,time);

	return ret;
}



JNIEXPORT void JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_processBuffer(
		JNIEnv *env, jobject obj, jbyteArray srcData, jint width_s,
		jint height_s,jbyteArray dstData,  jint width,jint height,jint rotate,jint type){
	jbyte *byte_src = (*env)->GetByteArrayElements(env, srcData, 0);
	unsigned char *src = (unsigned char *) byte_src;
	jbyte *byte_dst = (*env)->GetByteArrayElements(env, dstData, 0);
	unsigned char *dst = (unsigned char *) byte_dst;
//	int buffLen = width*height*3/2;
//	unsigned char buff[buffLen];

	if(270 == rotate){
		rotate270(src,width_s,height_s, dst ,width,height,type);
	}else if(90 == rotate){
		rotate90(src,width_s,height_s,  dst,width,height,type);
	}

//	jbyteArray ret = (*env)->NewByteArray(env,buffLen);
//	(*env)->SetByteArrayRegion(env,ret,0,buffLen,buff);
	(*env)->ReleaseByteArrayElements(env, srcData, byte_src, 0);
	(*env)->ReleaseByteArrayElements(env, dstData, byte_dst, 0);
//	return ret;
 }

JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_convert(
  		JNIEnv *env, jobject obj, jbyteArray srcData, jbyteArray dstData, jint width,jint height,jint type){
		jbyte *byte_src = (*env)->GetByteArrayElements(env, srcData, 0);
		unsigned char *src = (unsigned char *) byte_src;
		jbyte *byte_dst = (*env)->GetByteArrayElements(env, dstData, 0);
		unsigned char *dst = (unsigned char *) byte_dst;

		int ret = convert(src,dst,width,height,type);

		(*env)->ReleaseByteArrayElements(env, srcData, byte_src, 0);
		(*env)->ReleaseByteArrayElements(env, dstData, byte_dst, 0);

		return ret;
}

JNIEXPORT void JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_createTestImage(
   		JNIEnv * env, jobject obj, jbyteArray srcData,jint size){
		jbyte *byte_src = (*env)->GetByteArrayElements(env, srcData, 0);
		unsigned char *src = (unsigned char *) byte_src;

		createTestImage(src,size);
		(*env)->ReleaseByteArrayElements(env, srcData, byte_src, 0);
}

JNIEXPORT void JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_convert2Nv21(
    		JNIEnv *env, jobject obj, jbyteArray dstData, jbyteArray srcData, jint stride,jint sliceHeight
    		,jint planar,jint mWidth,jint mSize){
		jbyte *byte_src = (*env)->GetByteArrayElements(env, srcData, 0);
		unsigned char *src = (unsigned char *) byte_src;
		jbyte *byte_dst = (*env)->GetByteArrayElements(env, dstData, 0);
		unsigned char *dst = (unsigned char *) byte_dst;


		convert2Nv21(dst,src,stride,sliceHeight,planar,mWidth,mSize);
		(*env)->ReleaseByteArrayElements(env, srcData, byte_src, 0);
		(*env)->ReleaseByteArrayElements(env, dstData, byte_dst, 0);
}


JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_putAudioBuffer(
		JNIEnv *env, jobject obj, jbyteArray data, jint size, jint time) {
	if (!pConfig)
		return 0;
	jbyte *byte_tmp = (*env)->GetByteArrayElements(env, data, 0);
	char *data_tmp = (char *) byte_tmp;
	int ret = Librtmp_PutAudioBuffer(pConfig, data_tmp, size, time);
	/*
	 if(ret != 1)
	 (*env)->putDataError(env,ret);
	 */
	(*env)->ReleaseByteArrayElements(env, data, byte_tmp, 0);
	//ERROR("== put audio packet result : %d  size : %d time :%d\n",ret,size,time);

	return ret;
}

JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_muteMic(
  		JNIEnv *env, jobject obj, jbyteArray data, jint size){
		jbyte *byte_tmp = (*env)->GetByteArrayElements(env, data, 0);
		char *data_tmp = (char *) byte_tmp;

		memset(data_tmp,0,size);

		(*env)->ReleaseByteArrayElements(env, data, byte_tmp, 0);
}

