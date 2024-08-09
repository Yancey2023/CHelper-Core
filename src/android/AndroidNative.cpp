//
// Created by Yancey on 2024/2/24.
//

#include "../chelper/Core.h"
#include "pch.h"

std::string jstring2string(JNIEnv *env, jstring jStr) {
    const char *cstr = env->GetStringUTFChars(jStr, nullptr);
    std::string str = std::string(cstr);
    env->ReleaseStringUTFChars(jStr, cstr);
    return str;
}

extern "C" [[maybe_unused]] JNIEXPORT jlong JNICALL
Java_yancey_chelper_core_CHelperCore_create0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jobject assetManager, jstring cpack_path) {
    if (cpack_path == nullptr) {
        return reinterpret_cast<jlong>(nullptr);
    }
    try {
        std::string cpackPath = jstring2string(env, cpack_path);
        if (HEDLEY_UNLIKELY(assetManager == nullptr)) {
            CHelper::Core *core = CHelper::Core::createByBinary(cpackPath);
            return reinterpret_cast<jlong>(core);
        } else {
            AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
            AAsset *asset = AAssetManager_open(mgr, cpackPath.c_str(), AASSET_MODE_BUFFER);
            if (HEDLEY_UNLIKELY(asset == nullptr)) {
                return reinterpret_cast<jlong>(nullptr);
            }
            size_t dataFileSize = AAsset_getLength(asset);
            char *buffer = new char[dataFileSize];
            int numBytesRead = AAsset_read(asset, buffer, dataFileSize);
            AAsset_close(asset);
            std::istringstream iss(std::string(buffer, numBytesRead));
            CHelper::Core *core = CHelper::Core::create([&iss]() {
                CHelper::BinaryReader binaryReader(true, iss);
                return CHelper::CPack::createByBinary(binaryReader);
            });
            delete[] buffer;
            return reinterpret_cast<jlong>(core);
        }
    } catch (...) {
        return reinterpret_cast<jlong>(nullptr);
    }
}

extern "C" [[maybe_unused]] JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_release0(
        [[maybe_unused]] JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    delete reinterpret_cast<CHelper::Core *>(pointer);
}

extern "C" [[maybe_unused]] JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_onTextChanged0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer, jstring text, jint index) {
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr || text == nullptr)) {
        return;
    }
    core->onTextChanged(jstring2string(env, text), index);
}

extern "C" [[maybe_unused]] JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_onSelectionChanged0(
        [[maybe_unused]] JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer, jint index) {
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return;
    }
    core->onSelectionChanged(index);
}

extern "C" [[maybe_unused]] JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getDescription0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return env->NewStringUTF(core->getDescription().c_str());
}

extern "C" [[maybe_unused]] JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getErrorReasons0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
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
Java_yancey_chelper_core_CHelperCore_getSuggestionsSize0(
        [[maybe_unused]] JNIEnv *env, [[maybe_unused]] jint thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return 0;
    }
    return static_cast<jint>(core->getSuggestions()->size());
}

extern "C" [[maybe_unused]] JNIEXPORT jobject JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestion0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer, jint which) {
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr || which < 0)) {
        return nullptr;
    }
    auto suggestions = core->getSuggestions();
    if (HEDLEY_UNLIKELY(suggestions->size() <= which)) {
        return nullptr;
    }
    CHelper::Suggestion suggestion = suggestions->at(which);
    jobject javaDataComplete = env->AllocObject(env->FindClass("yancey/chelper/core/Suggestion"));
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
Java_yancey_chelper_core_CHelperCore_getSuggestions0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
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
    jclass dataCompleteClass = env->FindClass("yancey/chelper/core/Suggestion");
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
Java_yancey_chelper_core_CHelperCore_getStructure0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return env->NewStringUTF(core->getStructure().c_str());
}

extern "C" [[maybe_unused]] JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_onSuggestionClick0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer, jint which) {
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
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

extern "C" [[maybe_unused]] JNIEXPORT jintArray JNICALL
Java_yancey_chelper_core_CHelperCore_getColors0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    CHelper::ColoredString coloredString = core->getColors();
    size_t size = coloredString.colors.size();
    jint *colors = new jint[size];
    for (int i = 0; i < size; ++i) {
        colors[i] = static_cast<jint>(coloredString.colors[i]);
    }
    jintArray result = env->NewIntArray(static_cast<jsize>(size));
    env->SetIntArrayRegion(result, 0, static_cast<jsize>(size), colors);
    delete[] colors;
    return result;
}

extern "C" [[maybe_unused]] JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_setTheme0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer, jobject theme) {
    if (HEDLEY_UNLIKELY(theme == nullptr)) {
        return;
    }
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return;
    }
    jclass jclassTheme = env->GetObjectClass(theme);
    core->settings.theme.colorBoolean = env->GetIntField(theme, env->GetFieldID(jclassTheme, "colorBoolean", "I"));
    core->settings.theme.colorFloat = env->GetIntField(theme, env->GetFieldID(jclassTheme, "colorFloat", "I"));
    core->settings.theme.colorInteger = env->GetIntField(theme, env->GetFieldID(jclassTheme, "colorInteger", "I"));
    core->settings.theme.colorSymbol = env->GetIntField(theme, env->GetFieldID(jclassTheme, "colorSymbol", "I"));
    core->settings.theme.colorId = env->GetIntField(theme, env->GetFieldID(jclassTheme, "colorId", "I"));
    core->settings.theme.colorTargetSelector = env->GetIntField(theme, env->GetFieldID(jclassTheme, "colorTargetSelector", "I"));
    core->settings.theme.colorCommand = env->GetIntField(theme, env->GetFieldID(jclassTheme, "colorCommand", "I"));
    core->settings.theme.colorBrackets1 = env->GetIntField(theme, env->GetFieldID(jclassTheme, "colorBrackets1", "I"));
    core->settings.theme.colorBrackets2 = env->GetIntField(theme, env->GetFieldID(jclassTheme, "colorBrackets2", "I"));
    core->settings.theme.colorBrackets3 = env->GetIntField(theme, env->GetFieldID(jclassTheme, "colorBrackets3", "I"));
    core->settings.theme.colorString = env->GetIntField(theme, env->GetFieldID(jclassTheme, "colorString", "I"));
    core->settings.theme.colorNull = env->GetIntField(theme, env->GetFieldID(jclassTheme, "colorNull", "I"));
    core->settings.theme.colorRange = env->GetIntField(theme, env->GetFieldID(jclassTheme, "colorRange", "I"));
    core->settings.theme.colorLiteral = env->GetIntField(theme, env->GetFieldID(jclassTheme, "colorLiteral", "I"));
}

CHelper::Old2New::BlockFixData blockFixData0;

extern "C" [[maybe_unused]] JNIEXPORT jboolean JNICALL
Java_yancey_chelper_core_CHelperCore_old2newInit0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jobject assetManager, jstring blockFixDataPath) {
    if (HEDLEY_UNLIKELY(blockFixDataPath == nullptr)) {
        return false;
    }
    std::string blockFixDataPath0 = jstring2string(env, blockFixDataPath);
    try {
        AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
        AAsset *asset = AAssetManager_open(mgr, blockFixDataPath0.c_str(), AASSET_MODE_BUFFER);
        if (HEDLEY_UNLIKELY(asset == nullptr)) {
            return false;
        }
        size_t dataFileSize = AAsset_getLength(asset);
        char *buffer = new char[dataFileSize];
        int numBytesRead = AAsset_read(asset, buffer, dataFileSize);
        AAsset_close(asset);
        std::istringstream iss(std::string(buffer, numBytesRead));
        CHelper::BinaryReader binaryReader(true, iss);
        blockFixData0 = binaryReader.read<CHelper::Old2New::BlockFixData>();
        delete[] buffer;
        return true;
    } catch (const std::exception &e) {
        return false;
    }
}

extern "C" [[maybe_unused]] JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_old2new0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jstring old) {
    if (HEDLEY_UNLIKELY(old == nullptr)) {
        return old;
    }
    return env->NewStringUTF(CHelper::Core::old2new(blockFixData0, jstring2string(env, old)).c_str());
}
