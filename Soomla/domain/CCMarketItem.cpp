//
// Created by Fedor Shubin on 5/19/13.
//


#include "CCMarketItem.h"

USING_NS_CC;

namespace soomla {

    CCMarketItem *CCMarketItem::create(CCString *productId, CCInteger *consumable, CCDouble *price) {
        CCMarketItem *ret = new CCMarketItem();
        ret->autorelease();
        ret->init(productId, consumable, price);
        return ret;
    }

    CCMarketItem *CCMarketItem::createWithDictionary(CCDictionary *dict) {
        CCMarketItem *ret = new CCMarketItem();
        ret->autorelease();
        ret->initWithDictionary(dict);
        return ret;
    }

    bool CCMarketItem::init(CCString *productId, CCInteger *consumable, CCDouble *price) {
        setProductId(productId);
        setConsumable(consumable);
        setPrice(price);

        return true;
    }

    bool CCMarketItem::initWithDictionary(CCDictionary *dict) {

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        char const* key = JSON_MARKETITEM_ANDROID_ID;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        char const* key = JSON_MARKETITEM_IOS_ID;
#endif
        cocos2d::CCObject* obj = dict->objectForKey(key);
        CCAssert(obj == NULL || dynamic_cast<CCString *>(obj), "invalid object type in dictionary");
        setProductId((CCString *)obj);

        if (mProductId == NULL) {
            fillProductIdFromDict(dict);
        }

        fillConsumableFromDict(dict);
        fillPriceFromDict(dict);
        fillMarketPriceFromDict(dict);
        fillMarketTitleFromDict(dict);
        fillMarketDescriptionFromDict(dict);
        fillMarketPriceWithCurrencySymbolFromDict(dict);

        return true;
    }

    CCMarketItem::~CCMarketItem() {
        CC_SAFE_RELEASE(mProductId);
        CC_SAFE_RELEASE(mConsumable);
        CC_SAFE_RELEASE(mPrice);
    }

    CCDictionary *CCMarketItem::toDictionary() {
        CCDictionary *dict = CCDictionary::create();

        putProductIdToDict(dict);
        putConsumableToDict(dict);
        putPriceToDict(dict);

        return dict;
    }
};

