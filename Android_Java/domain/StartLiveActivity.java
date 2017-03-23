package com.seu.magiccamera.activity;

import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import com.common.dns.DNSPaser;
import com.common.livestream.env.Env;
import com.common.livestream.jni.MediaRecorderUtils;
import com.common.livestream.liveplay.LivePlayProxy;
import com.common.livestream.liveplay.LivePlaySDK;
import com.common.livestream.log.XCLog;
import com.common.livestream.network.NetworkHelper;
import com.common.livestream.protocol.CodecWhiteListManage;
import com.common.livestream.utils.DeviceUtil;
import com.common.livestream.utils.IpUtil;
import com.common.livestream.utils.XCToast;
import com.pingan.avlive.utils.ChangeCDNUtils;
import com.pingan.college.media.liveclient.ILiveListener;
import com.pingan.college.media.liveclient.LiveBizType;
import com.pingan.college.media.liveclient.LiveClient;
import com.pingan.college.media.liveclient.LiveParam;
import com.pingan.college.media.liveclient.core.common.LiveLog;
import com.pingan.college.media.liveclient.entity.LiveAnchorRoomInfo;
import com.pingan.college.media.liveclient.entity.LiveQueryRoomInfo;
import com.pingan.im.interfaces.PAIMCallBack;
import com.pingan.im.interfaces.PAIMUserStatusListener;
import com.pingan.im.manager.PAIMManager;
import com.pingan.im.model.PAIMUser;
import com.pingan.im.type.PAIMAccountType;
import com.seu.magiccamera.R;
import com.seu.magiccamera.chat.ChatListActivity;
import com.seu.magiccamera.common.utils.IntentHelper;
import com.seu.magiccamera.widget.LiveListDialog;
import com.seu.magiccamera.widget.LiveListDialog.OnCommentDialogListener;
import com.seu.magicfilter.utils.MethodCostUtils;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.RadioGroup.OnCheckedChangeListener;
import android.widget.TextView;
import android.widget.Toast;

public class StartLiveActivity extends Activity {

	private static final String TAG = StartLiveActivity.class.getSimpleName();

	public static String PUSH_URL = "rtmp://test.ws.push.live.slw117.top/live";

	public static String PULL_URL = "rtmp://test.ws.pull.live.slw117.top/live";

	// 直播参数设置
	private static final String REG_USER_ID = "palive";
	private static final String REG_USER_PWD = "123456";
	private static final String ADMIN_USER_ID = "palive";
	private static final String ADMIN_USER_PWD = "123456";
	private static final String ANCHOR_USER_ID = "test1";
	private static final String ANCHOR_USER_PWD = "123456";
	private static final String VISITOR_USER_ID = "test1";
	private static final String VISITOR_USER_PWD = "123456";
	private static final String ANCHOR_ROOM_SUBJECT = "我的直播间";

	private LiveClient mClient;
	private LiveParam mParam;

	private String mRoomID;

	private String mRoomName;

	private LiveQueryRoomInfo mCurrentRoomInfo;

	private TextView mInformationTv;

	private SharedPreferences sharedPreferences;

	private TextView mChatView;

	private TextView mDebugMode;

	private RadioGroup mRadiaGroup, mRadiaGroup_cdn;

	private TextView mLogin;

	private TextView mLogout;

	private boolean isLogin;

	private TextView mVersionCode;

	private String loginStatus;

	private String phoneInformation;
	
	private CheckBox tokenCheckbox;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_live);
		loginStatus = getIntent().getStringExtra("loginStatus");
		phoneInformation = getIntent().getStringExtra("phoneInformation");

		initView();
		initLive();
	}

	private int cdn_type = 1;

	private void initView() {
		final EditText login = (EditText) findViewById(R.id.account);
		final EditText password = (EditText) findViewById(R.id.password);

		sharedPreferences = getSharedPreferences("IM", Context.MODE_PRIVATE);
		String logins = sharedPreferences.getString("loginIM", "");
		String passwords = sharedPreferences.getString("passwordIM", "");

		if (!TextUtils.isEmpty(logins) && !TextUtils.isEmpty(passwords)) {
			login.setText(logins);
			password.setText(passwords);
		}

		mLogin = (TextView) findViewById(R.id.login);
		mLogin.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				if (TextUtils.isEmpty(login.getText())
						|| TextUtils.isEmpty(password.getText())) {
					XCToast.showToast("帐号或密码不能为空！");
				}
				if (isLogin) {
					if (login.getText().toString()
							.equals(LiveParam.getInstance().getLoginAccount())) {
						doAction(LiveBizType.ROOM_INFO);
						return;
					}
					PAIMManager.getInstance().logout(new PAIMCallBack() {
						
						@Override
						public void onSuccess() {
							// TODO Auto-generated method stub
							
						}
						
						@Override
						public void onError(int errorcode, String errorString) {
							// TODO Auto-generated method stub
							
						}
					});
				}
				mLogin.setEnabled(false);
				mParam.setLoginAccount(login.getText().toString());
				mParam.setLoginPassword(password.getText().toString());
				doAction(LiveBizType.USER_LOGIN);
			}
		});

		mLogout = (TextView) findViewById(R.id.logout);
		mLogout.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				if (isLogin) {
					doAction(LiveBizType.USER_LOGOUT);
					mLogout.setEnabled(false);
				} else
					XCToast.showToast("请先登录");
			}
		});

		mInformationTv = (TextView) findViewById(R.id.information);

		mChatView = (TextView) findViewById(R.id.pivate_chat);
		mChatView.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				if (!isLogin) {
					XCToast.showToast("请先登录");
					return;
				}
				Intent intent = new Intent(StartLiveActivity.this,
						ChatListActivity.class);
				startActivity(intent);
			}
		});

		mRadiaGroup = (RadioGroup) findViewById(R.id.decode_type);
		mRadiaGroup.setOnCheckedChangeListener(new OnCheckedChangeListener() {

			@Override
			public void onCheckedChanged(RadioGroup group, int checkedId) {
				switch (checkedId) {
				case R.id.type_software:
					CodecWhiteListManage.getInstance().setHWVideoEncodeSupport(
							false);
					LivePlayProxy.getInstance().debug_switchOpengl(false);
					// XCToast.debugShowToast("设置软编码成功");
					break;
				case R.id.type_hardware:
					CodecWhiteListManage.getInstance().setHWVideoEncodeSupport(
							true);
					LivePlayProxy.getInstance().debug_switchOpengl(false);
					// XCToast.debugShowToast("设置硬编码成功");
					break;
				case R.id.type_opengl:
//					if(!LivePlayProxy.getInstance().isSupportOpengl()){
//						mRadiaGroup.check(R.id.type_hardware);
//						XCToast.showToast("该手机不支持美颜");
//						return ;
//					}
					CodecWhiteListManage.getInstance().setHWVideoEncodeSupport(
							true);
					LivePlayProxy.getInstance().debug_switchOpengl(true);
					// XCToast.debugShowToast("设置美颜硬编码成功");
					break;
				default:
					break;
				}
			}
		});

		mRadiaGroup_cdn = (RadioGroup) findViewById(R.id.cnd_type);
		mRadiaGroup_cdn
				.setOnCheckedChangeListener(new OnCheckedChangeListener() {

					@Override
					public void onCheckedChanged(RadioGroup group, int checkedId) {
						switch (checkedId) {
						case R.id.type_ws:
							cdn_type = 1;
							break;
						case R.id.type_xy:
							cdn_type = 2;
							break;
						case R.id.type_qn:
							cdn_type = 3;
							break;
						case R.id.type_pa:
							cdn_type = 4;
							break;
						default:
							break;
						}
						ChangeCDNUtils.currentCDNType = cdn_type;
						}
				});

		RadioButton btn = (RadioButton) findViewById(R.id.type_pa);
		btn.setChecked(true);

		mRadiaGroup.check(R.id.type_opengl);

		mDebugMode = (TextView) findViewById(R.id.debug_mode);
		mDebugMode.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				Intent intent = new Intent(StartLiveActivity.this,
						MainActivity.class);
				startActivity(intent);
			}
		});

		mVersionCode = (TextView) findViewById(R.id.version_code);

		if (!TextUtils.isEmpty(loginStatus)) {
			mInformationTv.setText(loginStatus);
		}

		if (!TextUtils.isEmpty(phoneInformation)) {
			mVersionCode.setText(phoneInformation);
		} else
			new Handler().postDelayed(new Runnable() {

				@Override
				public void run() {
					TelephonyManager mTelephonyMgr = (TelephonyManager) StartLiveActivity.this
							.getSystemService(Context.TELEPHONY_SERVICE);
					String IMSI = mTelephonyMgr.getSubscriberId();
					String providersName = "N/A";
					if (IMSI != null) {

						// IMSI号前面3位460是国家，紧接着后面2位00 02是中国移动，01是中国联通，03是中国电信。其中
						if (IMSI.startsWith("46000")
								|| IMSI.startsWith("46002")) {
							providersName = "ChinaMobile";
						} else if (IMSI.startsWith("46001")) {
							providersName = "ChinaUnicom";
						} else if (IMSI.startsWith("46003")) {
							providersName = "ChinaTelecom";
						}

					}

					boolean isTab = isTablet(StartLiveActivity.this);
					String phoneBand = android.os.Build.MODEL;
					// TODO Auto-generated method stub
					String notice = "";
					try {
						notice = "  平板："
								+ isTab
								+ "  sdk:"
								+ Build.VERSION.SDK_INT
								+ " \n screenWidth:"
								+ DeviceUtil.getScreenWidth()
								+ "  screenHeight:"
								+ DeviceUtil.getScreenHeight()
								+ "\n 网络类型："
								+ NetworkHelper.getInstance()
										.getCurrentNetworkTypeString()
								+ "\n 手机型号："
								+ phoneBand
								+ "\n sim供应商："
								+ providersName
								+ "\n sdk版本号："
								+ LivePlaySDK.getSdkVersion()
								+ "\n Demo版本号："
								+ getPackageManager().getPackageInfo(
										getPackageName(), 0).versionName;
					} catch (NameNotFoundException e) {
						e.printStackTrace();
					}
					if (!LivePlaySDK.getInstance().isEnableBeauty()
							&& Build.VERSION.SDK_INT >= 18) {
						notice = notice + "美颜：不支持";
					}
					mVersionCode.setText(notice);
				}
			}, 1500);

		PAIMManager.getInstance().setUserStatusListener(
				new PAIMUserStatusListener() {

					@Override
					public void onUserSigExpired() {
						XCToast.debugShowToast("用户信息已经过期，请重新登录");
						Intent intent = new Intent(StartLiveActivity.this,
								StartLiveActivity.class);
						intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
						intent.putExtra("loginStatus", "用户信息已经过期，请重新登录");
						if (mVersionCode.getText() != null) {
							intent.putExtra("phoneInformation", mVersionCode
									.getText().toString());
						}
						startActivity(intent);
					}

					@Override
					public void onForceOffline() {
						XCToast.debugShowToast("你被踢下线了");
						Intent intent = new Intent(StartLiveActivity.this,
								StartLiveActivity.class);
						intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
						intent.putExtra("loginStatus", "你被踢下线了");
						if (mVersionCode.getText() != null) {
							intent.putExtra("phoneInformation", mVersionCode
									.getText().toString());
						}
						startActivity(intent);
					}
				});
	
		
		
		tokenCheckbox = (CheckBox) findViewById(R.id.checkbox);
		tokenCheckbox.setOnCheckedChangeListener(new android.widget.CompoundButton.OnCheckedChangeListener() {
			
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				ChangeCDNUtils.token = isChecked;
			}
		});
	}

	public static boolean isTablet(Context context) {
		return (context.getResources().getConfiguration().screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK) >= Configuration.SCREENLAYOUT_SIZE_LARGE;
	}

	private void doAction(LiveBizType type) {
		mClient.doAction(type, mParam);
	}

	private void enterRoom() {
		if (mRoomID == null) {
			XCToast.debugShowToast("roomid 为空");
			return;
		}
		mInformationTv.setText("用户" + LiveParam.getInstance().getLoginAccount()
				+ "登录成功");
		if (LiveParam.getInstance().getLoginAccount()
				.equals(mCurrentRoomInfo.getUserAccount())) {
			// if
			// (LiveParam.getInstance().getLoginAccount().equals("chenwenlong"))
			// {
			LiveLog.v(TAG, "主播进入房间");
			Intent intent1 = new Intent();
			intent1.setClass(StartLiveActivity.this, IMAnchorActivity.class);
			intent1.putExtra(IntentHelper.INTENT_KEY_RTMP_PULL_URL, PUSH_URL);
			intent1.putExtra(IntentHelper.INTENT_KEY_LIVE_ID, mRoomID);
			intent1.putExtra(IntentHelper.INTENT_KEY_CDN, cdn_type);
			intent1.putExtra(IntentHelper.INTENT_KEY_TOKEN, LiveParam
					.getInstance().getUserInfo().getToken());
			intent1.putExtra(IntentHelper.INTENT_KEY_ROOMNAME, LiveParam
					.getInstance().getLoginAccount());
			// intent.putExtra(IntentHelper.INTENT_KEY_RTMP_PLAY_URL,
			// PLAY_URl);
			startActivity(intent1);
		} else {
			LiveLog.v(TAG, "观众进入房间");
			Intent intent2 = new Intent();
			intent2.setClass(StartLiveActivity.this, IMFansActivity.class);
			intent2.putExtra(IntentHelper.INTENT_KEY_LIVE_ID, mRoomID);
			intent2.putExtra(IntentHelper.INTENT_KEY_ROOMNAME, mRoomName);
			// intent.putExtra(IntentHelper.INTENT_KEY_RTMP_PULL_URL,
			// PULL_URL);
			intent2.putExtra(IntentHelper.INTENT_KEY_CDN, cdn_type);
			intent2.putExtra(IntentHelper.INTENT_KEY_RTMP_PLAY_URL, PULL_URL);
			intent2.putExtra(IntentHelper.INTENT_KEY_TOKEN, LiveParam
					.getInstance().getUserInfo().getToken());
			startActivity(intent2);
		}

	}

	private LiveBizType mState = null;

	private void initLive() {
		mClient = LiveClient.getInstance();
		mParam = LiveParam.getInstance();
		mClient.setLiveListener(new ILiveListener() {
			@Override
			public void onLiveEvent(LiveBizType type, LiveParam param) {
				mParam = param;
				mState = type;
				List<LiveQueryRoomInfo> roomInfoList;
				LiveAnchorRoomInfo anchorRoomInfo;
				switch (type) {
				case USER_REG:
					mParam.setLoginAccount(mParam.getRegAccount());
					mParam.setLoginPassword(mParam.getRegPassword());
					doAction(LiveBizType.USER_LOGIN);
					break;
				case USER_LOGIN:

					LiveParam.getInstance().setAnchorRoomId("");

					sharedPreferences
							.edit()
							.putString("loginIM",
									LiveParam.getInstance().getLoginAccount())
							.commit();
					sharedPreferences
							.edit()
							.putString("passwordIM",
									LiveParam.getInstance().getLoginPassword())
							.commit();

					if (LiveParam.getInstance().getUserInfo() == null
							|| TextUtils.isEmpty(LiveParam.getInstance()
									.getUserInfo().getToken())) {
						XCToast.debugShowToast("登录失败");
						mLogin.setEnabled(true);
						return;
					}
					PAIMManager.getInstance().init(Env.getContext());
					XCLog.logError("===", "==="+LiveParam.getInstance().getUserInfo().getUserId());
//					XCToast.showToast("==="+LiveParam.getInstance().getUserInfo().getUserId());
					PAIMUser user = new PAIMUser();
					user.setAccountType(PAIMAccountType.ACCOUNT_TYPE_DEPENDENT);
					user.setIdentifier(LiveParam.getInstance().getUserInfo().getUserId());
					PAIMManager.getInstance().login(
							user,
							LiveParam.getInstance().getUserInfo().getToken(),
							new PAIMCallBack() {

								@Override
								public void onSuccess() {
									// XCToast.debugShowToast("登录IM服务器成功");
									XCToast.showToast("用户"
											+ LiveParam.getInstance()
													.getLoginAccount() + "登录成功");
									mInformationTv
											.setText("用户"
													+ LiveParam.getInstance()
															.getLoginAccount()
													+ "登录成功");
									isLogin = true;
									mLogin.setEnabled(true);
									doAction(LiveBizType.ROOM_INFO);
								}

								@Override
								public void onError(int errorcode,
										String errorString) {
									XCToast.debugShowToast("登录IM服务器失败："
											+ errorcode + errorString);
									mLogin.setEnabled(true);
								}
							});

					break;
				case ROOM_INFO:
					roomInfoList = mParam.getQueryRoomInfoList();
					// showCoverDialog(roomInfoList, "直播间列表");
					Intent intent = new Intent(StartLiveActivity.this,
							LiveRoomListActivity.class);
					intent.putExtra(IntentHelper.INTENT_KEY_CDN, cdn_type);
					startActivity(intent);
					break;
				case ROOM_CREATE:
					mParam.setMasterRoomId(mParam.getMasterRoomId());
					mParam.setMasterRoomPublish(1);
					doAction(LiveBizType.ROOM_PUBLISH);
					break;
				case ROOM_PUBLISH:
					// if (mParam.getRoleType() == LiveRoleType.ANCHOR) {
					// // 主播查询自己的房间
					// mParam.setAnchorAccount(mParam.getLoginAccount());
					// }
					doAction(LiveBizType.ROOM_INFO);
					break;
				case ROOM_ENTRY:
					// 如果GetStreamUrl字段不为空，则需访问该返回url获取真正流地址
					// anchorRoomInfo = mParam.getAnchorRoomInfo();
					// mCurrentRoomInfo = anchorRoomInfo;
					// if (anchorRoomInfo != null &
					// anchorRoomInfo.getGetStreamUrl() != null) {
					// doAction(LiveBizType.ROOM_GET_ADDRESS);
					// mInformationTv.setText("正在获取地址，准备进入直播间...");
					// } else {
					// if (anchorRoomInfo.getStreamUrl() == null ||
					// anchorRoomInfo.getStreamUrl().isEmpty()) {
					// XCToast.debugShowToast("播放地址为空");
					// return;
					// }
					// LiveLog.v(TAG, "start playing " +
					// anchorRoomInfo.getStreamUrl().get(0));
					// PULL_URL = anchorRoomInfo.getStreamUrl().get(0);
					//
					// }

					break;
				case ROOM_GET_ADDRESS:
					// 获取到流地址
					anchorRoomInfo = mParam.getAnchorRoomInfo();
					if (!LiveParam.getInstance().getLoginAccount()
							.equals(anchorRoomInfo.getUserAccount())) {
						if (anchorRoomInfo.getStreamUrl() == null
								|| anchorRoomInfo.getStreamUrl().isEmpty()) {
							XCToast.debugShowToast("播放地址为空");
							return;
						}
						LiveLog.v(TAG, "start playing "
								+ anchorRoomInfo.getStreamUrl().get(0));
						PULL_URL = anchorRoomInfo.getStreamUrl().get(0);
						mRoomName = anchorRoomInfo.getUserAccount();
						enterRoom();
					} else {
						doAction(LiveBizType.ROOM_LIVE_START);
					}
					break;
				case ROOM_LIVE_START:
					anchorRoomInfo = mParam.getAnchorRoomInfo();
					if (LiveParam.getInstance().getLoginAccount()
							.equals(anchorRoomInfo.getUserAccount())) {
						if (anchorRoomInfo.getStreamUrl() == null
								|| anchorRoomInfo.getStreamUrl().isEmpty()) {
							XCToast.debugShowToast("推流地址为空");
							return;
						}
						LiveLog.v(TAG, "start publishing "
								+ anchorRoomInfo.getStreamUrl().get(0));
						PUSH_URL = anchorRoomInfo.getStreamUrl().get(0);
						mRoomName = anchorRoomInfo.getUserAccount();
						enterRoom();
						// doAction(LiveBizType.ROOM_LIVE_STOP);
					}
					break;
				case ROOM_LIVE_STOP:
					break;
				case ROOM_LEAVE:
					doAction(LiveBizType.USER_LOGOUT);
					break;
				case USER_LOGOUT:
					PAIMManager.getInstance().logout(new PAIMCallBack() {
						
						@Override
						public void onSuccess() {
							
						}
						
						@Override
						public void onError(int errorcode, String errorString) {
							
						}
					});
					isLogin = false;
					mLogout.setEnabled(true);
					mInformationTv.setText("未登录");
					XCToast.showToast("登出成功");
					break;
				default:
					break;
				}
			}

			@Override
			public void onLiveError(LiveBizType type, String desc) {
				LiveLog.e(TAG, "Error type " + type);
				Toast.makeText(StartLiveActivity.this, desc, Toast.LENGTH_SHORT)
						.show();
				switch (type) {
				case USER_LOGIN:
					mLogin.setEnabled(true);
					break;
				case USER_LOGOUT:
					mLogout.setEnabled(true);
					break;

				default:
					break;
				}
			}
		});
	}

	private LiveListDialog coverDialog;

	private void showCoverDialog(List<LiveQueryRoomInfo> list, String title) {
		if (list == null || list.size() == 0) {
			XCToast.showToast("查询直播间列表为空");
			return;
		}

		if (coverDialog == null) {
			coverDialog = new LiveListDialog(this, R.style.Live_list);
		}

		coverDialog.setOnCommentDialogListener(new OnCommentDialogListener() {

			@Override
			public void onItemClick(LiveQueryRoomInfo info) {
				// 进入直播间
				mInformationTv.setText("查询房间信息...");
				if (info != null) {
					mRoomID = info.getClassroomId();
					mRoomName = info.getUserAccount();

					mCurrentRoomInfo = info;
					LiveParam.getInstance().setAnchorRoomId(
							info.getClassroomId());
					// doAction(LiveBizType.ROOM_ENTRY);

					enterRoom();
				}
				hideCoverDialog();
			}

			@Override
			public void onCreateLive() {
				// 创建直播间
				// 房主为主播自己
				// if (mParam.getRoleType() == LiveRoleType.ANCHOR) {
				// mParam.setAnchorAccount(mParam.getLoginAccount());
				// mParam.setMasterRoomSubject(ANCHOR_ROOM_SUBJECT);
				// mParam.setMasterRoomOpen(1);
				// mParam.setMasterRoomExpireTime(60 * 24 * 60 * 60);
				// doAction(LiveBizType.ROOM_CREATE);
				// }
			}
		});
		coverDialog.setData(list);
		coverDialog.setListTitle(title);
		if (!coverDialog.isShowing()) {
			coverDialog.show();
		}

	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		if (event.getAction() == MotionEvent.ACTION_POINTER_3_DOWN) {
			mDebugMode.setVisibility(View.VISIBLE);
			tokenCheckbox.setVisibility(View.VISIBLE);
		}
		return super.onTouchEvent(event);
	}

	private void hideCoverDialog() {
		if (coverDialog != null && coverDialog.isShowing()) {
			coverDialog.dismiss();
			coverDialog = null;
		}
	}
	int time = 0;
	String domain = "pili-live-rtmp.ps.qiniucdn.com";
	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		
		DNSPaser.getIpAddress("192.168.1.1", domain);
		
		Timer  timer=new Timer(); 
		
		TimerTask myTask=new TimerTask() {
			
			@Override
			public void run() {
				// TODO Auto-generated method stub
				MethodCostUtils.init();
				time++;
				String[] ipAddressFromCache = DNSPaser.getIpAddressFromCache("192.168.1.1", domain);
				if (ipAddressFromCache!=null) {
					String ret= "";
					for (int i = 0; i < ipAddressFromCache.length; i++) {
						ret  = ret  + "\n"+ipAddressFromCache[i];
					}
					ret +="\n";
					Log.e("MethodCostUtil_function"," DNS "+ ret);
				}
				
				MediaRecorderUtils.test(domain);
//				MediaRecorderUtils.test("157.255.158.70");
//				MediaRecorderUtils.test("pili-live-rtmp.ps.qiniucdn.com");
//				MediaRecorderUtils.test("www.baidu.com");
				MethodCostUtils.mark("===   "+time +"   ");
			}
		}; 
		if (time == 0) {
			  timer.schedule(myTask, 5000, 10000);
		}
		
      
		
		
		
		
	}

	@Override
	protected void onDestroy() {
		PAIMManager.getInstance().logout(new PAIMCallBack() {
			
			@Override
			public void onSuccess() {
				
			}
			
			@Override
			public void onError(int errorcode, String errorString) {
				
			}
		});
		super.onDestroy();
	}
}
