//
// Created by Yancey on 2023/12/2.
//

#include "NodePosition.h"
#include "NodeRelativeFloat.h"

namespace CHelper::Node {

    NodePosition::NodePosition(const std::optional<std::string> &id,
                               const std::optional<std::string> &description)
            : NodeBase(id, description, false) {}

    NodePosition::NodePosition(const nlohmann::json &j,
                               [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, true) {}

    NodeType *NodePosition::getNodeType() const {
        return NodeType::POSITION.get();
    }

    ASTNode NodePosition::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        // 0 - 绝对坐标，1 - 相对坐标，2 - 局部坐标
        std::vector<ASTNode> threeChildNodes;
        threeChildNodes.reserve(3);
        std::uint8_t types[3];
        for (std::uint8_t &type: types) {
            std::pair<std::uint8_t, ASTNode> node = NodeRelativeFloat::getASTNode(this, cpack, tokenReader);
            type = node.first;
            threeChildNodes.push_back(std::move(node.second));
        }
        //判断有没有错误
        VectorView <Token> tokens = tokenReader.collect();
        ASTNode result = ASTNode::andNode(this, std::move(threeChildNodes), tokens, nullptr);
        if (HEDLEY_UNLIKELY(!result.isError())) {
            std::uint8_t type = 0;
            for (std::uint8_t item: types) {
                if (HEDLEY_LIKELY(item == 0 || item == type)) {
                    continue;
                } else if (HEDLEY_LIKELY(type == 0)) {
                    type = item;
                } else {
                    return ASTNode::andNode(this, {std::move(result)}, tokens,
                                            ErrorReason::logicError(tokens, "绝对坐标和相对坐标不能与局部坐标混用"));
                }
            }
        }
        return std::move(result);
    }

    void NodePosition::collectStructure(const ASTNode *astNode,
                                        StructureBuilder &structure,
                                        bool isMustHave) const {
        structure.append(isMustHave, "位置");
    }

} // CHelper::Node