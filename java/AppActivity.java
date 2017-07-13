/* 
 * jesssoft 
 */
package org.cocos2dx.cpp;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

/*
 * Util
 */
import android.net.Uri;

/*
 * Accounts
 */
import android.accounts.Account;
import android.accounts.AccountManager;

/*
 * Vibrating
 */
import android.os.Vibrator;

/*
 * Admob
 */
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.InterstitialAd;
import android.widget.RelativeLayout;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.view.ViewGroup;

/* 
 * In-app billing 
 */
import org.cocos2dx.cpp.util.IabHelper;
import org.cocos2dx.cpp.util.IabResult;
import org.cocos2dx.cpp.util.Inventory;
import org.cocos2dx.cpp.util.Purchase;
import org.cocos2dx.cpp.util.SkuDetails;
import android.util.Base64;
import java.util.ArrayList;
import java.util.HashMap;

public class AppActivity extends Cocos2dxActivity {
	public static AppActivity mMainActivity = null;
	public static Vibrator mVibrator = null;
	static final String TAG = "#### gamemadang ####";
	Context mContext = null;

	/*
	 * Admob
	 */
	public AdView mAdView = null;
	public InterstitialAd mInterstitial = null;
	private static final String ADMOB_ID_BANNER =
	    "ca-app-pub-id1";
	private static final String ADMOB_ID_INTERSTITIAL = 
	    "ca-app-pub-id2";

	/*
	 * In-app billing
	 */
	static final int RC_REQUEST = 10001;
	public IabHelper mIabHelper = null;
	public String mRequestPayload = null;
	private String mPublicKey = "public-key";
	public static final String[] SKU_ITEM_LIST = {
		"mole_cash_id_1100_11",
		"mole_cash_id_2200_23",
		"mole_cash_id_5500_60",
		"mole_cash_id_11000_132",
		"mole_cash_id_33000_430",
		"mole_cash_id_55000_770"
	};
	public static final HashMap<String, String> SKU_ITEM_PRICE_MAP =
	    new HashMap<String, String>();

	/* 
	 * jesssoft 
	 * ScrollView needs stencil buffer
	 */
	@Override
	public Cocos2dxGLSurfaceView onCreateView() {
		Cocos2dxGLSurfaceView glSurfaceView =
		    new Cocos2dxGLSurfaceView(this);
		/*
		 * ScrollView should create stencil buffer
		 * (redSize, greenSize, blueSize, alphaSize, depthSize,
		 * stencilSize)
		 */
		glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);
		return glSurfaceView;
	}

	@Override
	protected void onCreate(final Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		mMainActivity = this;
		mContext = getApplicationContext();
		mVibrator = (Vibrator)getSystemService(
		    Context.VIBRATOR_SERVICE);

		/*
		 * Notify
		 */
		onMsgFromJava("create", "");

		/*
		 * Admob
		 */
		initAdmob();

		/*
		 * In-app billing
		 */
		initInappBilling();

		/*
		 * Accounts list
		 */
		AccountManager accountManager = AccountManager.get(this);
		Account[] accounts = accountManager.getAccounts();
		for (int i = 0; i < accounts.length; i++) {
			String info = accounts[i].name;

			info += "(";
			info += accounts[i].type;
			info += ")";
			/*
			Log.d(TAG, "Account: " + info);
			*/
		
			onMsgFromJava("account", accounts[i].name);
		}

		/*
		 * Notify
		 */
		onMsgFromJava("init", "");
	}

	private static native void onMsgFromJava(String key, String val);

	public static void cleanupActivity() {
		/*
		 * Cleanup Admob
		 */
		if (mMainActivity.mAdView != null) {
			mMainActivity.runOnUiThread(new Runnable() {
				@Override
				public void run() {
					mMainActivity.mAdView.destroy();
					mMainActivity.mAdView = null;
				}
			});
		}

		/*
		 * Cleanup Inapp
		 */
		if (mMainActivity.mIabHelper != null) {
			mMainActivity.runOnUiThread(new Runnable() {
				@Override
				public void run() {
					mMainActivity.mIabHelper.dispose();
					mMainActivity.mIabHelper = null;
				}
			});
		}
		
		/*
		 * Waiting for complete
		 */
		while (true) {
			if (mMainActivity.mAdView != null ||
			    mMainActivity.mIabHelper != null) {
				Log.d(TAG, "thread wait...");
				try {
					Thread.sleep(100);
				} catch (InterruptedException ie) {
					/* nothing to do? */
				}
				continue;
			}
			break;
		}

		/*
		 * It's ok to quit now
		 */
		Log.d(TAG, "cleanupActivity() called");
	}

	/*
	 * Util
	 */
	public static void openWeb(String url, boolean finishFlag) {
		Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
		intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK |
		    Intent.FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET);
		mMainActivity.startActivity(intent);

		if (finishFlag)
			mMainActivity.finish();
	}

	public static void openMyAppWeb() {
		String packageName = mMainActivity.getApplicationContext().
		    getPackageName();
		openWeb("market://details?id=" + packageName, false);
	}

	/*
	 * Vibrator
	 */
	public static void vibrate(int ms) {
		if (ms <= 0)
			ms = 1;
		if (ms > 10000)
			ms = 10000;
		mVibrator.vibrate(ms);
	}

	/*
	 * Admob
	 */
	private void initAdmob() {
		initAdmobBanner();
		initAdmobInterstitial();
	}

	private void initAdmobBanner() {
		mAdView = new AdView(this);
		mAdView.setAdSize(AdSize.BANNER);
		mAdView.setAdUnitId(ADMOB_ID_BANNER);
		mAdView.setAdListener(new ToastAdListener(this) {
			@Override
			public void onAdLoaded() {
				super.onAdLoaded();
				onMsgFromJava("admob-banner", "loaded");
			}

			@Override
			public void onAdFailedToLoad(int errorCode) {
				super.onAdFailedToLoad(errorCode);
				onMsgFromJava("admob-banner", "failed");
			}
		});

		RelativeLayout.LayoutParams params =
		    new RelativeLayout.LayoutParams(
			RelativeLayout.LayoutParams.MATCH_PARENT,
			RelativeLayout.LayoutParams.WRAP_CONTENT);
		/*
		 * Uncomment next comment if you want to display ad on the
		 * bottom 
		 */
		/*
		params.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
		*/
		RelativeLayout layout = new RelativeLayout(this);
		layout.addView(mAdView, params);

		ViewGroup.LayoutParams vparams =
		    new ViewGroup.LayoutParams(
			ViewGroup.LayoutParams.MATCH_PARENT,
			ViewGroup.LayoutParams.MATCH_PARENT);

		/*
		mAdView.setVisibility(View.VISIBLE);
		mAdView.setVisibility(View.INVISIBLE);
		mAdView.setVisibility(View.GONE);
		*/

		AdRequest adRequest = new AdRequest.Builder()
			/* Test mode - start */
			/* In the release version, comment this out! */
			/*
			.addTestDevice(AdRequest.DEVICE_ID_EMULATOR)
			.addTestDevice("device-id")
			*/
			/* Test mode - end */
			.build();
		mAdView.loadAd(adRequest);
		addContentView(layout, vparams);
		mAdView.setVisibility(View.INVISIBLE);
		layout.bringToFront();
	}

	private void initAdmobInterstitial() {
		mInterstitial = new InterstitialAd(this);
		mInterstitial.setAdUnitId(ADMOB_ID_INTERSTITIAL);
		mInterstitial.setAdListener(new ToastAdListener(this) {
			@Override
			public void onAdLoaded() {
				super.onAdLoaded();
				/*
				showInterstitial();
				*/
				onMsgFromJava("admob-interstitial", "loaded");
			}

			@Override
			public void onAdFailedToLoad(int errorCode) {
				super.onAdFailedToLoad(errorCode);
				onMsgFromJava("admob-interstitial", "failed");
			}
		});
	}

	public static void showAdmobBanner() {
		mMainActivity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (mMainActivity.mAdView == null)
					return;
				if (mMainActivity.mAdView.getVisibility() ==
				    View.VISIBLE)
					return;
				mMainActivity.mAdView.setVisibility(
				    View.VISIBLE);
				mMainActivity.mAdView.bringToFront();

				/*
				 * Animation
				 */
				/* Down -> Up animation
				TranslateAnimation anim = 
				    new TranslateAnimation(
					Animation.RELATIVE_TO_SELF, 0.0f,
					Animation.RELATIVE_TO_SELF, 0.0f,
					Animation.RELATIVE_TO_SELF, 1.0f,
					Animation.RELATIVE_TO_SELF, 0.0f);
					*/
				/* Up -> Down animation */
				TranslateAnimation anim = 
				    new TranslateAnimation(
					Animation.RELATIVE_TO_SELF, 0.0f,
					Animation.RELATIVE_TO_SELF, 0.0f,
					Animation.RELATIVE_TO_SELF, -1.0f,
					Animation.RELATIVE_TO_SELF, 0.0f);
				anim.setFillEnabled(true);
				anim.setDuration(1000);

				mMainActivity.mAdView.setAnimation(anim);
				mMainActivity.mAdView.startAnimation(anim);
			}
		});
	}

	public static void hideAdmobBanner() {
		mMainActivity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (mMainActivity.mAdView == null)
					return;
				if (mMainActivity.mAdView.getVisibility() ==
				    View.INVISIBLE)
					return;
				mMainActivity.mAdView.setVisibility(
				    View.INVISIBLE);
				mMainActivity.mAdView.bringToFront();
			}
		});
	}
	public static void loadAdmobInterstitial() {
		mMainActivity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (mMainActivity.mInterstitial == null)
					return;
				mMainActivity.mInterstitial.loadAd(
				    new AdRequest.Builder().build());
			}
		});
	}

	public static void showAdmobInterstitial() {
		mMainActivity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (mMainActivity.mInterstitial == null)
					return;
				if (mMainActivity.mInterstitial.isLoaded())
					mMainActivity.mInterstitial.show();
			}
		});
	}

	/*
	 * In-app billing
	 */
	private void initInappBilling() {
		mIabHelper = new IabHelper(this, mPublicKey);

		/* debug mode */
		mIabHelper.enableDebugLogging(true); 
		mIabHelper.startSetup(
		    new IabHelper.OnIabSetupFinishedListener() {
		    	public void onIabSetupFinished(IabResult result) {
			/*
				Log.d(TAG, "iab: setup finished.");
				*/
				if (!result.isSuccess()) {
				/*
					Log.d(TAG, "iab: setup failed.");
					*/
					return;
				}

				if (mIabHelper == null)
					return;

				ArrayList<String> skuList = 
				    new ArrayList<String>();
				for (String sku : SKU_ITEM_LIST) {
					skuList.add(sku);
					/*
					Log.d(TAG,
					    "iab: " + sku + " added.");
					    */
				}

				/*
				Log.d(TAG, "iab: queryInventoryAsync() called");
				*/
				mIabHelper.queryInventoryAsync(true,
				    skuList, gotInventoryListener);
			}
		    }
		);
	}

	IabHelper.QueryInventoryFinishedListener gotInventoryListener =
	    new IabHelper.QueryInventoryFinishedListener() {
		public void onQueryInventoryFinished(IabResult result,
		    Inventory inventory) {
			/*
			Log.d(TAG, "iab: onQueryInventoryFinished() called");
			*/
			if (mIabHelper == null)
				return;

			if (result.isFailure()) {
				/*
				Log.d(TAG, "iab: inventory failed");
				*/
				return;
			}

			/*
			Log.d(TAG, "##############################");
			*/
			for (String sku : SKU_ITEM_LIST) {
				SkuDetails skuDetail = inventory.getSkuDetails(
				    sku);
				if (skuDetail == null) {
					/*
					Log.d(TAG,
					    "iab: skuDetail(" + sku +
					    ") not found");
					    */
					continue;
				}

				/*
				Log.d(TAG, "iab: price = " +
				    skuDetail.getPrice());
				    */

				SKU_ITEM_PRICE_MAP.put(sku,
				    skuDetail.getPrice());

				onMsgFromJava(sku, skuDetail.getPrice());
			}
			onMsgFromJava("iab_price_complete", "");

			Purchase p;
			for (String sku : SKU_ITEM_LIST) {
				p = inventory.getPurchase(sku);
				if (p != null && verifyDeveloperPayload(p))
					mIabHelper.consumeAsync(p,
					    consumeFinishedListener);
			}
		}
	};

	IabHelper.OnConsumeFinishedListener consumeFinishedListener =
	    new IabHelper.OnConsumeFinishedListener() {
		public void onConsumeFinished(Purchase purchase,
		    IabResult  result) {
			if (mIabHelper == null)
				return;

			if (!result.isSuccess()) {
				/*
				 * purchase fail callback
				 */
				callback(-1, purchase.getDeveloperPayload(),
				    "null", purchase.getOriginalJson(),
				    purchase.getSignature());
				return;
			}

			/*
			 * purchase success callback
			 */
			callback(0, purchase.getDeveloperPayload(),
			    purchase.getOrderId(), purchase.getOriginalJson(),
			    purchase.getSignature());
		}
	};

	IabHelper.OnIabPurchaseFinishedListener purchaseFinishedListener =
	    new IabHelper.OnIabPurchaseFinishedListener() {
		public void onIabPurchaseFinished(IabResult result,
		    Purchase purchase) {
			/*
			Log.d(TAG, "iab: onIabPurchaseFinished() 1");
			*/
			if (mIabHelper == null)
				return;

			/*
			Log.d(TAG, "iab: onIabPurchaseFinished() 2");
			*/
			if (result.isFailure()) {
				callback(-1, "null", "null", "null", "null");
				return;
			}

			/*
			Log.d(TAG, "iab: onIabPurchaseFinished() 3");
			*/
			if (!verifyDeveloperPayload(purchase)) {
				callback(-1, purchase.getDeveloperPayload(),
				    "null", purchase.getOriginalJson(),
				    purchase.getSignature());
				return;
			}

			/*
			 * Purchase successful.
			 */
			/*
			Log.d(TAG, "iab: onIabPurchaseFinished() 4");
			*/

			/* Save time - I'm not sure this way is right */
			/*
			callback(1, purchase.getDeveloperPayload(),
			    purchase.getOrderId(), purchase.getOriginalJson(),
			    purchase.getSignature());
			    */

			/* Consume time */
			if (isSkuItem(purchase.getSku())) {
				mIabHelper.consumeAsync(purchase,
				    consumeFinishedListener);
			} else {
				callback(-1, purchase.getDeveloperPayload(),
				    "null", "null", "null");
			}
		}
	};

	boolean verifyDeveloperPayload(Purchase p) {
		/*
		String payload = p.getDeveloperPayload();
		*/
		return true;
	}

	void callback(int code, String payload, String orderId,
	    String purchaseData, String signature) {
		String arg;

		String purchaseDataBase64 = Base64.encodeToString(
		    purchaseData.getBytes(), 
		    /* Base64.DEFAULT */Base64.NO_WRAP);

		/*
		 * First one character means the delimiter.
		 */
		arg = "!" + code + "!" + payload + "!" + orderId + "!" +
		    purchaseDataBase64 + "!" + signature;

		/*
		Log.d(TAG, "iab-final: " + arg);
		*/
		onMsgFromJava("iab", arg);
	}

	private static boolean isSkuItem(String itemName) {
		for (String sku : SKU_ITEM_LIST)
			if (itemName.equals(sku))
				return true;
		
		return false;
	}

	private void purchaseCash_() {
		String sku = mRequestPayload;

		if (!isSkuItem(sku))
			return;

		mIabHelper.launchPurchaseFlow(mMainActivity,
		    sku, RC_REQUEST, purchaseFinishedListener, mRequestPayload);
		mRequestPayload = null;
	}

	public static void purchaseCash(String payload) {
		if (mMainActivity.mRequestPayload != null)
			return;

		mMainActivity.mRequestPayload = payload;
		mMainActivity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				mMainActivity.purchaseCash_();
			}
		});
	}

	public static String getSkuPrice(String sku) {
		String val;

		val = SKU_ITEM_PRICE_MAP.get(sku);
		if (val == null)
			return "";
		return val;
	}

	@Override
	public void onActivityResult(int requestCode, int resultCode,
	    Intent data) {
		if (mIabHelper == null) {
			super.onActivityResult(requestCode, resultCode, data);
		} else if (!mIabHelper.handleActivityResult(requestCode,
		    resultCode, data)) {
			super.onActivityResult(requestCode, resultCode, data);
		}
	}

	@Override
	public void onResume() {
		super.onResume();
		if (mAdView != null)
			mAdView.resume();
	}

	@Override
	public void onPause() {
		if (mAdView != null)
			mAdView.pause();
		super.onPause();
	}

	/*
	 * Cocos2d-x just kills this process so that this method is never
	 * called
	 */
	@Override
	public void onDestroy() {
		Log.d(TAG,
		    "**** Cocos2d-x has changed its own end() function! ****");
		cleanupActivity();
		super.onDestroy();
	}
}
