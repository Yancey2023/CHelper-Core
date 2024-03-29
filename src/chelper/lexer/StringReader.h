//
// Created by Yancey on 2023/11/6.
//

#pragma once

#ifndef CHELPER_STRINGREADER_H
#define CHELPER_STRINGREADER_H

#include "pch.h"
#include "LexerPos.h"

namespace CHelper {

    class StringReader {
    public:
        CHelper::LexerPos pos, posBackup;
        const std::string &content;

        StringReader(const std::string &content,
                     const std::string &filePath);

        [[nodiscard]] bool ready() const;

        [[nodiscard]] signed char read();

        bool skip();

        [[nodiscard]] signed char next();

        [[nodiscard]] signed char peek() const;

        void mark();

        void reset();

        [[nodiscard]] std::string collect() const;
    };

}

#endif //CHELPER_STRINGREADER_H
