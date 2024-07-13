//
// Created by Yancey on 2023/12/2.
//

#include "NodePosition.h"
#include "NodeRelativeFloat.h"

namespace CHelper::Node {

    NodePosition::NodePosition(const std::optional<std::string> &id,
                               const std::optional<std::string> &description)
        : NodeBase(id, description, false) {}

    NodeType *NodePosition::getNodeType() const {
        return NodeType::POSITION.get();
    }

    ASTNode NodePosition::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        // 0 - 绝对坐标，1 - 相对坐标，2 - 局部坐标
        std::vector<ASTNode> threeChildNodes;
        threeChildNodes.reserve(3);
        uint8_t types[3];
        for (uint8_t &type: types) {
            std::pair<uint8_t, ASTNode> node = NodeRelativeFloat::getASTNode(this, cpack, tokenReader);
            if (threeChildNodes.empty() && node.second.isError() && !node.second.tokens.isEmpty()) {
                tokenReader.pop();
                TokensView tokens = node.second.tokens;
                return ASTNode::andNode(this, {std::move(node.second)}, tokens, nullptr, "positions");
            }
            type = node.first;
            threeChildNodes.push_back(std::move(node.second));
        }
        //判断有没有错误
        TokensView tokens = tokenReader.collect();
        ASTNode result = ASTNode::andNode(this, std::move(threeChildNodes), tokens, nullptr, "positions");
        if (HEDLEY_UNLIKELY(!result.isError())) {
            uint8_t type = 0;
            for (uint8_t item: types) {
                if (HEDLEY_LIKELY(item == 0 || item == type)) {
                    continue;
                } else if (HEDLEY_LIKELY(type == 0)) {
                    type = item;
                } else {
                    return ASTNode::andNode(this, {std::move(result)}, tokens, nullptr, "NodePositionMixError");
                }
            }
        }
        return std::move(result);
    }

    bool NodePosition::collectIdError(const ASTNode *astNode,
                                      std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(!astNode->isError() && astNode->id == "NodePositionMixError")) {
            idErrorReasons.push_back(ErrorReason::logicError(astNode->tokens, "绝对坐标和相对坐标不能与局部坐标混用"));
            return true;
        } else {
            return false;
        }
    }

    bool NodePosition::collectSuggestions(const ASTNode *astNode, size_t index, std::vector<Suggestions> &suggestions) const {
        if (HEDLEY_LIKELY(astNode->id != "positions")) {
            return false;
        }
        int errorCount = 0;
        for (const auto &item: astNode->childNodes) {
            if (item.isError()) {
                errorCount++;
            }
        }
        if (errorCount > 0) {
            return NodeRelativeFloat::collectSuggestions(index, suggestions, true);
        }
        return true;
    }

    void NodePosition::collectStructure(const ASTNode *astNode,
                                        StructureBuilder &structure,
                                        bool isMustHave) const {
        structure.append(isMustHave, "位置");
    }

    CODEC_UNIQUE_PTR(NodePosition)

}// namespace CHelper::Node