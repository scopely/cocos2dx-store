package com.soomla.cocos2dx.store;

import android.opengl.GLSurfaceView;
import com.soomla.store.IStoreAssets;
import com.soomla.store.StoreConfig;
import com.soomla.store.StoreController;
import com.soomla.store.StoreUtils;
import com.soomla.store.data.StoreInfo;
import com.soomla.store.domain.PurchasableVirtualItem;
import com.soomla.store.util.DeveloperPayloadGenerator;

import com.soomla.store.exceptions.VirtualItemNotFoundException;
import com.soomla.store.purchaseTypes.PurchaseWithMarket;

import com.soomla.store.billing.google.GooglePlayIabService;

/**
 * This bridge is used to let cocos2dx functions perform actions on StoreController (through JNI).
 *
 * You can see the documentation of every function in {@link StoreController}
 */
public class StoreControllerBridge {
    private static IStoreAssets mStoreAssets   = null;
    private static String mPublicKey           = "";
    private static EventHandlerBridge mEventHandler = null;
    private static GLSurfaceView mGLView = null;
    
    public static void setGLView(GLSurfaceView glView) {
        mGLView = glView;
    }

    public static GLSurfaceView getGLView() {
        return mGLView;
    }

    public static void initialize(String customSecret) {
        StoreUtils.LogDebug("SOOMLA", "initialize is called from java!");
        StoreConfig.InAppBillingService = new GooglePlayIabService();
        
        initializeEventHandler();
        StoreController.getInstance().initialize(mStoreAssets, mPublicKey, customSecret);
    }

    public static void initializeEventHandler() {
    	StoreUtils.LogDebug("SOOMLA", "initializeEventHandler");
    	if (mEventHandler == null) {
    		mEventHandler = new EventHandlerBridge(mGLView);
    	} else {
    		mEventHandler.setGLThread(mGLView);
    	}
    }

    public static void startIabServiceInBg() {
        StoreUtils.LogDebug("SOOMLA", "startIabServiceInBg is called from java!");
        StoreController.getInstance().startIabServiceInBg();
    }

    public static void stopIabServiceInBg() {
        StoreUtils.LogDebug("SOOMLA", "stopIabServiceInBg is called from java!");
        StoreController.getInstance().stopIabServiceInBg();
    }

    public static void buyWithMarket(String productId) throws VirtualItemNotFoundException {
        StoreUtils.LogDebug("SOOMLA", "buyWithMarket is called from java with productId: " + productId + "!");
        PurchasableVirtualItem pvi = StoreInfo.getPurchasableItem(productId);
        if(pvi.getPurchaseType() instanceof PurchaseWithMarket) {
        	String payload;
        	if (StoreConfig.PAYLOAD_GENERATOR != null) {
        		try {
        			DeveloperPayloadGenerator g = (DeveloperPayloadGenerator)Class.forName(StoreConfig.PAYLOAD_GENERATOR).newInstance();
        			payload = g.generatePayloadForSku(productId);
        			
        			StoreUtils.LogDebug("SOOMLA", "Generated developer payload:" + payload);    
        		} catch (Exception e) {
        			payload = "";
        			StoreUtils.LogDebug("SOOMLA", "Error generating developer payload");            			
        		}
        	} else {
        		payload = "";
        	}
        	
            StoreController.getInstance().buyWithMarket(((PurchaseWithMarket)pvi.getPurchaseType()).getMarketItem(), payload);
        } else {
            throw new VirtualItemNotFoundException("productId", productId);
        }
    }

    public static void restoreTransactions() {
        StoreUtils.LogDebug("SOOMLA", "restoreTransactions is called from java!");
        StoreController.getInstance().refreshInventory(false);
    }

    public static boolean transactionsAlreadyRestored() {
        throw new UnsupportedOperationException("transactionsAlreadyRestored has no use in Android");
    }

    public static void setStoreAssetsBridge(StoreAssetsBridge storeAssetsBridge) {
        StoreControllerBridge.mStoreAssets = storeAssetsBridge;
    }

    public static IStoreAssets getStoreAssetsBridge() {
        return StoreControllerBridge.mStoreAssets;
    }

    public static void setSoomSec(String soomSec) {
        StoreUtils.LogDebug("SOOMLA", "setSoomSec is called from java!");
        StoreConfig.SOOM_SEC = soomSec;
    }
    
    public static void setPayloadGeneratorClass(String payloadGenerator) {
        StoreUtils.LogDebug("SOOMLA", "setPayloadGeneratorClass is called from java!");
        StoreConfig.PAYLOAD_GENERATOR = payloadGenerator;
    }

    public static void setAndroidPublicKey(String publicKey) {
        StoreUtils.LogDebug("SOOMLA", "setAndroidPublicKey is called from java!");
        mPublicKey = publicKey;
    }

    public static void refreshInventory() {
        StoreUtils.LogDebug("SOOMLA", "refreshInventory is called from java!");
        StoreController.getInstance().refreshInventory(true);
    }

    private static String TAG = "StoreControllerBridge";
}
