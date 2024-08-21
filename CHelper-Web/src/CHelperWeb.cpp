//
// Created by Yancey on 2024/4/2.
//

#include "chelper/CHelperCore.h"
#include <emscripten/emscripten.h>
#include <emscripten/val.h>

class WrappedCHelperCore {
private:
    CHelper::CHelperCore *core;
    std::string description;
    std::string structure;
    std::optional<std::pair<std::string, size_t>> newStr;
    std::optional<std::string> errorReason;
    std::string tempStr;

public:
    explicit WrappedCHelperCore(CHelper::CHelperCore *core)
        : core(core) {}

    ~WrappedCHelperCore() {
        delete core;
    }

    void onTextChanged(const char *content, size_t index) {
        core->onTextChanged(utf8::utf8to16(std::string(content)), index);
    }

    void onSelectionChanged(size_t index0) {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return;
        }
        core->onSelectionChanged(index0);
    }

    const char *getStructure() {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return nullptr;
        }
        structure = utf8::utf16to8(core->getStructure());
        return structure.c_str();
    }

    const char *getDescription() {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return nullptr;
        }
        description = utf8::utf16to8(core->getDescription());
        return description.c_str();
    }

    void onSuggestionClick(size_t which) {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return;
        }
        std::optional<std::pair<std::u16string, size_t>> result = core->onSuggestionClick(which);
        if (result.has_value()) {
            newStr = {utf8::utf16to8(result.value().first), result.value().second};
        } else {
            newStr = std::nullopt;
        }
    }

    const char *getStringAfterSuggestionClick() {
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

    const char *getErrorReason() {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return nullptr;
        }
        auto errorReasons = core->getErrorReasons();
        if (HEDLEY_UNLIKELY(errorReasons.empty())) {
            errorReason = std::nullopt;
        } else if (HEDLEY_UNLIKELY(errorReasons.size() == 1)) {
            errorReason = utf8::utf16to8(errorReasons[0]->errorReason);
        } else {
            errorReason = "可能的错误原因：";
            int i = 0;
            for (const auto &item: errorReasons) {
                errorReason->append("\n").append(std::to_string(++i)).append(". ").append(utf8::utf16to8(item->errorReason));
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

    const char *getSuggestionTitle(size_t which) {
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
        tempStr = utf8::utf16to8(suggestions->at(which).content->name);
        return tempStr.c_str();
    }

    const char *getSuggestionDescription(size_t which) {
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
            tempStr = utf8::utf16to8(suggestDescription.value());
            return tempStr.c_str();
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

EMSCRIPTEN_KEEPALIVE void onTextChanged(WrappedCHelperCore *core, const char *content, size_t index) {
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

EMSCRIPTEN_KEEPALIVE const char *getStructure(WrappedCHelperCore *core) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return core->getStructure();
}

EMSCRIPTEN_KEEPALIVE const char *getDescription(WrappedCHelperCore *core) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return core->getDescription();
}

EMSCRIPTEN_KEEPALIVE const char *getErrorReason(WrappedCHelperCore *core) {
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

EMSCRIPTEN_KEEPALIVE const char *getSuggestionTitle(WrappedCHelperCore *core, size_t which) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return core->getSuggestionTitle(which);
}

EMSCRIPTEN_KEEPALIVE const char *getSuggestionDescription(WrappedCHelperCore *core, size_t which) {
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

EMSCRIPTEN_KEEPALIVE const char *getStringAfterSuggestionClick(WrappedCHelperCore *core) {
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