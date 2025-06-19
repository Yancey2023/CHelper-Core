//
// Created by Yancey on 2025-6-19.
//

#include <chelper/parameter_hint/ParameterHint.h>

#define CHELPER_CODEC_GET_PARAMETER_HINT(v1)                                                                                 \
    case Node::NodeTypeId::v1:                                                                                               \
        parameterHint = ParameterHint<typename Node::details::NodeTypeDetail<Node::NodeTypeId::v1>::Type>::getHint(astNode); \
        break;

namespace CHelper::ParameterHint {

    template<class NodeType = Node::NodeBase>
    struct ParameterHint {

        static_assert(std::is_base_of_v<Node::NodeBase, NodeType>, "NodeType must be derived from Node::NodeBase");

        static std::optional<std::u16string> getHint(const ASTNode &astNode) {
            const auto &node = reinterpret_cast<const NodeType &>(*astNode.node);
            if constexpr (std::is_same_v<Node::NodeBlock, NodeType>) {
                if (astNode.id == ASTNodeId::NODE_BLOCK_BLOCK_ID) {
                    return u"方块ID";
                } else if (astNode.id == ASTNodeId::NODE_BLOCK_BLOCK_STATE) {
                    return u"方块状态";
                } else {
                    return std::nullopt;
                }
            } else if constexpr (std::is_same_v<Node::NodeCommand, NodeType>) {
                if (astNode.id == ASTNodeId::NODE_COMMAND_COMMAND_NAME) {
                    return u"命令的名字";
                } else {
                    return std::nullopt;
                }
            } else if constexpr (std::is_same_v<Node::NodeItem, NodeType>) {
                return std::nullopt;
            } else if constexpr (std::is_same_v<Node::NodeRepeat, NodeType>) {
                if (HEDLEY_UNLIKELY(astNode.tokens.isEmpty())) {
                    return node.description;
                } else {
                    return std::nullopt;
                }
            } else if constexpr (std::is_base_of_v<Node::NodeSerializable, NodeType>) {
                return node.brief.has_value() ? node.brief : node.description;
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
            std::optional<std::u16string> parameterHint;
            switch (astNode.node->getNodeType()) {
                CODEC_PASTE(CHELPER_CODEC_GET_PARAMETER_HINT, CHELPER_NODE_TYPES)
                default:
                    HEDLEY_UNREACHABLE();
            }
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
                return getParameterHint(astNode.childNodes[astNode.whichBest], index);
            default:
                HEDLEY_UNREACHABLE();
        }
    }

}// namespace CHelper::ParameterHint
