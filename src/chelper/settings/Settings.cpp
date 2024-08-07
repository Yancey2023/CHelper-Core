//
// Created by Yancey on 2024-08-06.
//

#include "Settings.h"

namespace CHelper {

    uint32_t NO_COLOR = 0x00000000;

    Theme::Theme() {
        const uint32_t COLOR_PURPLE = 0xFFc586c0;
        const uint32_t COLOR_ORANGE = 0xFFce9178;
        const uint32_t COLOR_LIGHT_BLUE = 0xFF9cdcfe;
        const uint32_t COLOR_BLUE = 0xFF179fff;
        const uint32_t COLOR_LIGHT_GREEN = 0xFFb5cea8;
        const uint32_t COLOR_GREEN = 0xFF4ec9b0;
        const uint32_t COLOR_LIGHT_YELLOW = 0xFFdcdcaa;
        const uint32_t COLOR_YELLOW = 0xFFffd700;

        colorBoolean = COLOR_LIGHT_GREEN;
        colorFloat = COLOR_LIGHT_GREEN;
        colorInteger = COLOR_LIGHT_GREEN;
        colorSymbol = COLOR_LIGHT_GREEN;
        colorId = COLOR_LIGHT_YELLOW;
        colorTargetSelector = COLOR_GREEN;
        colorCommand = COLOR_PURPLE;
        colorBrackets1 = COLOR_YELLOW;
        colorBrackets2 = COLOR_PURPLE;
        colorBrackets3 = COLOR_BLUE;
        colorString = COLOR_ORANGE;
        colorNull = COLOR_LIGHT_BLUE;
        colorRange = COLOR_LIGHT_BLUE;
        colorLiteral = COLOR_LIGHT_BLUE;
    }

}// namespace CHelper