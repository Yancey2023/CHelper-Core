//
// Created by Yancey on 2024/2/24.
//

#ifndef CHELPER_ANDROIDNATIVE_H
#define CHELPER_ANDROIDNATIVE_H

#include "pch.h"

#if CHelperAndroid == true

#include "../chelper/Core.h"

std::string jstring2string(JNIEnv *env, jstring jStr) {
    const char *cstr = env->GetStringUTFChars(jStr, nullptr);
    std::string str = std::string(cstr);
    env->ReleaseStringUTFChars(jStr, cstr);
    return str;
}

std::shared_ptr<CHelper::Core> core;

extern "C" JNIEXPORT jboolean JNICALL
Java_yancey_chelper_core_CHelperCore_init(
        JNIEnv *env, jobject thiz, jstring cpack_path) {
    core = CHelper::Core::create(jstring2string(env, cpack_path));
    return core != nullptr;
}

extern "C" JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_onTextChanged(
        JNIEnv *env, jobject thiz, jstring cpack_path, jint index) {
    if (core == nullptr) {
        return;
    }
    core->onTextChanged(jstring2string(env, cpack_path), index);
}

extern "C" JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_onSelectionChanged(
        JNIEnv *env, jobject thiz, jint index) {
    if (core == nullptr) {
        return;
    }
    core->onSelectionChanged(index);
}

extern "C" JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getDescription(
        JNIEnv *env, jobject thiz) {
    if (core == nullptr) {
        return nullptr;
    }
    std::string description = core->getDescription();
    return env->NewStringUTF(description.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getErrorReasons(
        JNIEnv *env, jobject thiz) {
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

extern "C" JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestions(
        JNIEnv *env, jobject thiz) {
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

extern "C" JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getStructure(
        JNIEnv *env, jobject thiz) {
    if (core == nullptr) {
        return nullptr;
    }
    std::string structure = core->getStructure();
    return env->NewStringUTF(structure.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_onSuggestionClick(
        JNIEnv *env, jobject thiz, jint which) {
    if (core == nullptr) {
        return nullptr;
    }
    std::optional<std::string> result = core->onSuggestionClick(which);
    if (result.has_value()) {
        return env->NewStringUTF(result.value().c_str());
    } else {
        return nullptr;
    }
}

#endif
#endif //CHELPER_ANDROIDNATIVE_H