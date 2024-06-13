//
// Created by Yancey on 2024/4/2.
//

#include "../chelper/Core.h"
#include "emscripten/emscripten.h"

std::shared_ptr<CHelper::Core> core;

int main(int argc, char **argv) {
    printf("Hello World\n");
}

#ifdef __cplusplus
extern "C" {
#endif

void EMSCRIPTEN_KEEPALIVE init(char *cpackPtr, size_t cpackLength) {
    core = CHelper::Core::create([&cpackPtr, &cpackLength]() -> std::unique_ptr<CHelper::CPack> {
        std::string str = std::string(cpackPtr, cpackLength);
        std::istringstream iss(str);
        CHelper::BinaryReader binaryReader(true, iss);
        return CHelper::CPack::createByBinary(binaryReader);
    });
}

void EMSCRIPTEN_KEEPALIVE onTextChanged(const std::string &content, size_t index) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return;
    }
    core->onTextChanged(content, index);
}

void EMSCRIPTEN_KEEPALIVE onSelectionChanged(size_t index) {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return;
    }
    core->onSelectionChanged(index);
}

const char *EMSCRIPTEN_KEEPALIVE getDescription() {
    if (HEDLEY_UNLIKELY(core == nullptr)) {
        return nullptr;
    }
    std::string description = core->getDescription();
    char *result = new char[description.size() + 1];
    strcpy_s(description.data(), description.size() + 1, result);
    return result;
}

void EMSCRIPTEN_KEEPALIVE release(const char *str) {
    delete[] str;
}

#ifdef __cplusplus
}
#endif
