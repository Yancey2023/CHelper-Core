//
// Created by Yancey on 2024/4/2.
//

#include "../chelper/Core.h"
#include <emscripten/emscripten.h>
#include <codecvt>
#include <locale>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

// these two method is slow and codecvt is deprecated in c++17
// do not use this implementation in your project.
// you should implement it depend on your platform, such as use Windows API.

static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;

std::string wstring2string(const std::wstring &wstring) {
    return utf8_conv.to_bytes(wstring);
}

std::wstring string2wstring(const std::string &string) {
    return utf8_conv.from_bytes(string);
}

#pragma clang diagnostic pop

class WrappedCHelperCore {
private:
    CHelper::Core *core;
    std::wstring description;
    std::wstring structure;
    std::optional<std::pair<std::wstring, size_t>> newStr;
    std::optional<std::wstring> errorReason;

public:
    explicit WrappedCHelperCore(CHelper::Core *core)
        : core(core) {}

    ~WrappedCHelperCore() {
        delete core;
    }
    void onTextChanged(const wchar_t *content, size_t index) {
        core->onTextChanged(content, index);
    }

    void onSelectionChanged(size_t index0) {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return;
        }
        core->onSelectionChanged(index0);
    }

    const wchar_t *getStructure() {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return nullptr;
        }
        structure = core->getStructure();
        return structure.c_str();
    }

    const wchar_t *getDescription() {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return nullptr;
        }
        description = core->getDescription();
        return description.c_str();
    }

    void onSuggestionClick(size_t which) {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return;
        }
        newStr = core->onSuggestionClick(which);
    }

    const wchar_t *getStringAfterSuggestionClick() {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return nullptr;
        }
        if (newStr.has_value()) {
            return newStr.value().first.c_str();
        } else {
            return nullptr;
        }
    }

    size_t getSelectionAfterSuggestionClick() {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return 0;
        }
        if (newStr.has_value()) {
            return newStr.value().second;
        } else {
            return 0;
        }
    }

    const wchar_t *getErrorReason() {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return nullptr;
        }
        auto errorReasons = core->getErrorReasons();
        if (HEDLEY_UNLIKELY(errorReasons.empty())) {
            errorReason = std::nullopt;
        } else if (HEDLEY_UNLIKELY(errorReasons.size() == 1)) {
            errorReason = errorReasons[0]->errorReason;
        } else {
            errorReason = L"可能的错误原因：";
            int i = 0;
            for (const auto &item: errorReasons) {
                errorReason->append(L"\n").append(std::to_wstring(++i)).append(L". ").append(item->errorReason);
            }
        }
        if (errorReason == std::nullopt) {
            return nullptr;
        } else {
            return errorReason->c_str();
        }
    }

    size_t getSuggestionSize() {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return 0;
        }
        auto suggestions = core->getSuggestions();
        if (suggestions == nullptr) {
            return 0;
        }
        return suggestions->size();
    }

    const wchar_t *getSuggestionTitle(size_t which) {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return nullptr;
        }
        auto suggestions = core->getSuggestions();
        if (suggestions == nullptr) {
            return nullptr;
        }
        if (which >= suggestions->size()) {
            return nullptr;
        }
        return suggestions->at(which).content->name.c_str();
    }

    const wchar_t *getSuggestionDescription(size_t which) {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return nullptr;
        }
        auto suggestions = core->getSuggestions();
        if (suggestions == nullptr) {
            return nullptr;
        }
        if (which >= suggestions->size()) {
            return nullptr;
        }
        const std::optional<std::wstring> &suggestDescription = suggestions->at(which).content->description;
        if (suggestDescription == std::nullopt) {
            return nullptr;
        } else {
            return suggestDescription->c_str();
        }
    }
};

extern "C" {

EMSCRIPTEN_KEEPALIVE WrappedCHelperCore *init(const char *cpackPtr, size_t cpackLength) {
    return new WrappedCHelperCore(CHelper::Core::create([&cpackPtr, &cpackLength]() {
        std::string str = std::string(cpackPtr, cpackLength);
        std::istringstream iss(str);
        CHelper::BinaryReader binaryReader(true, iss);
        return CHelper::CPack::createByBinary(binaryReader);
    }));
}

EMSCRIPTEN_KEEPALIVE void release(const WrappedCHelperCore *core) {
    delete core;
}

EMSCRIPTEN_KEEPALIVE void onTextChanged(WrappedCHelperCore *core, const wchar_t *content, size_t index) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return;
    }
    core->onTextChanged(content, index);
}

EMSCRIPTEN_KEEPALIVE void onSelectionChanged(WrappedCHelperCore *core, size_t index) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return;
    }
    core->onSelectionChanged(index);
}

EMSCRIPTEN_KEEPALIVE const wchar_t *getStructure(WrappedCHelperCore *core) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return core->getStructure();
}

EMSCRIPTEN_KEEPALIVE const wchar_t *getDescription(WrappedCHelperCore *core) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return core->getDescription();
}

EMSCRIPTEN_KEEPALIVE const wchar_t *getErrorReason(WrappedCHelperCore *core) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return core->getErrorReason();
}

EMSCRIPTEN_KEEPALIVE size_t getSuggestionSize(WrappedCHelperCore *core) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return 0;
    }
    return core->getSuggestionSize();
}

EMSCRIPTEN_KEEPALIVE const wchar_t *getSuggestionTitle(WrappedCHelperCore *core, size_t which) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return core->getSuggestionTitle(which);
}

EMSCRIPTEN_KEEPALIVE const wchar_t *getSuggestionDescription(WrappedCHelperCore *core, size_t which) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return core->getSuggestionDescription(which);
}

EMSCRIPTEN_KEEPALIVE void onSuggestionClick(WrappedCHelperCore *core, size_t which) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return;
    }
    core->onSuggestionClick(which);
}

EMSCRIPTEN_KEEPALIVE const wchar_t *getStringAfterSuggestionClick(WrappedCHelperCore *core) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return core->getStringAfterSuggestionClick();
}

EMSCRIPTEN_KEEPALIVE size_t getSelectionAfterSuggestionClick(WrappedCHelperCore *core) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return 0;
    }
    return core->getSelectionAfterSuggestionClick();
}
}