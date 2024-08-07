//
// Created by Yancey on 2024/4/2.
//

#include "../chelper/Core.h"
#include <emscripten/emscripten.h>

extern "C" {

class WrappedCHelperCore {
private:
    CHelper::Core *core;
    std::string description;
    std::string structure;
    std::optional<std::string> newStr;
    std::optional<std::string> errorReason;

public:
    explicit WrappedCHelperCore(CHelper::Core *core) : core(core) {
    }

    ~WrappedCHelperCore() {
        delete core;
    }
    void onTextChanged(const std::string &content, size_t index) {
        core->onTextChanged(content, index);
    }

    void onSelectionChanged(size_t index0) {
        if (core == nullptr) {
            return;
        }
        core->onSelectionChanged(index0);
    }

    [[nodiscard]] const char *getStructure() {
        if (core == nullptr) {
            return nullptr;
        }
        structure = core->getStructure();
        return structure.c_str();
    }

    [[nodiscard]] const char *getDescription() {
        if (core == nullptr) {
            return nullptr;
        }
        description = core->getDescription();
        return description.c_str();
    }

    [[nodiscard]] const char *onSuggestionClick(size_t which) {
        if (core == nullptr) {
            return nullptr;
        }
        newStr = core->onSuggestionClick(which);
        if (newStr.has_value()) {
            return newStr->c_str();
        } else {
            return nullptr;
        }
    }

    [[nodiscard]] const char *getErrorReason() {
        if (core == nullptr) {
            return nullptr;
        }
        auto errorReasons = core->getErrorReasons();
        if (HEDLEY_UNLIKELY(errorReasons.empty())) {
            errorReason = std::nullopt;
        } else if (HEDLEY_UNLIKELY(errorReasons.size() == 1)) {
            errorReason = errorReasons[0]->errorReason;
        } else {
            errorReason = "可能的错误原因：";
            int i = 0;
            for (const auto &item: errorReasons) {
                errorReason->append("\n").append(std::to_string(++i)).append(". ").append(item->errorReason);
            }
        }
        if (errorReason == std::nullopt) {
            return nullptr;
        } else {
            return errorReason->c_str();
        }
    }

    [[nodiscard]] size_t getSuggestionSize() {
        if (core == nullptr) {
            return 0;
        }
        auto suggestions = core->getSuggestions();
        if (suggestions == nullptr) {
            return 0;
        }
        return suggestions->size();
    }

    [[nodiscard]] const char *getSuggestionTitle(size_t which) {
        if (core == nullptr) {
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

    [[nodiscard]] const char *getSuggestionDescription(size_t which) {
        if (core == nullptr) {
            return nullptr;
        }
        auto suggestions = core->getSuggestions();
        if (suggestions == nullptr) {
            return nullptr;
        }
        if (which >= suggestions->size()) {
            return nullptr;
        }
        const std::optional<std::string> &suggestDescription = suggestions->at(which).content->description;
        if (suggestDescription == std::nullopt) {
            return nullptr;
        } else {
            return suggestDescription->c_str();
        }
    }
};

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

EMSCRIPTEN_KEEPALIVE const char *onSuggestionClick(WrappedCHelperCore *core, size_t which) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return core->onSuggestionClick(which);
}
}