//
// Created by Yancey on 2024/4/2.
//

#include "chelper/CHelperCore.h"
#include <emscripten/emscripten.h>

class WrappedCHelperCore {
private:
    CHelper::CHelperCore *core;
    std::u16string description;
    std::u16string structure;
    std::optional<std::pair<std::u16string, size_t>> newStr;
    std::optional<std::u16string> errorReason;

public:
    explicit WrappedCHelperCore(CHelper::CHelperCore *core)
        : core(core) {}

    ~WrappedCHelperCore() {
        delete core;
    }

    void onTextChanged(const char16_t *content, size_t index) {
        core->onTextChanged(content, index);
    }

    void onSelectionChanged(size_t index0) {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return;
        }
        core->onSelectionChanged(index0);
    }

    const char16_t *getStructure() {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return nullptr;
        }
        structure = core->getStructure();
        return structure.c_str();
    }

    const char16_t *getDescription() {
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

    const char16_t *getStringAfterSuggestionClick() {
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

    const char16_t *getErrorReason() {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return nullptr;
        }
        auto errorReasons = core->getErrorReasons();
        if (HEDLEY_UNLIKELY(errorReasons.empty())) {
            errorReason = std::nullopt;
        } else if (HEDLEY_UNLIKELY(errorReasons.size() == 1)) {
            errorReason = errorReasons[0]->errorReason;
        } else {
            errorReason = u"可能的错误原因：";
            int i = 0;
            for (const auto &item: errorReasons) {
                errorReason->append(u"\n").append(utf8::utf8to16(std::to_string(++i))).append(u". ").append(item->errorReason);
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

    const char16_t *getSuggestionTitle(size_t which) {
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

    const char16_t *getSuggestionDescription(size_t which) {
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
        const std::optional<std::u16string> &suggestDescription = suggestions->at(which).content->description;
        if (suggestDescription == std::nullopt) {
            return nullptr;
        } else {
            return suggestDescription->c_str();
        }
    }
};

extern "C" {

EMSCRIPTEN_KEEPALIVE WrappedCHelperCore *init(const char *cpackPtr, size_t cpackLength) {
    return new WrappedCHelperCore(CHelper::CHelperCore::create([&cpackPtr, &cpackLength]() {
        std::string str = std::string(cpackPtr, cpackLength);
        std::istringstream iss(str);
        CHelper::BinaryReader binaryReader(true, iss);
        return CHelper::CPack::createByBinary(binaryReader);
    }));
}

EMSCRIPTEN_KEEPALIVE void release(const WrappedCHelperCore *core) {
    delete core;
}

EMSCRIPTEN_KEEPALIVE void onTextChanged(WrappedCHelperCore *core, const char16_t *content, size_t index) {
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

EMSCRIPTEN_KEEPALIVE const char16_t *getStructure(WrappedCHelperCore *core) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return core->getStructure();
}

EMSCRIPTEN_KEEPALIVE const char16_t *getDescription(WrappedCHelperCore *core) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return core->getDescription();
}

EMSCRIPTEN_KEEPALIVE const char16_t *getErrorReason(WrappedCHelperCore *core) {
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

EMSCRIPTEN_KEEPALIVE const char16_t *getSuggestionTitle(WrappedCHelperCore *core, size_t which) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return core->getSuggestionTitle(which);
}

EMSCRIPTEN_KEEPALIVE const char16_t *getSuggestionDescription(WrappedCHelperCore *core, size_t which) {
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

EMSCRIPTEN_KEEPALIVE const char16_t *getStringAfterSuggestionClick(WrappedCHelperCore *core) {
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