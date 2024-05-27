//
// Created by Yancey on 2024/2/24.
//

#pragma once

#ifndef CHELPER_ANDROIDNATIVE_H
#define CHELPER_ANDROIDNATIVE_H

#define CHelperAndroid true

#include "../chelper/Core.h"
#include "pch.h"

std::string jstring2string(JNIEnv *env, jstring jStr) {
    const char *cstr = env->GetStringUTFChars(jStr, nullptr);
    std::string str = std::string(cstr);
    env->ReleaseStringUTFChars(jStr, cstr);
    return std::move(str);
}

std::shared_ptr<CHelper::Core> core;

extern "C" [[maybe_unused]] JNIEXPORT jboolean JNICALL
Java_yancey_chelper_core_CHelperCore_init(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jobject assetManager, jstring cpack_path) {
    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
    std::string cpackPath = jstring2string(env, cpack_path);
    AAsset *asset = AAssetManager_open(mgr, cpackPath.c_str(), AASSET_MODE_BUFFER);
    if (HEDLEY_UNLIKELY(asset == nullptr)) {
        return false;
    }
    size_t dataFileSize = AAsset_getLength(asset);
    char *buffer = new char[dataFileSize];
    int numBytesRead = AAsset_read(asset, buffer, dataFileSize);
    AAsset_close(asset);
    std::istringstream iss(std::string(buffer, numBytesRead));
    core = CHelper::Core::create([&iss]() {
        CHelper::BinaryReader binaryReader(true, iss);
        return CHelper::CPack::createByBinary(binaryReader);
    });
    delete[] buffer;
    return static_cast<jboolean>(core != nullptr);
}

extern "C" [[maybe_unused]] JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_onTextChanged(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jstring cpack_path, jint index) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return;
    }
    std::string content = jstring2string(env, cpack_path);
    core->onTextChanged(content, index);
}

extern "C" [[maybe_unused]] JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_onSelectionChanged(
        [[maybe_unused]] JNIEnv *env, [[maybe_unused]] jobject thiz, jint index) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return;
    }
    core->onSelectionChanged(index);
}

extern "C" [[maybe_unused]] JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getDescription(
        JNIEnv *env, [[maybe_unused]] jobject thiz) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return env->NewStringUTF(core->getDescription().c_str());
}

extern "C" [[maybe_unused]] JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getErrorReasons(
        JNIEnv *env, [[maybe_unused]] jobject thiz) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    auto errorReasons = core->getErrorReasons();
    if (HEDLEY_UNLIKELY(errorReasons.empty())) {
        return nullptr;
    } else if (HEDLEY_UNLIKELY(errorReasons.size() == 1)) {
        return env->NewStringUTF(errorReasons[0]->errorReason.c_str());
    } else {
        std::string result = "可能的错误原因：";
        int i = 0;
        for (const auto &item: errorReasons) {
            result.append("\n").append(std::to_string(++i)).append(". ").append(item->errorReason);
        }
        return env->NewStringUTF(result.c_str());
    }
}

extern "C" [[maybe_unused]] JNIEXPORT jlong JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestionsSize(
        [[maybe_unused]] JNIEnv *env, [[maybe_unused]] jint thiz) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return 0;
    }
    return static_cast<jint>(core->getSuggestions()->size());
}

extern "C" [[maybe_unused]] JNIEXPORT jobject JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestion(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jint which) {
    if (HEDLEY_UNLIKELY(core == nullptr || which < 0)) {
        return nullptr;
    }
    auto suggestions = core->getSuggestions();
    if (HEDLEY_UNLIKELY(suggestions->size() <= which)) {
        return nullptr;
    }
    CHelper::Suggestion suggestion = suggestions->at(which);
    jobject javaDataComplete = env->AllocObject(env->FindClass("yancey/chelper/util/DataComplete"));
    env->SetObjectField(javaDataComplete,
                        env->GetFieldID(env->GetObjectClass(javaDataComplete), "name", "Ljava/lang/String;"),
                        env->NewStringUTF(suggestion.content->name.c_str()));
    env->SetObjectField(javaDataComplete,
                        env->GetFieldID(env->GetObjectClass(javaDataComplete), "description", "Ljava/lang/String;"),
                        suggestion.content->description.has_value()
                                ? env->NewStringUTF(suggestion.content->description.value().c_str())
                                : nullptr);
    return javaDataComplete;
}

extern "C" [[maybe_unused]] JNIEXPORT jobject JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestions(
        JNIEnv *env, [[maybe_unused]] jobject thiz) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    auto suggestions = core->getSuggestions();
    if (HEDLEY_UNLIKELY(suggestions->empty())) {
        return nullptr;
    }
    jclass arrayListClass = env->FindClass("java/util/ArrayList");
    jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
    jmethodID arrayListAdd = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");
    jclass dataCompleteClass = env->FindClass("yancey/chelper/util/DataComplete");
    jobject javaList = env->NewObject(arrayListClass, arrayListConstructor);
    for (const auto &item: *suggestions) {
        jobject javaDataComplete = env->AllocObject(dataCompleteClass);
        env->SetObjectField(javaDataComplete,
                            env->GetFieldID(env->GetObjectClass(javaDataComplete), "name", "Ljava/lang/String;"),
                            env->NewStringUTF(item.content->name.c_str()));
        env->SetObjectField(javaDataComplete,
                            env->GetFieldID(env->GetObjectClass(javaDataComplete), "description", "Ljava/lang/String;"),
                            item.content->description.has_value()
                                    ? env->NewStringUTF(item.content->description.value().c_str())
                                    : nullptr);
        env->CallBooleanMethod(javaList, arrayListAdd, javaDataComplete);
    }
    return javaList;
}

extern "C" [[maybe_unused]] JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getStructure(
        JNIEnv *env, [[maybe_unused]] jobject thiz) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return env->NewStringUTF(core->getStructure().c_str());
}

extern "C" [[maybe_unused]] JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_onSuggestionClick(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jint which) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    std::optional<std::string> result = core->onSuggestionClick(which);
    if (HEDLEY_LIKELY(result.has_value())) {
        return env->NewStringUTF(result.value().c_str());
    } else {
        return nullptr;
    }
}

#endif//CHELPER_ANDROIDNATIVE_H