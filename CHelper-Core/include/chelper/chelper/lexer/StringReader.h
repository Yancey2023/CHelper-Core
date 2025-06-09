//
// Created by Yancey on 2023/11/6.
//

#pragma once

#ifndef CHELPER_STRINGREADER_H
#define CHELPER_STRINGREADER_H

#include "LexerPos.h"
#include "pch.h"

namespace CHelper {

    class StringReader {
    public:
        const std::u16string &content;
        LexerPos pos, posBackup;

        explicit StringReader(const std::u16string &content);

        [[nodiscard]] bool ready() const;

        [[nodiscard]] std::optional<char16_t> read();

        bool skip();

        [[nodiscard]] std::optional<char16_t> next();

        [[nodiscard]] std::optional<char16_t> peek() const;

        void mark();

        void reset();

        [[nodiscard]] std::u16string_view collect() const;
    };

}// namespace CHelper

#endif//CHELPER_STRINGREADER_H
