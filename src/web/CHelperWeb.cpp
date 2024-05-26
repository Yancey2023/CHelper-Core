//
// Created by Yancey on 2024/4/2.
//

#include "CHelperWeb.h"
#include "../chelper/Core.h"

std::shared_ptr<CHelper::Core> core;

extern "C" [[maybe_unused]] void init(const std::string &cpack) {
    core = CHelper::Core::create([&cpack]() {
        return CHelper::CPack::createByJson(nlohmann::json::parse(cpack));
    });
}

extern "C" [[maybe_unused]] void onTextChanged(const std::string &content, size_t index) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return;
    }
    core->onTextChanged(content, index);
}

extern "C" [[maybe_unused]] void onSelectionChanged(size_t index) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return;
    }
    core->onSelectionChanged(index);
}

extern "C" [[maybe_unused]] std::string getDescription() {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return {};
    }
    return core->getDescription();
}