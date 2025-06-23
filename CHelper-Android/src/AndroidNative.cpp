//
// Created by Yancey on 2024/2/24.
//

#include <chelper/CHelperCore.h>
#include <pch.h>
#include <spdlog/sinks/android_sink.h>

std::u16string jstring2u16string(JNIEnv *env, jstring jString) {
    if (HEDLEY_UNLIKELY(jString == nullptr)) {
        SPDLOG_WARN("call jstring2u16string when jString is null");
        return {};
    }
    jsize length = env->GetStringLength(jString);
    const jchar *jchars = env->GetStringChars(jString, nullptr);
    std::u16string str = std::u16string(reinterpret_cast<const char16_t *>(jchars), length);
    env->ReleaseStringChars(jString, jchars);
    return str;
}

jstring u16string2jstring(JNIEnv *env, const std::u16string &u16string) {
    return env->NewString(reinterpret_cast<const jchar *>(u16string.c_str()), static_cast<jsize>(u16string.size()));
}

std::string jstring2string(JNIEnv *env, jstring jString) {
    if (HEDLEY_UNLIKELY(jString == nullptr)) {
        SPDLOG_WARN("call jstring2string when jString is null");
        return {};
    }
    const char *cstr = env->GetStringUTFChars(jString, nullptr);
    std::string str = cstr;
    env->ReleaseStringUTFChars(jString, cstr);
    return str;
}

jstring string2jstring(JNIEnv *env, const std::string &string) {
    return env->NewStringUTF(string.c_str());
}

static jclass errorReasonClass;
static jfieldID errorReasonFieldId;
static jfieldID startFieldId;
static jfieldID endFieldId;
static jclass suggestionClass;
static jfieldID nameFieldId;
static jfieldID descriptionFieldId;
static jclass clickSuggestionResultClass;
static jfieldID textFieldId;
static jfieldID selectionnFieldId;

extern "C" [[maybe_unused]] JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_init0(
        JNIEnv *env, [[maybe_unused]] jobject thiz) {
    spdlog::set_default_logger(spdlog::android_logger_mt("android", "CHelperNative"));
    errorReasonClass = env->FindClass("yancey/chelper/core/ErrorReason");
    errorReasonFieldId = env->GetFieldID(errorReasonClass, "errorReason", "Ljava/lang/String;");
    startFieldId = env->GetFieldID(errorReasonClass, "start", "I");
    endFieldId = env->GetFieldID(errorReasonClass, "end", "I");
    suggestionClass = env->FindClass("yancey/chelper/core/Suggestion");
    nameFieldId = env->GetFieldID(suggestionClass, "name", "Ljava/lang/String;");
    descriptionFieldId = env->GetFieldID(suggestionClass, "description", "Ljava/lang/String;");
    clickSuggestionResultClass = env->FindClass("yancey/chelper/core/ClickSuggestionResult");
    textFieldId = env->GetFieldID(clickSuggestionResultClass, "text", "Ljava/lang/String;");
    selectionnFieldId = env->GetFieldID(clickSuggestionResultClass, "selection", "I");
}

extern "C" [[maybe_unused]] JNIEXPORT jlong JNICALL
Java_yancey_chelper_core_CHelperCore_create0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jobject assetManager, jstring cpack_path) {
    if (cpack_path == nullptr) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_create0 when cpack_path is null");
        return reinterpret_cast<jlong>(nullptr);
    }
    try {
        std::string cpackPath = jstring2string(env, cpack_path);
        if (HEDLEY_UNLIKELY(assetManager == nullptr)) {
            CHelper::CHelperCore *core = CHelper::CHelperCore::createByBinary(cpackPath);
            return reinterpret_cast<jlong>(core);
        } else {
            AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
            AAsset *asset = AAssetManager_open(mgr, cpackPath.c_str(), AASSET_MODE_BUFFER);
            if (HEDLEY_UNLIKELY(asset == nullptr)) {
                return reinterpret_cast<jlong>(nullptr);
            }
            auto dataFileSize = static_cast<size_t>(AAsset_getLength(asset));
            char *buffer = new char[dataFileSize];
            int numBytesRead = AAsset_read(asset, buffer, dataFileSize);
            AAsset_close(asset);
            std::istringstream iss(std::string(buffer, numBytesRead));
            CHelper::CHelperCore *core = CHelper::CHelperCore::create([&iss]() {
                return CHelper::CPack::createByBinary(iss);
            });
            delete[] buffer;
            return reinterpret_cast<jlong>(core);
        }
    } catch (...) {
        SPDLOG_WARN("fail to init CHelper Core");
        return reinterpret_cast<jlong>(nullptr);
    }
}

extern "C" [[maybe_unused]] JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_release0(
        [[maybe_unused]] JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    delete reinterpret_cast<CHelper::CHelperCore *>(pointer);
}

extern "C" [[maybe_unused]] JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_onTextChanged0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer, jstring text, jint index) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_onTextChanged0 when core is nullptr");
        return;
    }
    if (HEDLEY_UNLIKELY(text == nullptr)) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_onTextChanged0 when core is nullptr");
        return;
    }
    core->onTextChanged(jstring2u16string(env, text), index);
}

extern "C" [[maybe_unused]] JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_onSelectionChanged0(
        [[maybe_unused]] JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer, jint index) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_onSelectionChanged0 when core is nullptr");
        return;
    }
    core->onSelectionChanged(index);
}

extern "C" [[maybe_unused]] JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getDescription0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_getDescription0 when core is nullptr");
        return nullptr;
    }
    return u16string2jstring(env, core->getDescription());
}

extern "C" [[maybe_unused]] JNIEXPORT jobjectArray JNICALL
Java_yancey_chelper_core_CHelperCore_getErrorReasons0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_getErrorReasons0 when core is nullptr");
        return env->NewObjectArray(0, errorReasonClass, nullptr);
    }
    auto errorReasons = core->getErrorReasons();
    jobjectArray result = env->NewObjectArray(static_cast<jsize>(errorReasons.size()), errorReasonClass, nullptr);
    for (size_t i = 0; i < errorReasons.size(); ++i) {
        const CHelper::ErrorReason &item = *errorReasons[i];
        jobject javaErrorReason = env->AllocObject(errorReasonClass);
        env->SetObjectField(javaErrorReason, errorReasonFieldId, u16string2jstring(env, item.errorReason));
        env->SetIntField(javaErrorReason, startFieldId, static_cast<jint>(item.start));
        env->SetIntField(javaErrorReason, endFieldId, static_cast<jint>(item.end));
        env->SetObjectArrayElement(result, static_cast<jint>(i), javaErrorReason);
    }
    return result;
}

extern "C" [[maybe_unused]] JNIEXPORT jlong JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestionsSize0(
        [[maybe_unused]] JNIEnv *env, [[maybe_unused]] jint thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_getSuggestionsSize0 when core is nullptr");
        return 0;
    }
    return static_cast<jint>(core->getSuggestions()->size());
}

extern "C" [[maybe_unused]] JNIEXPORT jobject JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestion0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer, jint which) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_getSuggestion0 when core is nullptr");
        return nullptr;
    }
    if (HEDLEY_UNLIKELY(which < 0)) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_getSuggestion0 when which < 0");
        return nullptr;
    }
    auto suggestions = core->getSuggestions();
    if (HEDLEY_UNLIKELY(static_cast<jint>(suggestions->size()) <= which)) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_getSuggestion0 when suggestions->size() <= which");
        return nullptr;
    }
    CHelper::AutoSuggestion::Suggestion suggestion = suggestions->at(which);
    jobject javaSuggestion = env->AllocObject(suggestionClass);
    env->SetObjectField(javaSuggestion, nameFieldId, u16string2jstring(env, suggestion.content->name));
    env->SetObjectField(javaSuggestion, descriptionFieldId, suggestion.content->description.has_value() ? u16string2jstring(env, suggestion.content->description.value()) : nullptr);
    return javaSuggestion;
}

extern "C" [[maybe_unused]] JNIEXPORT jobject JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestions0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_getSuggestions0 when core is nullptr");
        return env->NewObjectArray(0, suggestionClass, nullptr);
    }
    const std::vector<CHelper::AutoSuggestion::Suggestion> &suggestions = *core->getSuggestions();
    jobjectArray result = env->NewObjectArray(static_cast<jsize>(suggestions.size()), suggestionClass, nullptr);
    for (size_t i = 0; i < suggestions.size(); ++i) {
        const CHelper::AutoSuggestion::Suggestion &item = suggestions[i];
        jobject javaSuggestion = env->AllocObject(suggestionClass);
        env->SetObjectField(javaSuggestion, nameFieldId, u16string2jstring(env, item.content->name));
        env->SetObjectField(javaSuggestion, descriptionFieldId, item.content->description.has_value() ? u16string2jstring(env, item.content->description.value()) : nullptr);
        env->SetObjectArrayElement(result, static_cast<jint>(i), javaSuggestion);
    }
    return result;
}

extern "C" [[maybe_unused]] JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getStructure0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_getStructure0 when core is nullptr");
        return nullptr;
    }
    return u16string2jstring(env, core->getStructure());
}

extern "C" [[maybe_unused]] JNIEXPORT jobject JNICALL
Java_yancey_chelper_core_CHelperCore_onSuggestionClick0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer, jint which) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_onSuggestionClick0 when core is nullptr");
        return nullptr;
    }
    std::optional<std::pair<std::u16string, size_t>> result = core->onSuggestionClick(which);
    if (HEDLEY_LIKELY(result.has_value())) {
        jobject javaResult = env->AllocObject(clickSuggestionResultClass);
        env->SetObjectField(javaResult, textFieldId, u16string2jstring(env, result.value().first));
        env->SetIntField(javaResult, selectionnFieldId, static_cast<jint>(result.value().second));
        return javaResult;
    } else {
        return nullptr;
    }
}

extern "C" [[maybe_unused]] JNIEXPORT jintArray JNICALL
Java_yancey_chelper_core_CHelperCore_getColors0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_getColors0 when core is nullptr");
        return nullptr;
    }
    auto syntaxResult = core->getSyntaxResult();
    size_t size = syntaxResult.tokenTypes.size();
    jint *tokenTypes = new jint[size];
    for (size_t i = 0; i < size; ++i) {
        tokenTypes[i] = static_cast<jint>(syntaxResult.tokenTypes[i]);
    }
    jintArray result = env->NewIntArray(static_cast<jsize>(size));
    env->SetIntArrayRegion(result, 0, static_cast<jsize>(size), tokenTypes);
    delete[] tokenTypes;
    return result;
}

CHelper::Old2New::BlockFixData blockFixData0;

extern "C" [[maybe_unused]] JNIEXPORT jboolean JNICALL
Java_yancey_chelper_core_CHelperCore_old2newInit0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jobject assetManager, jstring blockFixDataPath) {
    if (HEDLEY_UNLIKELY(blockFixDataPath == nullptr)) {
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_old2newInit0 when blockFixDataPath is nullptr");
        return false;
    }
    std::string blockFixDataPath0 = jstring2string(env, blockFixDataPath);
    try {
        AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
        AAsset *asset = AAssetManager_open(mgr, blockFixDataPath0.c_str(), AASSET_MODE_BUFFER);
        if (HEDLEY_UNLIKELY(asset == nullptr)) {
            return false;
        }
        auto dataFileSize = static_cast<size_t>(AAsset_getLength(asset));
        char *buffer = new char[dataFileSize];
        int numBytesRead = AAsset_read(asset, buffer, dataFileSize);
        AAsset_close(asset);
        std::istringstream iss(std::string(buffer, numBytesRead));
        serialization::from_binary<true>(iss, blockFixData0);
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
        SPDLOG_WARN("call Java_yancey_chelper_core_CHelperCore_old2new0 when old is nullptr");
        return nullptr;
    }
    return u16string2jstring(env, CHelper::CHelperCore::old2new(blockFixData0, jstring2u16string(env, old)));
}
