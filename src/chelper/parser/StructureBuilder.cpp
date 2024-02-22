//
// Created by Yancey666 on 2024/2/21.
//

#include "StructureBuilder.h"

namespace CHelper {

    bool StructureBuilder::isDirty() const {
        return dirty;
    }

    void StructureBuilder::appendUnknownIfNotDirty() {
        if (!dirty) {
            append(true, "未知结构");
        }
        dirty = false;
    }

    void StructureBuilder::appendSymbol(char ch) {
        structure.push_back(ch);
        dirty = true;
    }

    void StructureBuilder::append(const std::string &str) {
        structure.append(str);
        dirty = true;
    }

    void StructureBuilder::appendWhiteSpace() {
        appendSymbol(' ');
    }

    void StructureBuilder::appendLeftBracket(bool isMustHave) {
        appendSymbol(isMustHave ? '<' : '[');
    }

    void StructureBuilder::appendRightBracket(bool isMustHave) {
        appendSymbol(isMustHave ? '>' : ']');
    }

    void StructureBuilder::append(bool isMustHave, const std::string &str) {
        appendWhiteSpace();
        appendLeftBracket(isMustHave);
        append(str);
        appendRightBracket(isMustHave);
    }

    //使用std::move()节省性能，所以这个类不可以继续使用了，也没有必要继续使用
    std::string StructureBuilder::build() {
        return std::move(structure);
    }

}