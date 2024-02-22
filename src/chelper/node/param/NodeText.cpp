//
// Created by Yancey666 on 2023/12/2.
//

#include "NodeText.h"

namespace CHelper::Node {

    NODE_TYPE("TEXT", NodeText)

    CHelper::Node::NodeText::NodeText(const std::optional<std::string> &id,
                                      const std::optional<std::string> &description,
                                      const CHelper::NormalId &data)
            : NodeBase(id, description),
              data(data) {}

    CHelper::Node::NodeText::NodeText(const nlohmann::json &j,
                                      const CPack &cpack)
            : NodeBase(j, cpack),
              data(FROM_JSON(j, data, NormalId)) {}

    void CHelper::Node::NodeText::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, data);
    }

    ASTNode NodeText::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return getStringASTNode(tokenReader);
    }

    bool NodeText::collectIdError(const ASTNode *astNode,
                                  std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = astNode->tokens[0].content;
        if (str != data.name) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, "找不到含义 -> " + str));
        }
        return true;
    }

} // CHelper::Node