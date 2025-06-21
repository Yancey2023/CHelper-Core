//
// Created by Yancey on 2025-6-19.
//

#include <chelper/node/NodeType.h>
#include <chelper/parameter_hint/ParameterHint.h>

#define CHELPER_GET_PARAMETER_HINT(v1)                                                                              \
    case Node::NodeTypeId::v1:                                                                                      \
        parameterHint = ParameterHint<typename Node::NodeTypeDetail<Node::NodeTypeId::v1>::Type>::getHint(astNode); \
        break;

namespace CHelper::ParameterHint {

    template<class NodeType, class = void>
    struct ParameterHint {
        static_assert(std::is_base_of_v<Node::NodeBase, NodeType>, "NodeType must be derived from NodeBase");
        static std::optional<std::u16string> getHint(const ASTNode &astNode) {
            return std::nullopt;
        }
    };

    template<class NodeType>
    struct ParameterHint<NodeType, std::enable_if_t<std::is_base_of_v<Node::NodeSerializable, NodeType>>> {
        static std::optional<std::u16string> getHint(const ASTNode &astNode) {
            return reinterpret_cast<const NodeType &>(*astNode.node).description;
        }
    };

    template<>
    struct ParameterHint<Node::NodeBlock> {
        static std::optional<std::u16string> getHint(const ASTNode &astNode) {
            if (astNode.id == ASTNodeId::NODE_BLOCK_BLOCK_ID) {
                return u"方块ID";
            } else if (astNode.id == ASTNodeId::NODE_BLOCK_BLOCK_STATE) {
                return u"方块状态";
            } else {
                return std::nullopt;
            }
        }
    };

    template<>
    struct ParameterHint<Node::NodeCommand> {
        static std::optional<std::u16string> getHint(const ASTNode &astNode) {
            if (astNode.id == ASTNodeId::NODE_COMMAND_COMMAND_NAME) {
                return u"命令的名字";
            } else {
                return std::nullopt;
            }
        }
    };

    template<>
    struct ParameterHint<Node::NodeItem> {
        static std::optional<std::u16string> getHint(const ASTNode &astNode) {
            return std::nullopt;
        }
    };

    template<>
    struct ParameterHint<Node::NodeRepeat> {
        static std::optional<std::u16string> getHint(const ASTNode &astNode) {
            if (HEDLEY_UNLIKELY(astNode.tokens.isEmpty())) {
                return reinterpret_cast<const Node::NodeRepeat &>(*astNode.node).description;
            } else {
                return std::nullopt;
            }
        }
    };

    std::optional<std::u16string> getParameterHint(const ASTNode &astNode, size_t index) {
        if (HEDLEY_UNLIKELY(index < astNode.tokens.getStartIndex() || index > astNode.tokens.getEndIndex())) {
            return std::nullopt;
        }
        if (HEDLEY_UNLIKELY(astNode.id != ASTNodeId::COMPOUND && astNode.id != ASTNodeId::NEXT_NODE && !astNode.isAllSpaceError())) {
#ifdef CHelperTest
            Profile::push("get parameter hint: {}", FORMAT_ARG(Node::NodeTypeHelper::getName(astNode.node->nodeTypeId)));
#endif
            std::optional<std::u16string> parameterHint;
            switch (astNode.node->nodeTypeId) {
                CODEC_PASTE(CHELPER_GET_PARAMETER_HINT, CHELPER_NODE_TYPES)
                default:
                    HEDLEY_UNREACHABLE();
            }
#ifdef CHelperTest
            Profile::pop();
#endif
            if (parameterHint.has_value()) {
                return parameterHint;
            }
        }
        switch (astNode.mode) {
            case ASTNodeMode::NONE:
                return std::nullopt;
            case ASTNodeMode::AND:
                for (const ASTNode &item: astNode.childNodes) {
                    auto parameterHint = getParameterHint(item, index);
                    if (HEDLEY_UNLIKELY(parameterHint.has_value())) {
                        return parameterHint;
                    }
                }
                return std::nullopt;
            case ASTNodeMode::OR:
                return getParameterHint(astNode.getBestNode(), index);
            default:
                HEDLEY_UNREACHABLE();
        }
    }

}// namespace CHelper::ParameterHint
