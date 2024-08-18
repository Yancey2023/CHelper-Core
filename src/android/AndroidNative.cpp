//
// Created by Yancey on 2024/2/24.
//

#include "../chelper/Core.h"
#include "pch.h"
#include <codecvt>
#include <locale>
#include <unicode/ustring.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

// TODO these two method is slow and codecvt is deprecated in c++17

static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;

std::string wstring2string(const std::wstring &wstring) {
    return utf8_conv.to_bytes(wstring);
}

std::wstring string2wstring(const std::string &string) {
    //    icu::UnicodeString unicodeString;
    //    u_strlen(string);
    //    auto *dest = new UChar[string.size()];
    //    UErrorCode errorCode;
    //    int32_t destLength;
    //    u_strFromUTF8(dest, 100, &destLength, string.c_str(), static_cast<int32_t>(string.length()), &errorCode);
    //    std::wstring result = std::wstring(reinterpret_cast<wchar_t *>(dest));
    //    delete[] dest;
    //    return result;
    return utf8_conv.from_bytes(string);
}

#pragma clang diagnostic pop

std::wstring jstring2wstring(JNIEnv *env, jstring jString) {
    if (HEDLEY_UNLIKELY(jString == nullptr)) {
        return {};
    }
    jsize length = env->GetStringLength(jString);
    const jchar *jchars = env->GetStringChars(jString, nullptr);
    std::wstring str = std::wstring(reinterpret_cast<const wchar_t *>(jchars), length);
    env->ReleaseStringChars(jString, jchars);
    return str;
}

jstring wstring2jstring(JNIEnv *env, const std::wstring &wstring) {
    return env->NewString(reinterpret_cast<const jchar *>(wstring.c_str()), static_cast<jsize>(wstring.size()));
}

std::string jstring2string(JNIEnv *env, jstring jString) {
    if (HEDLEY_UNLIKELY(jString == nullptr)) {
        return {};
    }
    const char *cstr = env->GetStringUTFChars(jString, nullptr);
    std::string str = cstr;
    env->ReleaseStringUTFChars(jString, cstr);
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
    core->onTextChanged(jstring2wstring(env, text), index);
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
    return wstring2jstring(env, core->getDescription());
}

extern "C" [[maybe_unused]] JNIEXPORT jobjectArray JNICALL
Java_yancey_chelper_core_CHelperCore_getErrorReasons0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
    jclass errorReasonClass = env->FindClass("yancey/chelper/core/ErrorReason");
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return env->NewObjectArray(0, errorReasonClass, nullptr);
    }
    auto errorReasons = core->getErrorReasons();
    jobjectArray result = env->NewObjectArray(static_cast<jsize>(errorReasons.size()), errorReasonClass, nullptr);
    for (int i = 0; i < errorReasons.size(); ++i) {
        const CHelper::ErrorReason &item = *errorReasons[i];
        jobject javaErrorReason = env->AllocObject(errorReasonClass);
        env->SetObjectField(javaErrorReason,
                            env->GetFieldID(errorReasonClass, "errorReason", "Ljava/lang/String;"),
                            wstring2jstring(env, item.errorReason));
        env->SetIntField(javaErrorReason,
                         env->GetFieldID(errorReasonClass, "start", "I"),
                         static_cast<jint>(item.start));
        env->SetIntField(javaErrorReason,
                         env->GetFieldID(errorReasonClass, "end", "I"),
                         static_cast<jint>(item.end));
        env->SetObjectArrayElement(result, i, javaErrorReason);
    }
    return result;
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
    jclass suggestionClass = env->FindClass("yancey/chelper/core/Suggestion");
    jobject javaSuggestion = env->AllocObject(suggestionClass);
    env->SetObjectField(javaSuggestion,
                        env->GetFieldID(suggestionClass, "name", "Ljava/lang/String;"),
                        wstring2jstring(env, suggestion.content->name));
    env->SetObjectField(javaSuggestion,
                        env->GetFieldID(suggestionClass, "description", "Ljava/lang/String;"),
                        suggestion.content->description.has_value()
                                ? wstring2jstring(env, suggestion.content->description.value())
                                : nullptr);
    return javaSuggestion;
}

extern "C" [[maybe_unused]] JNIEXPORT jobject JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestions0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
    jclass suggestionClass = env->FindClass("yancey/chelper/core/Suggestion");
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return env->NewObjectArray(0, suggestionClass, nullptr);
    }
    const std::vector<CHelper::Suggestion> &suggestions = *core->getSuggestions();
    if (HEDLEY_UNLIKELY(suggestions.empty())) {
        return env->NewObjectArray(0, suggestionClass, nullptr);
    }
    jobjectArray result = env->NewObjectArray(suggestions.size(), suggestionClass, nullptr);
    for (int i = 0; i < suggestions.size(); ++i) {
        const CHelper::Suggestion &item = suggestions[i];
        jobject javaSuggestion = env->AllocObject(suggestionClass);
        env->SetObjectField(javaSuggestion,
                            env->GetFieldID(suggestionClass, "name", "Ljava/lang/String;"),
                            wstring2jstring(env, item.content->name));
        env->SetObjectField(javaSuggestion,
                            env->GetFieldID(suggestionClass, "description", "Ljava/lang/String;"),
                            item.content->description.has_value()
                                    ? wstring2jstring(env, item.content->description.value())
                                    : nullptr);
        env->SetObjectArrayElement(result, i, javaSuggestion);
    }
    return result;
}

extern "C" [[maybe_unused]] JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getStructure0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return wstring2jstring(env, core->getStructure());
}

extern "C" [[maybe_unused]] JNIEXPORT jobject JNICALL
Java_yancey_chelper_core_CHelperCore_onSuggestionClick0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer, jint which) {
    auto *core = reinterpret_cast<CHelper::Core *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    std::optional<std::pair<std::wstring, size_t>> result = core->onSuggestionClick(which);
    if (HEDLEY_LIKELY(result.has_value())) {
        jclass resultClass = env->FindClass("yancey/chelper/core/ClickSuggestionResult");
        jobject javaResult = env->AllocObject(resultClass);
        env->SetObjectField(javaResult,
                            env->GetFieldID(resultClass, "text", "Ljava/lang/String;"),
                            wstring2jstring(env, result.value().first));
        env->SetIntField(javaResult,
                         env->GetFieldID(resultClass, "selection", "I"),
                         static_cast<jint>(result.value().second));
        return resultClass;
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
    return wstring2jstring(env, CHelper::Core::old2new(blockFixData0, jstring2wstring(env, old)));
}
