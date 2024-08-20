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

        [[nodiscard]] NodeType *getNodeType() const override {
            return NodeType::JSON_INTEGER.get();
        }

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override {
            if constexpr (std::is_same<T, int32_t>()) {
                return tokenReader.readIntegerASTNode(this);
            } else {
                return tokenReader.readFloatASTNode(this);
            }
        }

        bool collectIdError(const ASTNode *astNode,
                            std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const override {
            if (HEDLEY_UNLIKELY(astNode->isError())) {
                return true;
            }
            std::string str = utf8::utf16to8(astNode->tokens.toString());
            bool isInRange;
            if constexpr (std::is_same<T, int32_t>()) {
                char *end;
                std::intmax_t value = std::strtoimax(str.c_str(), &end, 10);
                isInRange = end == str.c_str() || *end != '\0' ||
                            value < min.value_or(std::numeric_limits<int32_t>::lowest()) ||
                            value > max.value_or(std::numeric_limits<int32_t>::max());
            } else {
                char *end;
                float value = std::strtof(str.c_str(), &end);
                isInRange = end == str.c_str() || *end != '\0' ||
                            value == HUGE_VALF || value == -HUGE_VALF ||
                            value < min.value_or(std::numeric_limits<float>::lowest()) ||
                            value > max.value_or(std::numeric_limits<float>::max());
            }
            if (!HEDLEY_UNLIKELY(isInRange)) {
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

        bool collectColor(const ASTNode *astNode,
                          ColoredString &coloredString,
                          const Theme &theme) const override {
            if constexpr (std::is_same<T, int32_t>()) {
                coloredString.setColor(astNode->tokens, theme.colorInteger);
            } else {
                coloredString.setColor(astNode->tokens, theme.colorFloat);
            }
            return true;
        }
    };

}// namespace CHelper::Node

#endif//CHELPER_NODENUMBER_H
