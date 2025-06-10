//
// Created by Yancey on 2024/2/21.
//

#include <chelper/parser/StructureBuilder.h>

namespace CHelper {

    StructureBuilder &StructureBuilder::appendUnknown(bool isMustHave) {
        return append(isMustHave, u"未知");
    }

    StructureBuilder &StructureBuilder::appendSymbol(char16_t ch) {
        structure.push_back(ch);
        isDirty = true;
        return *this;
    }

    StructureBuilder &StructureBuilder::append(const std::u16string &str) {
        structure.append(str);
        isDirty = true;
        return *this;
    }

    StructureBuilder &StructureBuilder::appendSpace() {
        if (HEDLEY_UNLIKELY(structure.empty())) {
            isDirty = true;
            return *this;
        }
        return appendSymbol(' ');
    }

    StructureBuilder &StructureBuilder::appendLeftBracket(bool isMustHave) {
        return appendSymbol(isMustHave ? u'<' : u'[');
    }

    StructureBuilder &StructureBuilder::appendRightBracket(bool isMustHave) {
        return appendSymbol(isMustHave ? u'>' : u']');
    }

    StructureBuilder &StructureBuilder::append(bool isMustHave, const std::u16string &str) {
        return appendSpace().appendLeftBracket(isMustHave).append(str).appendRightBracket(isMustHave);
    }

    std::u16string StructureBuilder::build() {
        return std::move(structure);
    }

}// namespace CHelper