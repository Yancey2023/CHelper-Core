//
// Created by Yancey on 2024-08-06.
//

#ifndef CHELPER_SETTINGS_H
#define CHELPER_SETTINGS_H

#include <cinttypes>

namespace CHelper {

    extern uint32_t NO_COLOR;

    class Theme {
    public:
        uint32_t colorBoolean = NO_COLOR;
        uint32_t colorFloat = NO_COLOR;
        uint32_t colorInteger = NO_COLOR;
        uint32_t colorSymbol = NO_COLOR;
        uint32_t colorId = NO_COLOR;
        uint32_t colorTargetSelector = NO_COLOR;
        uint32_t colorCommand = NO_COLOR;
        uint32_t colorBrackets1 = NO_COLOR;
        uint32_t colorBrackets2 = NO_COLOR;
        uint32_t colorBrackets3 = NO_COLOR;
        uint32_t colorString = NO_COLOR;
        uint32_t colorNull = NO_COLOR;
        uint32_t colorRange = NO_COLOR;
        uint32_t colorLiteral = NO_COLOR;

        Theme();
    };

    class Settings {
    public:
        Theme theme;

        Settings() = default;
    };


}// namespace CHelper

#endif//CHELPER_SETTINGS_H
