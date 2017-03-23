package com.common.livestream.jni;

import android.util.Log;

import com.common.livestream.data.RtmpPreference;
import com.common.livestream.liveplay.LivePlayProxy;
import com.common.livestream.liveplay.LivePlaySDK;
import com.common.livestream.liveplay.LivePlayProxy.RTMPStateListener;
import com.common.livestream.log.XCLog;
import com.common.livestream.mediarecorder.video.H264MediaCodecHandler;
import com.common.livestream.protocol.RtmpManager;
import com.common.livestream.statistics.StatisticsUtil;
import com.seu.magicfilter.utils.MethodCostUtils;

public class MediaRecorderUtils {
	
	public static native void test(String rtmpUrl);
	
	public static native void start(String rtmpUrl,int max_delay,
	        int wait_time,int discard_type,int send_window,int retrytime,int hasaudio,int hasvideo,
	        int audio_channel,int audio_sample,int audio_samplesize,int video_framerate,int video_width,int video_height,int profile,int video_samplesize);
	
	public static native void stop();
	
	public static native int getBufferCurrentTime();
	
	public static native int putVideoBuffer(byte[] pData,int size,int time);
	
	public static native int putAudioBuffer(byte[] pData,int size,int time);
	
	public static native void processBuffer(byte[] src, int width_s,
			int height_s,byte[] dst, int width, int height,int rotate,int type);
	

	
	public static native int convert(byte[] src,byte[] des,int width,int height,int type);

	public static native void createTestImage(byte[] mInitialImage,int mSize);
	
	public static native void convert2Nv21(byte[] buffer, byte[] src, int stride, int sliceHeight,
			int planar,int mWidth,int mSize);
	
	public static native int setNetChange();
	public static native void muteMic(byte[] data,int size);
	
	public static int curState ;
	/**
	 * @since v1.0.4  状态改变的回调  {@link RtmpState.Class}
	 * jni 回调java
	 * @param stateCode  错误码
	 */
	public static void onStatusChange(int stateCode){
		StatisticsUtil.savaEvent(stateCode,0,0);
		curState = stateCode;
		XCLog.logError("---", "---===---  "+stateCode);
		 RTMPStateListener listener = LivePlayProxy.getInstance().getRTMPStateListener();
		switch (stateCode) {
		case RtmpState.RTMP_SERVER_INITIAL:
			RtmpManager.getInstance().getFlag().set(false);
			break;
		case RtmpState.RTMP_SERVER_CONNECT_SUCCESS:
			RtmpManager.getInstance().getFlag().set(false);
			
			break;
		case RtmpState.RTMP_SERVER_CONNECT_FAIL:
			RtmpManager.getInstance().getFlag().set(false);
			if (listener!=null) {
				listener.onConnectFailed();
			}
			break;
		case RtmpState.RTMP_SERVER_SENDING:
			RtmpManager.getInstance().getFlag().set(true);
			if (listener!=null) {
				listener.onConnectSucc();
			}
			break;
		case RtmpState.RTMP_SERVER_STOP:
			RtmpManager.getInstance().getFlag().set(false);
			if (listener!=null) { 
				listener.onStreamDisconnect(RtmpState.RTMP_SERVER_STOP);
			}
			break;
		case RtmpState.RTMP_SERVER_RECONNECT_FAIL:
			RtmpManager.getInstance().getFlag().set(false);
			if (listener!=null) { 
				listener.onStreamDisconnect(RtmpState.RTMP_SERVER_RECONNECT_FAIL);
			}
			break;
		case RtmpState.RTMP_SERVER_RECONNECT_START:
			RtmpManager.getInstance().getFlag().set(false);
			if (listener!=null) { 
				listener.onStreamDisconnect(RtmpState.RTMP_SERVER_RECONNECT_START);
			}
			break;
		case RtmpState.RTMP_SERVER_RECONNECT_SUCCESS:
			RtmpManager.getInstance().getFlag().set(true);
			if (listener!=null) { 
				listener.onStreamDisconnect(RtmpState.RTMP_SERVER_RECONNECT_SUCCESS);
			}
			break;
		case RtmpState.NETWORK_BAD:
			RtmpManager.getInstance().getFlag().set(true);
			if (listener!=null) { 
				listener.onStreamDisconnect(RtmpState.NETWORK_BAD);
			}
			break;
		case RtmpState.NETWORK_GOOD:
			break;
		case RtmpState.ONLY_AUDIO:
			break;
		case RtmpState.AUDIO_VIDEO:
			break;

		default:
			break;
		}
	}
	
	

	/**
	 * 推流速度回调
	 * @param speed
	 */
	public static int onSpeedChange(int speed){
		XCLog.logError("---", "---===---onSpeedChange  "+speed);
		RTMPStateListener listener = LivePlayProxy.getInstance().getRTMPStateListener();
		if(listener != null){
			return listener.onSpeedChange(speed);
		}
		
		return -100;
	}
	
	
	/**
	 * 保存配置
	 * @param key
	 * @param value
	 */
	public static void setProperties(String key,String value){
//		XCLog.logError("---", "---===---setProperties  key="+key+" value="+value);
		if (key.startsWith("====")) {
			Log.e("MethodCostUtil_function", " setProperties："+value);
			return;
		}
		
		if (key.startsWith("pts=")) {
			long tag3 = System.currentTimeMillis();
			String[] time = key.split("=");
			Long pts = Long.parseLong(time[1]);
			Long tag1 = H264MediaCodecHandler.java_ts.get(pts);
			Long duration_L = H264MediaCodecHandler.java_timeCost.get(pts);
			if (pts!=null&&tag1!=null&&duration_L!=null) {
				long duration12 = duration_L;
				XCLog.logError("MethodCostUtil", "###java 总耗时："+duration12+" rtmp库 总耗时 "+(tag3-tag1-duration12)+" 推流总耗时: "+(tag3-tag1));
			}
			
			return ;
		}
		
		RtmpPreference.setProperties(key, value);
	}
	
	/**
	 * 获取配置
	 * @param key
	 */
	public static String getProperties(String key){
		XCLog.logError("---", "---===---getProperties  key="+key);
		return RtmpPreference.getProperteis(key);
	}

	public static void setValue(String key,String value){
		XCLog.logError("---", "---===---setValue  key="+key+" value="+value);
		if ("value_publish_rtmp_real_url".equals(key)) {
			StatisticsUtil.saveEvent("portalUrl", value);
		}else{
			StatisticsUtil.saveEvent(key, value);
		}
	}

	
}
