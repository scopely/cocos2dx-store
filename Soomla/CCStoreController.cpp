//
// Created by Fedor Shubin on 5/20/13.
//

#include "CCStoreController.h"
#include "CCSoomla.h"
#include "CCStoreUtils.h"
#include "CCSoomlaNdkBridge.h"
#include "data/CCStoreInfo.h"

namespace soomla {
    #define TAG "SOOMLA StoreController"

    USING_NS_CC;

    static CCStoreController *s_SharedStoreController = NULL;

    CCStoreController *CCStoreController::sharedStoreController() {
        if (!s_SharedStoreController)
        {
            s_SharedStoreController = new CCStoreController();
            s_SharedStoreController->retain();
        }
        return s_SharedStoreController;
    }

    void CCStoreController::initShared(CCIStoreAssets *storeAssets, __Dictionary *storeParams) {
        CCStoreController *storeController = CCStoreController::sharedStoreController();
        if (!storeController->init(storeAssets, storeParams)) {
            exit(1);
        }
    }

    CCStoreController::CCStoreController() {
        this->mSoomSec = NULL;
    }

    CCStoreController::~CCStoreController() {

    }

    bool CCStoreController::init(CCIStoreAssets *storeAssets, __Dictionary *storeParams) {
        __String *customSecret = dynamic_cast<__String *>(storeParams->objectForKey("customSecret"));
        __String *soomSec = mSoomSec;
        if (soomSec == NULL) {
            soomSec = dynamic_cast<__String *>(storeParams->objectForKey("soomSec"));
        }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        __String *androidPublicKey = dynamic_cast<__String *>(storeParams->objectForKey("androidPublicKey"));
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        __Bool *SSV = dynamic_cast<__Bool *>(storeParams->objectForKey("SSV"));
#endif

        if (customSecret == NULL) customSecret = __String::create("");
        if (soomSec      == NULL) soomSec      = __String::create("");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        if (androidPublicKey == NULL) androidPublicKey = __String::create("");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        if (SSV == NULL) SSV = __Bool::create(false);
#endif

        // Redundancy checking. Most JS libraries don't do this. I hate it when they don't do this. Do this.
        DictElement* el = NULL;
        CCDICT_FOREACH(storeParams, el) {
            std::string key = el->getStrKey();
            if (!(
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
                  key.compare("androidPublicKey") == 0 ||
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                  key.compare("SSV") == 0 ||
#endif
                  key.compare("customSecret") == 0 ||
                  key.compare("soomSec")      == 0 )) {
                __String *message =
                    __String::createWithFormat("WARNING!! Possible typo in member of storeParams: %s", key.c_str());
                CCStoreUtils::logError(TAG, message->getCString());
            }
        }


        if (customSecret->length() == 0 || soomSec->length() == 0) {
            CCStoreUtils::logError(TAG, "SOOMLA/COCOS2DX MISSING customSecret or soomSec !!! Stopping here !!");
            return false;
        }

        if (customSecret->compare(SOOMLA_ONLY_ONCE_DEFAULT) == 0 ||
            soomSec->compare(SOOMLA_ONLY_ONCE_DEFAULT) == 0) {

            CCStoreUtils::logError(TAG, "SOOMLA/COCOS2DX You have to change customSecret and soomSec !!! Stopping here !!");
            return false;
        }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        if (androidPublicKey->length() == 0) {
            CCStoreUtils::logError(TAG, "SOOMLA/COCOS2DX MISSING publickKey !!! Stopping here !!");
            return false;
        }

        if (androidPublicKey->compare(SOOMLA_AND_PUB_KEY_DEFAULT) == 0) {
            CCStoreUtils::logError(TAG, "SOOMLA/COCOS2DX You have to change android publicKey !!! Stopping here !!");
            return false;
        }

        {
            __Dictionary *params = __Dictionary::create();
            params->setObject(__String::create("CCStoreController::setAndroidPublicKey"), "method");
            params->setObject(androidPublicKey, "androidPublicKey");
            CCSoomlaNdkBridge::callNative(params, NULL);
        }
#endif

        {
            __Dictionary *params = __Dictionary::create();
            params->setObject(__String::create("CCStoreController::setSoomSec"), "method");
            params->setObject(soomSec, "soomSec");
            CCSoomlaNdkBridge::callNative(params, NULL);
        }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        {
            __Dictionary *params = __Dictionary::create();
            params->setObject(__String::create("CCStoreController::setSSV"), "method");
            params->setObject(SSV, "ssv");
            CCSoomlaNdkBridge::callNative(params, NULL);
        }
#endif

        CCStoreInfo::createShared(storeAssets);

        {
            __Dictionary *params = __Dictionary::create();
            params->setObject(__String::create("CCStoreController::init"), "method");
            params->setObject(customSecret, "customSecret");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            params->setObject(androidPublicKey, "androidPublicKey");
#endif
            CCSoomlaError *soomlaError = NULL;
            CCSoomlaNdkBridge::callNative(params, &soomlaError);

            if (soomlaError) {
                return false;
            }
        }

        return true;
    }

    void CCStoreController::setupSoomSec() {
        CC_ASSERT(mSoomSec);
        {
            __Dictionary *params = __Dictionary::create();
            params->setObject(__String::create("CCStoreController::setSoomSec"), "method");
            params->setObject(mSoomSec, "soomSec");
            CCSoomlaNdkBridge::callNative(params, NULL);
        }
    }

    void CCStoreController::buyMarketItem(const char *productId, const char *payload, CCSoomlaError **soomlaError) {
        __Dictionary *params = __Dictionary::create();
        params->setObject(__String::create("CCStoreController::buyMarketItem"), "method");
        params->setObject(__String::create(productId), "productId");
        // NOTE: payload is not supported on iOS !
        params->setObject(__String::create(payload), "payload");
        CCSoomlaNdkBridge::callNative(params, soomlaError);
    }

    void CCStoreController::restoreTransactions() {
        __Dictionary *params = __Dictionary::create();
        params->setObject(__String::create("CCStoreController::restoreTransactions"), "method");
        CCSoomlaNdkBridge::callNative(params, NULL);
    }

    void CCStoreController::refreshInventory() {
        __Dictionary *params = __Dictionary::create();
        params->setObject(__String::create("CCStoreController::refreshInventory"), "method");
        CCSoomlaNdkBridge::callNative(params, NULL);
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    bool CCStoreController::transactionsAlreadyRestored() {
        __Dictionary *params = __Dictionary::create();
        params->setObject(__String::create("CCStoreController::transactionsAlreadyRestored"), "method");
        __Dictionary *retParams = (__Dictionary *) CCSoomlaNdkBridge::callNative(params, NULL);
        __Bool *retValue = (__Bool *) retParams->objectForKey("return");
        return retValue->getValue();
    }

    void CCStoreController::refreshMarketItemsDetails(CCSoomlaError **soomlaError) {
        __Dictionary *params = __Dictionary::create();
        params->setObject(__String::create("CCStoreController::refreshMarketItemsDetails"), "method");
        CCSoomlaNdkBridge::callNative(params, soomlaError);
    }
    
    void CCStoreController::setCustomReceiptVerificationClassName(__String *className)
    {
        __Dictionary *params = __Dictionary::create();
        params->setObject(__String::create("CCStoreController::setCustomReceiptVerificationClassName"), "method");
        params->setObject(className, "customReceiptVerificationClassName");
        CCSoomlaNdkBridge::callNative(params, NULL);
    }
    
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    void CCStoreController::startIabServiceInBg() {
        __Dictionary *params = __Dictionary::create();
        params->setObject(__String::create("CCStoreController::startIabServiceInBg"), "method");
        CCSoomlaNdkBridge::callNative(params, NULL);
    }

    void CCStoreController::stopIabServiceInBg() {
        __Dictionary *params = __Dictionary::create();
        params->setObject(__String::create("CCStoreController::stopIabServiceInBg"), "method");
        CCSoomlaNdkBridge::callNative(params, NULL);
    }

    void CCStoreController::setAndroidReceiptVerificationClassName(__String *androidReceiptVerificationClass) {
    	__Dictionary *params = __Dictionary::create();
    	params->setObject(__String::create("CCStoreController::setAndroidReceiptVerificationClassName"), "method");
    	params->setObject(androidReceiptVerificationClass, "androidReceiptVerificationClass");
    	CCSoomlaNdkBridge::callNative(params, NULL);
    }
#endif
}
