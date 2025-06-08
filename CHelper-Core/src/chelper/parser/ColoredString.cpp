//
// Created by Yancey on 2024-08-07.
//

#include <chelper/parser/ColoredString.h>
#include <chelper/settings/Settings.h>

namespace CHelper {

    ColoredString::ColoredString(const std::u16string_view &str)
        : str(str),
          colors(str.length(), NO_COLOR) {}

    void ColoredString::setColor(size_t index, uint32_t color) {
#ifdef CHelperDebug
        if (colors[index] != NO_COLOR && str[index] != '[' && str[index] != ']' && str[index] != '{' && str[index] != '}') {
            SPDLOG_ERROR("replace color");
        }
        if (index > str.length()) {
            SPDLOG_ERROR("index out of range");
            return;
        }
#endif
        colors[index] = color;
    }

    void ColoredString::setColor(size_t start, size_t end, uint32_t color) {
#ifdef CHelperDebug
        for (size_t i = start; i < end; i++) {
            if (colors[i] != NO_COLOR && str[i] != '[' && str[i] != ']' && str[i] != '{' && str[i] != '}') {
                SPDLOG_ERROR("replace color");
            }
        }
        if (start > str.length() || end > str.length()) {
            SPDLOG_ERROR("index out of range");
            return;
        }
        if (start > end) {
            SPDLOG_ERROR("start should less than end");
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
