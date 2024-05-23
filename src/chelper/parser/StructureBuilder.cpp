//
// Created by Yancey on 2024/2/21.
//

#include "StructureBuilder.h"

namespace CHelper {

    StructureBuilder &StructureBuilder::appendUnknown(bool isMustHave) {
        return append(isMustHave, "未知");
    }

    StructureBuilder &StructureBuilder::appendSymbol(char ch) {
        structure.push_back(ch);
        isDirty = true;
        return *this;
    }

    StructureBuilder &StructureBuilder::append(const std::string &str) {
        structure.append(str);
        isDirty = true;
        return *this;
    }

    StructureBuilder &StructureBuilder::appendWhiteSpace() {
        if (HEDLEY_UNLIKELY(structure.empty())) {
            isDirty = true;
            return *this;
        }
        return appendSymbol(' ');
    }

    StructureBuilder &StructureBuilder::appendLeftBracket(bool isMustHave) {
        return appendSymbol(isMustHave ? '<' : '[');
    }

    StructureBuilder &StructureBuilder::appendRightBracket(bool isMustHave) {
        return appendSymbol(isMustHave ? '>' : ']');
    }

    StructureBuilder &StructureBuilder::append(bool isMustHave, const std::string &str) {
        return appendWhiteSpace().appendLeftBracket(isMustHave).append(str).appendRightBracket(isMustHave);
    }

    //使用std::move()节省性能，所以这个类不可以继续使用了，也没有必要继续使用
    std::string StructureBuilder::build() {
        return std::move(structure);
    }

}// namespace CHelper