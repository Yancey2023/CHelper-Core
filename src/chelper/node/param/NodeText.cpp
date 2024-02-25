//
// Created by Yancey on 2023/12/2.
//

#include "NodeText.h"
#include "../../util/TokenUtil.h"

#include <utility>

namespace CHelper::Node {

    CHelper::Node::NodeText::NodeText(const std::optional<std::string> &id,
                                      const std::optional<std::string> &description,
                                      std::shared_ptr<NormalId> data)
            : NodeBase(id, description, false),
              data(std::move(data)) {}

    CHelper::Node::NodeText::NodeText(const nlohmann::json &j,
                                      [[maybe_unused]] const CPack &cpack)
            : NodeBase(j),
              data(std::make_shared<NormalId>(j.at("data"))) {}

    NodeType NodeText::getNodeType() const {
        return NodeType::TEXT;
    }

    void CHelper::Node::NodeText::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        nlohmann::json dataJson;
        data->toJson(dataJson);
        j.push_back(dataJson);
    }

    ASTNode NodeText::getASTNode(TokenReader &tokenReader) const {
        return tokenReader.getStringASTNode(this);
    }

    bool NodeText::collectIdError(const ASTNode *astNode,
                                  std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = TokenUtil::toString(astNode->tokens);
        if (str != data->name) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, "找不到含义 -> " + str));
        }
        return true;
    }

    bool NodeText::collectSuggestions(const ASTNode *astNode,
                                      size_t index,
                                      std::vector<Suggestion> &suggestions) const {
        std::string str = TokenUtil::toString(astNode->tokens)
                .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));
        if (StringUtil::isStartOf(data->name, str)) {
            suggestions.emplace_back(astNode->tokens, data);
        }
        return true;
    }

    void NodeText::collectStructure(const ASTNode *astNode,
                                    StructureBuilder &structure,
                                    bool isMustHave) const {
        structure.appendWhiteSpace().append(data->name);
    }

} // CHelper::Node