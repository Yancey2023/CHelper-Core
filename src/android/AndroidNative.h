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
    return static_cast<jboolean>(core != nullptr);
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
            result.append("\n").append(std::to_string(++i)).append(". ").append(item->errorReason);
        }
        return env->NewStringUTF(result.c_str());
    }
}

extern "C" JNIEXPORT jobject JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestions(
        JNIEnv *env, jobject thiz) {
    if (core == nullptr) {
        return nullptr;
    }
    auto suggestions = core->getSuggestions();
    if (suggestions->empty()) {
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
                            item.content->description.has_value() ? env->NewStringUTF(
                                    item.content->description.value().c_str()) : nullptr);
        env->CallBooleanMethod(javaList, arrayListAdd, javaDataComplete);
    }
    return javaList;
}

extern "C" JNIEXPORT jobject JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestion(
        JNIEnv *env, jobject thiz, jint which) {
    if (core == nullptr || which < 0) {
        return nullptr;
    }
    auto suggestions = core->getSuggestions();
    if (suggestions->size() >= which) {
        return nullptr;
    }
    CHelper::Suggestion suggestion = suggestions->at(which);
    jobject javaDataComplete = env->AllocObject(env->FindClass("yancey/chelper/util/DataComplete"));
    env->SetObjectField(javaDataComplete,
                        env->GetFieldID(env->GetObjectClass(javaDataComplete), "name", "Ljava/lang/String;"),
                        env->NewStringUTF(suggestion.content->name.c_str()));
    env->SetObjectField(javaDataComplete,
                        env->GetFieldID(env->GetObjectClass(javaDataComplete), "description", "Ljava/lang/String;"),
                        suggestion.content->description.has_value() ? env->NewStringUTF(
                                suggestion.content->description.value().c_str()) : nullptr);
    return javaDataComplete;
}

extern "C" JNIEXPORT jlong JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestionsSize(
        JNIEnv *env, jint thiz) {
    if (core == nullptr) {
        return 0;
    }
    return static_cast<jint>(core->getSuggestions()->size());
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