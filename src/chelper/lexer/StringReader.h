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
        const std::string &content;

        explicit StringReader(const std::string &content);

        [[nodiscard]] bool ready() const;

        [[nodiscard]] signed char read();

        bool skip();

        [[nodiscard]] signed char next();

        [[nodiscard]] signed char peek() const;

        void mark();

        void reset();

        [[nodiscard]] std::string_view collect() const;
    };

}// namespace CHelper

#endif//CHELPER_STRINGREADER_H
