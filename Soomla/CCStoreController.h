//
// Created by Fedor Shubin on 5/20/13.
//

#ifndef __CCStoreController_H_
#define __CCStoreController_H_

#include "cocos2d.h"
#include "CCIStoreAssets.h"
#include "CCSoomlaError.h"

namespace soomla {
	/** \class CCStoreController
		\brief An interface to the native StoreController class.

		An interface to the native StoreController class, use this class to
		access the native StoreController functionality.
	 */
    class CCStoreController: public cocos2d::Ref {
        CC_SYNTHESIZE_RETAIN(cocos2d::__String *, mSoomSec, SoomSec);
    public:
		/**
		   This class is singleton, use this function to access it.
		*/
        static CCStoreController* sharedStoreController();

		/**
		   Initialize StoreController on native side and allow using its
		   functions.
           \param storeAssets An instance of your store's assets class.
           \param storeParams A __Dictionary containing parameters for CCStoreController (These were previously found in CCSoomla).
             This dictionary can contain the following:
             "soomSec": __String - The value of the primary encryption key.
             "customSecret": __String - The value of the secondary encryption key.
             "androidPublicKey": __String - Your Android public key.
             "SSV": __Bool - Whether or not to enable server side verification of purchases.
		 */
        static void initShared(CCIStoreAssets *storeAssets, cocos2d::__Dictionary *storeParams);

        CCStoreController(void);
        virtual ~CCStoreController(void);
        virtual bool init(CCIStoreAssets *storeAssets, cocos2d::__Dictionary *storeParams);

        virtual void setupSoomSec();

		/**
		   Buy an item from the App Store or Google Play.
		   \param productId The Product ID of the item in the App Store or Google Play
		   \param soomlaError A CCSoomlaError for error checking.
		 */
        void buyMarketItem(const char *productId, const char *payload, CCSoomlaError **soomlaError);

		/**
		   Restore this user's previous transactions.
		 */
        void restoreTransactions();

        void refreshInventory();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		/**
		   Find out if the user restored his transactions.
		   \return Whether or not the user restored his transactions.
		*/
        bool transactionsAlreadyRestored();
        void refreshMarketItemsDetails(CCSoomlaError **soomlaError);
        
        void setCustomReceiptVerificationClassName(cocos2d::__String *className);
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		void startIabServiceInBg();
		void stopIabServiceInBg();
		void setAndroidReceiptVerificationClassName(cocos2d::__String *payloadGeneratorClass);
#endif
    };
};

#endif // !__CCStoreController_H_
