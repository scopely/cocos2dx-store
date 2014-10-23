//
// Created by Fedor Shubin on 5/19/13.
//


#include "CCMarketItem.h"

USING_NS_CC;

namespace soomla {

    CCMarketItem *CCMarketItem::create(__String *productId, __Integer *consumable, __Double *price) {
        CCMarketItem *ret = new CCMarketItem();
        ret->autorelease();
        ret->init(productId, consumable, price);
        return ret;
    }

    CCMarketItem *CCMarketItem::createWithDictionary(__Dictionary *dict) {
        CCMarketItem *ret = new CCMarketItem();
        ret->autorelease();
        ret->initWithDictionary(dict);
        return ret;
    }

    bool CCMarketItem::init(__String *productId, __Integer *consumable, __Double *price) {
        setProductId(productId);
        setConsumable(consumable);
        setPrice(price);

        return true;
    }

    bool CCMarketItem::initWithDictionary(__Dictionary *dict) {

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        char const* key = JSON_MARKETITEM_ANDROID_ID;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        char const* key = JSON_MARKETITEM_IOS_ID;
#endif
        cocos2d::Ref* obj = dict->objectForKey(key);
        CCAssert(obj == NULL || dynamic_cast<__String *>(obj), "invalid object type in dictionary");
        setProductId((__String *)obj);

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

    __Dictionary *CCMarketItem::toDictionary() {
        __Dictionary *dict = __Dictionary::create();

        putProductIdToDict(dict);
        putConsumableToDict(dict);
        putPriceToDict(dict);

        return dict;
    }
};

