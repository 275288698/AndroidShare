/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>

#ifndef _Included_com_common_jni_MediaRecorderUtils
#define _Included_com_common_jni_MediaRecorderUtils
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_common_android_jni_MediaRecorderUtils
 * Method:    connectRtmp
 * Signature: (Ljava/lang/String;)V
 */JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_start(
		JNIEnv *, jobject, jstring, jint, jint, jint, jint, jint, jint, jint,
		jint, jint, jint, jint, jint, jint, jint, jint);
/*
 * Class:     com_common_android_jni_MediaRecorderUtils
 * Method:    disconnect
 * Signature: ()V
 */JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_stop(
		JNIEnv *, jobject);
JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_setNetChange(
		JNIEnv *env, jobject obj);
/*
 * set buffer begin time
 */
//JNIEXPORT void JNICALL Java_com_common_android_jni_MediaRecorderUtils_SetBufferBeginTime(JNIEnv *, jobject,jint );
//JNIEXPORT void JNICALL Java_com_common_jni_MediaRecorderUtils_SetBufferStartTime();
JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_getBufferCurrentTime();
/*
 * put H264 data , packet time,packet size,into video buffer
 */JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_putVideoBuffer(
		JNIEnv *, jobject, jbyteArray, jint, jint);
 JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_test(
 		JNIEnv *, jobject, jstring);


/*
 * put AAC data,packet time,packet size ,into audio buffer
 */JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_putAudioBuffer(
		JNIEnv *, jobject, jbyteArray, jint, jint);

 /*
  * put AAC data,packet time,packet size ,into audio buffer
  */JNIEXPORT void JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_processBuffer(
 		JNIEnv *, jobject, jbyteArray, jint, jint, jbyteArray, jint, jint,jint,jint);

  JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_convert(
   		JNIEnv *, jobject, jbyteArray, jbyteArray, jint,jint,jint);

  JNIEXPORT void JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_createTestImage(
     		JNIEnv *, jobject, jbyteArray,jint);

  JNIEXPORT void JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_convert2Nv21(
     		JNIEnv *, jobject, jbyteArray, jbyteArray, jint,jint,jint,jint,jint);

  JNIEXPORT jint JNICALL Java_com_common_livestream_jni_MediaRecorderUtils_muteMic(
  		JNIEnv *, jobject, jbyteArray, jint);

void StateChange(int state);
int SpeedChange(int speed);
void setValue(char *key, char *value);
void SetProperties(char *key, char *value);
char* GetProperties(char *key);
void init(JNIEnv* env, jobject thiz);
void uninit();

#ifdef __cplusplus
}
#endif
#endif
