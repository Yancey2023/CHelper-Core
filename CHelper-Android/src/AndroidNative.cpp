//
// Created by Yancey on 2024/2/24.
//

#include <chelper/CHelperCore.h>
#include <pch.h>

std::u16string jstring2u16string(JNIEnv *env, jstring jString) {
    if (HEDLEY_UNLIKELY(jString == nullptr)) {
        CHELPER_WARN("call jstring2u16string when jString is null");
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
        CHELPER_WARN("call jstring2string when jString is null");
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

extern "C" [[maybe_unused]] JNIEXPORT jlong JNICALL
Java_yancey_chelper_core_CHelperCore_create0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jobject assetManager, jstring cpack_path) {
    if (cpack_path == nullptr) {
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_create0 when cpack_path is null");
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
            size_t dataFileSize = AAsset_getLength(asset);
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
        CHELPER_WARN("fail to init CHelper Core");
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
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_onTextChanged0 when core is nullptr");
        return;
    }
    if (HEDLEY_UNLIKELY(text == nullptr)) {
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_onTextChanged0 when core is nullptr");
        return;
    }
    core->onTextChanged(jstring2u16string(env, text), index);
}

extern "C" [[maybe_unused]] JNIEXPORT void JNICALL
Java_yancey_chelper_core_CHelperCore_onSelectionChanged0(
        [[maybe_unused]] JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer, jint index) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_onSelectionChanged0 when core is nullptr");
        return;
    }
    core->onSelectionChanged(index);
}

extern "C" [[maybe_unused]] JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getDescription0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_getDescription0 when core is nullptr");
        return nullptr;
    }
    return u16string2jstring(env, core->getDescription());
}

extern "C" [[maybe_unused]] JNIEXPORT jobjectArray JNICALL
Java_yancey_chelper_core_CHelperCore_getErrorReasons0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    jclass errorReasonClass = env->FindClass("yancey/chelper/core/ErrorReason");
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_getErrorReasons0 when core is nullptr");
        return env->NewObjectArray(0, errorReasonClass, nullptr);
    }
    auto errorReasons = core->getErrorReasons();
    jobjectArray result = env->NewObjectArray(static_cast<jsize>(errorReasons.size()), errorReasonClass, nullptr);
    for (int i = 0; i < errorReasons.size(); ++i) {
        const CHelper::ErrorReason &item = *errorReasons[i];
        jobject javaErrorReason = env->AllocObject(errorReasonClass);
        env->SetObjectField(javaErrorReason,
                            env->GetFieldID(errorReasonClass, "errorReason", "Ljava/lang/String;"),
                            u16string2jstring(env, item.errorReason));
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
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_getSuggestionsSize0 when core is nullptr");
        return 0;
    }
    return static_cast<jint>(core->getSuggestions()->size());
}

extern "C" [[maybe_unused]] JNIEXPORT jobject JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestion0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer, jint which) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_getSuggestion0 when core is nullptr");
        return nullptr;
    }
    if (HEDLEY_UNLIKELY(which < 0)) {
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_getSuggestion0 when which < 0");
        return nullptr;
    }
    auto suggestions = core->getSuggestions();
    if (HEDLEY_UNLIKELY(suggestions->size() <= which)) {
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_getSuggestion0 when suggestions->size() <= which");
        return nullptr;
    }
    CHelper::Suggestion suggestion = suggestions->at(which);
    jclass suggestionClass = env->FindClass("yancey/chelper/core/Suggestion");
    jobject javaSuggestion = env->AllocObject(suggestionClass);
    env->SetObjectField(javaSuggestion,
                        env->GetFieldID(suggestionClass, "name", "Ljava/lang/String;"),
                        u16string2jstring(env, suggestion.content->name));
    env->SetObjectField(javaSuggestion,
                        env->GetFieldID(suggestionClass, "description", "Ljava/lang/String;"),
                        suggestion.content->description.has_value()
                                ? u16string2jstring(env, suggestion.content->description.value())
                                : nullptr);
    return javaSuggestion;
}

extern "C" [[maybe_unused]] JNIEXPORT jobject JNICALL
Java_yancey_chelper_core_CHelperCore_getSuggestions0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    jclass suggestionClass = env->FindClass("yancey/chelper/core/Suggestion");
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_getSuggestions0 when core is nullptr");
        return env->NewObjectArray(0, suggestionClass, nullptr);
    }
    const std::vector<CHelper::Suggestion> &suggestions = *core->getSuggestions();
    jobjectArray result = env->NewObjectArray(static_cast<jsize>(suggestions.size()), suggestionClass, nullptr);
    for (int i = 0; i < suggestions.size(); ++i) {
        const CHelper::Suggestion &item = suggestions[i];
        jobject javaSuggestion = env->AllocObject(suggestionClass);
        env->SetObjectField(javaSuggestion,
                            env->GetFieldID(suggestionClass, "name", "Ljava/lang/String;"),
                            u16string2jstring(env, item.content->name));
        env->SetObjectField(javaSuggestion,
                            env->GetFieldID(suggestionClass, "description", "Ljava/lang/String;"),
                            item.content->description.has_value()
                                    ? u16string2jstring(env, item.content->description.value())
                                    : nullptr);
        env->SetObjectArrayElement(result, i, javaSuggestion);
    }
    return result;
}

extern "C" [[maybe_unused]] JNIEXPORT jstring JNICALL
Java_yancey_chelper_core_CHelperCore_getStructure0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_getStructure0 when core is nullptr");
        return nullptr;
    }
    return u16string2jstring(env, core->getStructure());
}

extern "C" [[maybe_unused]] JNIEXPORT jobject JNICALL
Java_yancey_chelper_core_CHelperCore_onSuggestionClick0(
        JNIEnv *env, [[maybe_unused]] jobject thiz, jlong pointer, jint which) {
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_onSuggestionClick0 when core is nullptr");
        return nullptr;
    }
    std::optional<std::pair<std::u16string, size_t>> result = core->onSuggestionClick(which);
    if (HEDLEY_LIKELY(result.has_value())) {
        jclass resultClass = env->FindClass("yancey/chelper/core/ClickSuggestionResult");
        jobject javaResult = env->AllocObject(resultClass);
        env->SetObjectField(javaResult,
                            env->GetFieldID(resultClass, "text", "Ljava/lang/String;"),
                            u16string2jstring(env, result.value().first));
        env->SetIntField(javaResult,
                         env->GetFieldID(resultClass, "selection", "I"),
                         static_cast<jint>(result.value().second));
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
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_getColors0 when core is nullptr");
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
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_setTheme0 when theme is nullptr");
        return;
    }
    auto *core = reinterpret_cast<CHelper::CHelperCore *>(pointer);
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_setTheme0 when core is nullptr");
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
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_old2newInit0 when blockFixDataPath is nullptr");
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
        CHELPER_WARN("call Java_yancey_chelper_core_CHelperCore_old2new0 when old is nullptr");
        return nullptr;
    }
    return u16string2jstring(env, CHelper::CHelperCore::old2new(blockFixData0, jstring2u16string(env, old)));
}
