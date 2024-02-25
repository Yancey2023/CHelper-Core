//
// Created by Yancey on 2024/2/24.
//

#ifndef CHELPER_ANDROIDNATIVE_H
#define CHELPER_ANDROIDNATIVE_H
#if CHelperAndroid == true

#include "pch.h"
#include "../chelper/Core.h"

std::string jstring2string(JNIEnv *env, jstring jStr) {
    const char *cstr = env->GetStringUTFChars(jStr, nullptr);
    std::string str = std::string(cstr);
    env->ReleaseStringUTFChars(jStr, cstr);
    return str;
}

std::shared_ptr<CHelper::Core> core;

extern "C"
JNIEXPORT jboolean JNICALL
Java_yancey_chelper_core_CHelperCore_init(JNIEnv *env, jobject thiz,
                                          jstring cpack_path) {
    try {
        clock_t start, end;
        start = clock();
        core = std::make_shared<CHelper::Core>(CHelper::Core::create(jstring2string(env, cpack_path)));
        end = clock();
        CHELPER_INFO("CPack load successfully (" + std::to_string(end - start) + "ms)");
        return true;
    } catch (const std::exception &e) {
        CHELPER_ERROR("parse failed");
        CHelper::Exception::printStackTrace(e);
        CHelper::Profile::clear();
        return false;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_onTextChanged(JNIEnv *env,
                                                   jobject thiz,
                                                   jstring cpack_path,
                                                   jint index) {
    if (core == nullptr) {
        return;
    }
    core->onTextChanged(jstring2string(env, cpack_path), index);
}

extern "C"
JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_onSelectionChanged(JNIEnv *env,
                                                        jobject thiz,
                                                        jint index) {
    if (core == nullptr) {
        return;
    }
    core->onSelectionChanged(index);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getDescription(JNIEnv *env,
                                                    jobject thiz) {
    if (core == nullptr) {
        return nullptr;
    }
    std::string description = core->getDescription();
    return env->NewStringUTF(description.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getErrorReasons(JNIEnv *env,
                                                     jobject thiz) {
    if (core == nullptr) {
        return nullptr;
    }
    auto errorReasons = core->getErrorReasons();
    if (errorReasons.empty()) {
        return nullptr;
    } else if (errorReasons.size() == 1) {
        std::string result = errorReasons[0]->errorReason;
        return env->NewStringUTF(result.c_str());
    } else {
        std::string result = "可能的错误原因：";
        int i = 0;
        for (const auto &item: errorReasons) {
            result.append("\n").append(std::to_string(i++)).append(". ").append(item->errorReason);
        }
        return env->NewStringUTF(result.c_str());
    }
}

extern "C"
JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestions(JNIEnv *env,
                                                    jobject thiz) {
    if (core == nullptr) {
        return nullptr;
    }
    auto suggestions = core->getSuggestions();
    std::string result;
    bool isFirst = true;
    for (const auto &item: suggestions) {
        if (isFirst) {
            isFirst = false;
        } else {
            result.append("\n");
        }
        result.append(item.content->name).append("\n").append(item.content->description.value_or(""));
    }
    return env->NewStringUTF(result.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getStructure(JNIEnv *env,
                                                  jobject thiz) {
    if (core == nullptr) {
        return nullptr;
    }
    std::string structure = core->getStructure();
    return env->NewStringUTF(structure.c_str());
}

#endif
#endif //CHELPER_ANDROIDNATIVE_H
