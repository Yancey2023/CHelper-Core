//
// Created by Yancey on 2024-08-20.
//

#ifndef CHELPER_NODENUMBER_H
#define CHELPER_NODENUMBER_H

#include "../NodeBase.h"

namespace CHelper::Node {

    template<class T>
    class NodeNumber : public NodeBase {
    public:
        std::optional<T> min, max;

        NodeNumber() = default;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override {
            if constexpr (std::is_same<T, float>() || std::is_same<T, double>()) {
                return tokenReader.readFloatASTNode(this);
            } else {
                return tokenReader.readIntegerASTNode(this);
            }
        }

    private:
        static auto str2number(const std::string &str, char *&end) {
            if constexpr (std::numeric_limits<T>::is_integer) {
                return std::strtoimax(str.c_str(), &end, 10);
            } else if constexpr (std::is_same<T, float>()) {
                return std::strtof(str.c_str(), &end);
            } else if constexpr (std::is_same<T, double>()) {
                return std::strtod(str.c_str(), &end);
            } else if constexpr (std::is_same<T, long double>()) {
                return std::strtold(str.c_str(), &end);
            }
        }

    public:
        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override {
            if (HEDLEY_UNLIKELY(astNode->isError())) {
                return true;
            }
            std::string str = utf8::utf16to8(astNode->tokens.toString());
            char *end;
            auto value = str2number(str, end);
            if (HEDLEY_UNLIKELY(end == str.c_str() || *end != '\0' ||
                                (std::numeric_limits<T>::has_infinity &&
                                 (value == std::numeric_limits<T>::infinity() ||
                                  value == -std::numeric_limits<T>::infinity())) ||
                                value < min.value_or(std::numeric_limits<T>::lowest()) ||
                                value > max.value_or(std::numeric_limits<T>::max()))) {
                idErrorReasons.push_back(
                        ErrorReason::idError(astNode->tokens,
                                             std::u16string(u"数值不在范围")
                                                     .append(u"[")
                                                     .append(utf8::utf8to16(std::to_string(min.value_or(std::numeric_limits<T>::lowest()))))
                                                     .append(u", ")
                                                     .append(utf8::utf8to16(std::to_string(max.value_or(std::numeric_limits<T>::max()))))
                                                     .append(u"]")
                                                     .append(u"内 -> ")
                                                     .append(astNode->tokens.toString())));
            }
            return true;
        }

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override {
            structure.append(isMustHave, description.value_or(u"数字"));
        }

        bool collectColor(const ASTNode *astNode,
                          ColoredString &coloredString,
                          const Theme &theme) const override {
            if constexpr (std::numeric_limits<T>::is_integer) {
                coloredString.setColor(astNode->tokens, theme.colorInteger);
            } else {
                coloredString.setColor(astNode->tokens, theme.colorFloat);
            }
            return true;
        }
    };

}// namespace CHelper::Node

#endif//CHELPER_NODENUMBER_H
