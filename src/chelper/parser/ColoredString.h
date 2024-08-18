//
// Created by Yancey on 2024-08-07.
//

#ifndef CHELPER_COLOREDSTRING_H
#define CHELPER_COLOREDSTRING_H

#include "TokensView.h"

namespace CHelper {

    class ColoredString {
    public:
        std::wstring_view str;
        std::vector<uint32_t> colors;

        explicit ColoredString(const std::wstring_view &str);

        void setColor(size_t index, uint32_t color);

        void setColor(size_t start, size_t end, uint32_t color);

        void setColor(const TokensView &tokensView, uint32_t color);
    };

}// namespace CHelper

#endif//CHELPER_COLOREDSTRING_H
