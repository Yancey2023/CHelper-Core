//
// Created by Yancey on 2023/12/22.
//

#include "NodeOr.h"

namespace CHelper::Node {

    NodeOr::NodeOr(const std::optional<std::string> &id,
                   const std::optional<std::string> &description,
                   std::vector<const NodeBase *> childNodes,
                   const bool isAttachToEnd,
                   const bool isUseFirst,
                   const bool noSuggestion,
                   const char *defaultErrorReason,
                   std::string nodeId)
        : NodeBase(id, description, false),
          isAttachToEnd(isAttachToEnd),
          isUseFirst(isUseFirst),
          noSuggestion(noSuggestion),
          childNodes(std::move(childNodes)),
          defaultErrorReason(defaultErrorReason),
          nodeId(std::move(nodeId)) {
#if CHelperDebug == true
        for (const auto &item: this->childNodes) {
            if (HEDLEY_UNLIKELY(item == nullptr)) {
                Profile::push("null node in node or");
                throw Exception::NodeLoadFailed();
            }
        }
#endif
    }

    NodeType *NodeOr::getNodeType() const {
        return NodeType::OR.get();
    }

    ASTNode NodeOr::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        std::vector<ASTNode> childASTNodes;
        std::vector<size_t> indexes;
        if (HEDLEY_LIKELY(!isUseFirst)) {
            childASTNodes.reserve(childNodes.size());
            indexes.reserve(childNodes.size());
        }
        for (const auto &item: childNodes) {
            tokenReader.push();
            ASTNode node = item->getASTNode(tokenReader, cpack);
            bool isNodeError = node.isError();
            childASTNodes.push_back(std::move(node));
            indexes.push_back(tokenReader.index);
            tokenReader.restore();
            if (HEDLEY_UNLIKELY(isUseFirst && !isNodeError)) {
                break;
            }
        }
        if (HEDLEY_UNLIKELY(isAttachToEnd)) {
            tokenReader.push();
            tokenReader.skipToLF();
            const VectorView<Token> tokens = tokenReader.collect();
            return ASTNode::orNode(this, std::move(childASTNodes), tokens, defaultErrorReason, nodeId);
        } else {
            ASTNode result = ASTNode::orNode(this, std::move(childASTNodes), nullptr, defaultErrorReason, nodeId);
            tokenReader.index = indexes[result.whichBest];
            return std::move(result);
        }
    }

    std::optional<std::string> NodeOr::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

    bool NodeOr::collectSuggestions(const ASTNode *astNode,
                                    size_t index,
                                    std::vector<Suggestions> &suggestions) const {
        return noSuggestion;
    }

}// namespace CHelper::Node