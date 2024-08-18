//
// Created by Yancey on 2023/12/2.
//

#include "NodePosition.h"
#include "NodeRelativeFloat.h"

namespace CHelper::Node {

    NodePosition::NodePosition(const std::optional<std::wstring> &id,
                               const std::optional<std::wstring> &description)
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
                return ASTNode::andNode(this, {std::move(node.second)}, tokens, nullptr, ASTNodeId::NODE_POSITION_POSITIONS);
            }
            type = node.first;
            threeChildNodes.push_back(std::move(node.second));
        }
        //判断有没有错误
        TokensView tokens = tokenReader.collect();
        ASTNode result = ASTNode::andNode(this, std::move(threeChildNodes), tokens, nullptr, ASTNodeId::NODE_POSITION_POSITIONS);
        if (HEDLEY_UNLIKELY(!result.isError())) {
            uint8_t count = 0;
            for (uint8_t item: types) {
                if (HEDLEY_UNLIKELY(item == 3)) {
                    count++;
                }
            }
            if (count == 1 || count == 2) {
                return ASTNode::andNode(this, {std::move(result)}, tokens, nullptr, ASTNodeId::NODE_POSITION_POSITIONS_WITH_ERROR);
            }
        }
        return std::move(result);
    }

    bool NodePosition::collectIdError(const ASTNode *astNode,
                                      std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(!astNode->isError() && astNode->id == ASTNodeId::NODE_POSITION_POSITIONS_WITH_ERROR)) {
            idErrorReasons.push_back(ErrorReason::logicError(astNode->tokens, L"绝对坐标和相对坐标不能与局部坐标混用"));
            return true;
        } else {
            return false;
        }
    }

    bool NodePosition::collectSuggestions(const ASTNode *astNode, size_t index, std::vector<Suggestions> &suggestions) const {
        if (HEDLEY_LIKELY(astNode->id != ASTNodeId::NODE_POSITION_POSITIONS)) {
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
        structure.append(isMustHave, L"位置");
    }

    bool NodePosition::collectColor(const ASTNode *astNode,
                                    ColoredString &coloredString,
                                    const Theme &theme) const {
        if (astNode->id == ASTNodeId::NODE_RELATIVE_FLOAT_NUMBER) {
            coloredString.setColor(astNode->tokens, theme.colorFloat);
            return true;
        } else {
            return false;
        }
    }

    CODEC_UNIQUE_PTR(NodePosition)

}// namespace CHelper::Node