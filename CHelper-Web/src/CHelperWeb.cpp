//
// Created by Yancey on 2024/4/2.
//

#include <chelper/CHelperCore.h>
#include <emscripten/emscripten.h>
#include <emscripten/val.h>

class WrappedCHelperCore {
private:
    CHelper::CHelperCore *core;
    std::optional<std::pair<std::u16string, size_t>> newStr;
    std::optional<std::u16string> errorReason;
    std::u16string tempStr;

public:
    explicit WrappedCHelperCore(CHelper::CHelperCore *core)
        : core(core) {}

    ~WrappedCHelperCore() {
        delete core;
    }

    void onTextChanged(const char16_t *content, size_t index) const {
        core->onTextChanged(content, index);
    }

    void onSelectionChanged(size_t index0) const {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return;
        }
        core->onSelectionChanged(index0);
    }

    [[nodiscard]] const char16_t *getStructure() {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return nullptr;
        }
        tempStr = core->getStructure();
        return tempStr.c_str();
    }

    [[nodiscard]] const char16_t *getDescription() {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return nullptr;
        }
        tempStr = core->getDescription();
        return tempStr.c_str();
    }

    void onSuggestionClick(size_t which) {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return;
        }
        newStr = core->onSuggestionClick(which);
    }

    [[nodiscard]] const char16_t *getStringAfterSuggestionClick() const {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return nullptr;
        }
        if (newStr.has_value()) {
            return newStr.value().first.c_str();
        } else {
            return nullptr;
        }
    }

    [[nodiscard]] size_t getSelectionAfterSuggestionClick() const {
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
                errorReason->append(u"\n").append(utf8::utf8to16(std::to_string(++i)).append(u". ").append(item->errorReason));
            }
        }
        if (errorReason.has_value()) {
            return errorReason->c_str();
        } else {
            return nullptr;
        }
    }

    [[nodiscard]] size_t getSuggestionSize() const {
        if (HEDLEY_UNLIKELY(core == nullptr)) {
            return 0;
        }
        auto suggestions = core->getSuggestions();
        if (suggestions == nullptr) {
            return 0;
        }
        return suggestions->size();
    }

    [[nodiscard]] const char16_t *getSuggestionTitle(size_t which) const {
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
        return (*suggestions)[which].content->name.c_str();
    }

    [[nodiscard]] const char16_t *getSuggestionDescription(size_t which) const {
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
        const std::optional<std::u16string> &suggestDescription = (*suggestions)[which].content->description;
        if (suggestDescription.has_value()) {
            return suggestDescription.value().c_str();
        } else {
            return nullptr;
        }
    }
};

extern "C" {

EMSCRIPTEN_KEEPALIVE WrappedCHelperCore *init(const char *cpackPtr, size_t cpackLength) {
    return new WrappedCHelperCore(CHelper::CHelperCore::create([&cpackPtr, &cpackLength]() -> std::unique_ptr<CHelper::CPack> {
        std::string str = std::string(cpackPtr, cpackLength);
        std::istringstream iss(str);
        return CHelper::CPack::createByBinary(iss);
    }));
}

EMSCRIPTEN_KEEPALIVE void release(const WrappedCHelperCore *core) {
    delete core;
}

EMSCRIPTEN_KEEPALIVE void onTextChanged(const WrappedCHelperCore *core, const char16_t *content, size_t index) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return;
    }
    core->onTextChanged(content, index);
}

EMSCRIPTEN_KEEPALIVE void onSelectionChanged(const WrappedCHelperCore *core, size_t index) {
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

EMSCRIPTEN_KEEPALIVE size_t getSuggestionSize(const WrappedCHelperCore *core) {
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

EMSCRIPTEN_KEEPALIVE const char16_t *getStringAfterSuggestionClick(const WrappedCHelperCore *core) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    return core->getStringAfterSuggestionClick();
}

EMSCRIPTEN_KEEPALIVE size_t getSelectionAfterSuggestionClick(const WrappedCHelperCore *core) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return 0;
    }
    return core->getSelectionAfterSuggestionClick();
}
}