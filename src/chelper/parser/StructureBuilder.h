//
// Created by Yancey666 on 2024/2/21.
//

#ifndef CHELPER_STRUCTUREBUILDER_H
#define CHELPER_STRUCTUREBUILDER_H

#include "pch.h"

namespace CHelper {

    class StructureBuilder {
    private:
        std::string structure;
        bool dirty = false;

    public:
        bool isDirty() const;

        void appendUnknownIfNotDirty();

        void appendSymbol(char ch);

        void append(const std::string &str);

        void appendWhiteSpace();

        void appendLeftBracket(bool isMustHave);

        void appendRightBracket(bool isMustHave);

        void append(bool isMustHave, const std::string &str);

        std::string build();

    };

} // CHelper

#endif //CHELPER_STRUCTUREBUILDER_H
