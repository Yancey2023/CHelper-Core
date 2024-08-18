//
// Created by Yancey on 2024/2/21.
//

#pragma once

#ifndef CHELPER_STRUCTUREBUILDER_H
#define CHELPER_STRUCTUREBUILDER_H

#include "pch.h"

namespace CHelper {

    class StructureBuilder {
    private:
        std::wstring structure;

    public:
        bool isDirty = false;

        StructureBuilder &appendUnknown(bool isMustHave);

        StructureBuilder &appendSymbol(wchar_t ch);

        StructureBuilder &append(const std::wstring &str);

        StructureBuilder &appendWhiteSpace();

        StructureBuilder &appendLeftBracket(bool isMustHave);

        StructureBuilder &appendRightBracket(bool isMustHave);

        StructureBuilder &append(bool isMustHave, const std::wstring &str);

        std::wstring build();
    };

}// namespace CHelper

#endif//CHELPER_STRUCTUREBUILDER_H
