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

    std::shared_ptr<NodeType> NodePosition::getNodeType() const {
        return NodeType::POSITION;
    }

    ASTNode NodePosition::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        // 0 - 绝对坐标，1 - 相对坐标，2 - 局部坐标
        std::vector<ASTNode> threeChildNodes;
        int types[3];
        for (int &type: types) {
            auto node = NodeRelativeFloat::getASTNode(this, cpack, tokenReader, true);
            type = node.first;
            threeChildNodes.push_back(node.second);
        }
        //判断有没有错误
        VectorView <Token> tokens = tokenReader.collect();
        ASTNode result = ASTNode::andNode(this, threeChildNodes, tokens, nullptr, "position");
        if (!result.isError()) {
            int type = 0;
            for (int i: types) {
                if (i == type) {
                    continue;
                }
                if (type == 0) {
                    type = i;
                }

                if (i == 0 || i == type) {
                    continue;
                } else if (type == 0) {
                    type = i;
                } else {
                    return ASTNode::andNode(this, threeChildNodes, tokens,
                                            ErrorReason::logicError(tokens, "绝对坐标和相对坐标不能于局部坐标混用"),
                                            "position");
                }
            }
        }
        return result;
    }

    void NodePosition::collectStructure(const ASTNode *astNode,
                                        StructureBuilder &structure,
                                        bool isMustHave) const {
        if (astNode == nullptr || astNode->id == "position") {
            structure.append(isMustHave, "位置");
        }
    }

} // CHelper::Node