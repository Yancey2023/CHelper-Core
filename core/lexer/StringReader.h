//
// Created by Yancey666 on 2023/11/6.
//

#ifndef CHELPER_STRINGREADER_H
#define CHELPER_STRINGREADER_H

#include "pch.h"
#include "Pos.h"

namespace CHelper {

    class StringReader {
    public:
        CHelper::Pos pos, posBackup;
        std::string content;

        StringReader(std::string content, std::string filename);

        [[nodiscard]] bool ready() const;

        [[nodiscard]] char read();

        bool skip();

        [[nodiscard]] char next();

        [[nodiscard]] char peek();

        void mark();

        void reset();

        [[nodiscard]] std::string collect() const;
    };

}

#endif //CHELPER_STRINGREADER_H
