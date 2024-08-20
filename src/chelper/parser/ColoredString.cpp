//
// Created by Yancey on 2024-08-07.
//

#include "ColoredString.h"
#include "../settings/Settings.h"

namespace CHelper {

    ColoredString::ColoredString(const std::u16string_view &str)
        : str(str),
          colors(str.length(), NO_COLOR) {}

    void ColoredString::setColor(size_t index, uint32_t color) {
#if CHelperDebug == true
        if (colors[index] != NO_COLOR && str[index] != '[' && str[index] != ']' && str[index] != '{' && str[index] != '}') {
            CHELPER_ERROR("replace color");
        }
        if (index > str.length()) {
            CHELPER_ERROR("index out of range");
            return;
        }
#endif
        colors[index] = color;
    }

    void ColoredString::setColor(size_t start, size_t end, uint32_t color) {
#if CHelperDebug == true
        for (size_t i = start; i < end; i++) {
            if (colors[i] != NO_COLOR && str[i] != '[' && str[i] != ']' && str[i] != '{' && str[i] != '}') {
                CHELPER_ERROR("replace color");
            }
        }
        if (start > str.length() || end > str.length()) {
            CHELPER_ERROR("index out of range");
            return;
        }
        if (start > end) {
            CHELPER_ERROR("start should less than end");
            return;
        }
#endif
        std::fill(colors.begin() + static_cast<std::u16string::difference_type>(start),
                  colors.begin() + static_cast<std::u16string::difference_type>(end),
                  color);
    }
    void ColoredString::setColor(const TokensView &tokensView, uint32_t color) {
        setColor(tokensView.startIndex, tokensView.endIndex, color);
    }

}// namespace CHelper
