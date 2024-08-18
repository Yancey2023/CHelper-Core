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
        LexerPos pos, posBackup;
        const std::wstring &content;

        explicit StringReader(const std::wstring &content);

        [[nodiscard]] bool ready() const;

        [[nodiscard]] std::optional<wchar_t> read();

        bool skip();

        [[nodiscard]] std::optional<wchar_t> next();

        [[nodiscard]] std::optional<wchar_t> peek() const;

        void mark();

        void reset();

        [[nodiscard]] std::wstring_view collect() const;
    };

}// namespace CHelper

#endif//CHELPER_STRINGREADER_H
