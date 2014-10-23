//
// Created by Fedor Shubin on 1/22/14.
//

#ifdef COCOS2D_JAVASCRIPT

#include "JSBinding.h"
#include "CCSoomlaError.h"
#include "jansson.h"
#include "CCSoomlaJsonHelper.h"
#include "CCSoomlaNdkBridge.h"

void Soomla::JSBinding::callNative(const char *params, std::string &result) {
    result.assign(params);

    CCLog("callNative: in >> %s", params);

    json_error_t error;
    json_t *root;
    root = json_loads(params, 0, &error);

    if (!root) {
        CCLog("error: at line #%d: %s", error.line, error.text);
        return;
    }

    cocos2d::Ref *dataToPass = CCSoomlaJsonHelper::getRefFromJson(root);
    __Dictionary *dictToPass = dynamic_cast<__Dictionary *>(dataToPass);
    CC_ASSERT(dictToPass);

    soomla::CCSoomlaError *soomlaError = NULL;
    __Dictionary *retParams = (__Dictionary *) soomla::CCSoomlaNdkBridge::callNative(dictToPass, &soomlaError);

    __Dictionary *resultParams = __Dictionary::create();
    if (soomlaError != NULL) {
        retParams = __Dictionary::create();
        retParams->setObject(__Integer::create(soomlaError->getCode()), "code");
        retParams->setObject(__String::create(soomlaError->getInfo()), "info");

        resultParams->setObject(__Bool::create(false), "success");
    } else {
        resultParams->setObject(__Bool::create(true), "success");
    }
    resultParams->setObject(retParams, "result");

    root = CCSoomlaJsonHelper::getJsonFromRef(resultParams);
    char *dump = json_dumps(root, JSON_COMPACT | JSON_ENSURE_ASCII);
    CCLog("callNative: out >> %s", dump);
    result = dump;
    free(dump);
}

void Soomla::JSBinding::callCallback(__Dictionary *params) {
    json_t *root = CCSoomlaJsonHelper::getJsonFromRef(params);
    char *dump = json_dumps(root, JSON_COMPACT | JSON_ENSURE_ASCII);
    CCLog("callCallback: in >> %s", dump);

    std::string jsonParams = dump;
    free(dump);

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();

    jsval retval;
    jsval v[] = {
            v[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, jsonParams.c_str()))
    };
    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(ScriptingCore::getInstance()->getGlobalObject()),
            "easyNDKCallBack", 1, v, &retval);
}

#endif // COCOS2D_JAVASCRIPT