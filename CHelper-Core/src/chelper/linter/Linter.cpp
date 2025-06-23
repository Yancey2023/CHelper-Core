//
// Created by Yancey on 2025-06-20.
//

#include <chelper/linter/Linter.h>
#include <chelper/node/NodeType.h>

#define CHELPER_LINT(v1)                                                                                          \
    case Node::NodeTypeId::v1:                                                                                    \
        isDirty = Linter<typename Node::NodeTypeDetail<Node::NodeTypeId::v1>::Type>::lint(astNode, errorReasons); \
        break;

namespace CHelper::Linter {

    template<class NodeType>
    struct Linter {
        static bool lint(const ASTNode &astNode, std::vector<std::shared_ptr<ErrorReason>> &errorReasons) {
            return false;
        }
    };

    template<>
    struct Linter<Node::NodeJsonString> {
        static bool lint(const ASTNode &astNode, std::vector<std::shared_ptr<ErrorReason>> &errorReasons) {
            if (HEDLEY_UNLIKELY(astNode.id == ASTNodeId::NODE_STRING_INNER)) {
                auto convertResult = JsonUtil::jsonString2String(std::u16string(astNode.tokens.toString()));
                size_t offset = astNode.tokens.getStartIndex() + 1;
                for (const auto &item: getErrorsExceptParseError(astNode.childNodes[0])) {
                    item->start = convertResult.convert(item->start) + offset;
                    item->end = convertResult.convert(item->end) + offset;
                    errorReasons.push_back(item);
                }
            }
            return true;
        }
    };

    template<>
    struct Linter<Node::NodeCommandName> {
        static bool lint(const ASTNode &astNode, std::vector<std::shared_ptr<ErrorReason>> &errorReasons) {
            if (HEDLEY_UNLIKELY(astNode.isError())) {
                return true;
            }
            const auto &node = *reinterpret_cast<const Node::NodeCommandName *>(astNode.node.data);
            std::u16string_view str = astNode.tokens.toString();
            for (const auto &command: *node.commands) {
                for (const auto &name: command.name) {
                    if (HEDLEY_UNLIKELY(str == name)) {
                        return true;
                    }
                }
            }
            errorReasons.push_back(ErrorReason::idError(astNode.tokens, fmt::format(u"找不到命令名 -> {}", str)));
            return true;
        }
    };

    template<>
    struct Linter<Node::NodeNamespaceId> {
        static bool lint(const ASTNode &astNode, std::vector<std::shared_ptr<ErrorReason>> &errorReasons) {
            if (HEDLEY_UNLIKELY(astNode.isError())) {
                return true;
            }
            const auto &node = *reinterpret_cast<const Node::NodeNamespaceId *>(astNode.node.data);
            std::u16string_view str = astNode.tokens.toString();
            XXH64_hash_t strHash = XXH3_64bits(str.data(), str.size() * sizeof(decltype(str)::value_type));
            if (HEDLEY_UNLIKELY(std::all_of(node.customContents->begin(), node.customContents->end(), [&strHash](const auto &item) {
                    return !item->fastMatch(strHash) && !item->getIdWithNamespace()->fastMatch(strHash);
                }))) {
                errorReasons.push_back(ErrorReason::idError(astNode.tokens, fmt::format(u"找不到ID -> {}", str)));
            }
            return true;
        }
    };

    template<>
    struct Linter<Node::NodeNormalId> {
        static bool lint(const ASTNode &astNode, std::vector<std::shared_ptr<ErrorReason>> &errorReasons) {
            if (HEDLEY_UNLIKELY(astNode.isError())) {
                return true;
            }
            const auto &node = *reinterpret_cast<const Node::NodeNormalId *>(astNode.node.data);
            std::u16string_view str = astNode.tokens.toString();
            XXH64_hash_t strHash = XXH3_64bits(str.data(), str.size() * sizeof(decltype(str)::value_type));
            if (HEDLEY_UNLIKELY(std::all_of(node.customContents->begin(), node.customContents->end(), [&strHash](const auto &item) {
                    return !item->fastMatch(strHash);
                }))) {
                errorReasons.push_back(ErrorReason::idError(astNode.tokens, fmt::format(u"找不到ID -> {}", str)));
            }
            return true;
        }
    };

    template<>
    struct Linter<Node::NodePosition> {
        static bool lint(const ASTNode &astNode, std::vector<std::shared_ptr<ErrorReason>> &errorReasons) {
            if (HEDLEY_UNLIKELY(!astNode.isError() && astNode.id == ASTNodeId::NODE_POSITION_POSITIONS_WITH_ERROR)) {
                errorReasons.push_back(ErrorReason::logicError(astNode.tokens, u"绝对坐标和相对坐标不能与局部坐标混用"));
                return true;
            } else {
                return false;
            }
        }
    };

    template<>
    struct Linter<Node::NodeRelativeFloat> {
        static bool lint(const ASTNode &astNode, std::vector<std::shared_ptr<ErrorReason>> &errorReasons) {
            if (HEDLEY_UNLIKELY(!astNode.isError() && astNode.id == ASTNodeId::NODE_RELATIVE_FLOAT_WITH_ERROR)) {
                errorReasons.push_back(ErrorReason::logicError(astNode.tokens, u"不能使用局部坐标"));
                return true;
            } else {
                return false;
            }
        }
    };

    template<class T, bool isJson>
    struct Linter<Node::NodeTemplateNumber<T, isJson>> {
        static bool lint(const ASTNode &astNode, std::vector<std::shared_ptr<ErrorReason>> &errorReasons) {
            if (HEDLEY_UNLIKELY(astNode.isError())) {
                return true;
            }
            auto &node = *static_cast<const Node::NodeTemplateNumber<T, isJson> *>(astNode.node.data);
            std::string str = utf8::utf16to8(astNode.tokens.toString());
            char *end;
            auto value = Node::NodeTemplateNumber<T, isJson>::str2number(str, end);
            if (HEDLEY_UNLIKELY(end == str.c_str() || *end != '\0' ||
                                (std::numeric_limits<T>::has_infinity &&
                                 (value == std::numeric_limits<T>::infinity() ||
                                  value == -std::numeric_limits<T>::infinity())) ||
                                value < node.min.value_or(std::numeric_limits<T>::lowest()) ||
                                value > node.max.value_or(std::numeric_limits<T>::max()))) {
                errorReasons.push_back(ErrorReason::idError(
                        astNode.tokens,
                        fmt::format(
                                u"数值不在范围[{}, {}]内 -> {}",
                                node.min.value_or(std::numeric_limits<T>::lowest()),
                                node.max.value_or(std::numeric_limits<T>::max()),
                                astNode.tokens.toString())));
            }
            return true;
        }
    };

    void lint(const ASTNode &astNode, std::vector<std::shared_ptr<ErrorReason>> &errorReasons) {
        if (HEDLEY_UNLIKELY(!astNode.isAllSpaceError())) {
#ifdef CHelperTest
            Profile::push("collect id errors: {}", FORMAT_ARG(Node::getNodeTypeName(astNode.node.nodeTypeId)));
#endif
            bool isDirty = false;
            switch (astNode.node.nodeTypeId) {
                CODEC_PASTE(CHELPER_LINT, CHELPER_NODE_TYPES)
                default:
                    HEDLEY_UNREACHABLE();
            }
#ifdef CHelperTest
            Profile::pop();
#endif
            if (HEDLEY_UNLIKELY(isDirty)) {
                return;
            }
        }
        switch (astNode.mode) {
            case ASTNodeMode::NONE:
                break;
            case ASTNodeMode::AND:
                for (const ASTNode &item: astNode.childNodes) {
                    lint(item, errorReasons);
                }
                break;
            case ASTNodeMode::OR:
                lint(astNode.getBestNode(), errorReasons);
                break;
        }
    }

    std::vector<std::shared_ptr<ErrorReason>> sortByLevel(const std::vector<std::shared_ptr<ErrorReason>> &input) {
        std::vector<std::shared_ptr<ErrorReason>> output;
        uint8_t i = ErrorReasonLevel::maxLevel;
        while (true) {
            for (const auto &item: input) {
                if (HEDLEY_UNLIKELY(item->level == i)) {
                    output.push_back(item);
                }
            }
            if (HEDLEY_UNLIKELY(i == 0)) {
                break;
            }
            --i;
        }
        return output;
    }

    std::vector<std::shared_ptr<ErrorReason>> getErrorsExceptParseError(const ASTNode &astNode) {
        std::vector<std::shared_ptr<ErrorReason>> input;
#ifdef CHelperTest
        Profile::push("start get errors except parse error: {} {}", FORMAT_ARG(utf8::utf16to8(astNode.tokens.toString())), FORMAT_ARG(Node::getNodeTypeName(astNode.node.nodeTypeId)));
#endif
        lint(astNode, input);
#ifdef CHelperTest
        Profile::pop();
#endif
        return sortByLevel(input);
    }

    std::vector<std::shared_ptr<ErrorReason>> getErrorReasons(const ASTNode &astNode) {
        std::vector<std::shared_ptr<ErrorReason>> result = astNode.errorReasons;
#ifdef CHelperTest
        Profile::push("start getting error reasons: {} {}", FORMAT_ARG(utf8::utf16to8(astNode.tokens.toString())), FORMAT_ARG(Node::getNodeTypeName(astNode.node.nodeTypeId)));
#endif
        lint(astNode, result);
#ifdef CHelperTest
        Profile::pop();
#endif
        return sortByLevel(result);
    }

}// namespace CHelper::Linter
