//
// Created by Yancey on 2024/2/27.
//

#include "NodeRange.h"
#include "NodeText.h"
#include "../util/NodeOr.h"
#include "NodeFloat.h"
#include "../util/NodeAnd.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeRange::NodeRange(const std::optional<std::string> &id,
                         const std::optional<std::string> &description)
            : NodeBase(id, description, false) {}

    NodeRange::NodeRange(const nlohmann::json &j,
                         [[maybe_unused]] const CPack &cpack) :
            NodeBase(j, true) {}

    std::shared_ptr<NodeType> NodeRange::getNodeType() const {
        return NodeType::RANGE;
    }

    std::shared_ptr<ErrorReason> checkNumber(const VectorView <Token> &tokens, std::string_view str) {
        if (str.empty()) {
            return ErrorReason::contentError(tokens, "范围的数值为空");
        }
        for (const auto &item: str) {
            if (item < '0' || item > '9') {
                return ErrorReason::contentError(tokens, "范围的数值格式不正确，检测非法字符");
            }
        }
        return nullptr;
    }

    ASTNode NodeRange::getASTNode(TokenReader &tokenReader) const {
        ASTNode result = tokenReader.readStringASTNode(this);
        std::string str = TokenUtil::toString(result.tokens);
        std::shared_ptr<ErrorReason> errorReason;
        size_t index = str.find("..");
        if (index == std::string::npos) {
            errorReason = checkNumber(result.tokens, str);
        } else if (index == 0) {
            errorReason = checkNumber(result.tokens, std::string_view(str).substr(2));
        } else {
            errorReason = checkNumber(result.tokens, std::string_view(str).substr(0, index));
            if (errorReason == nullptr && index + 2 < str.length()) {
                errorReason = checkNumber(result.tokens, std::string_view(str).substr(index + 2));
            }
        }
        return ASTNode::simpleNode(this, result.tokens, errorReason);
    }

    bool NodeRange::collectSuggestions(const ASTNode *astNode,
                                       size_t index,
                                       std::vector<Suggestion> &suggestions) const {
        std::string str = TokenUtil::toString(astNode->tokens);
        size_t index0 = str.find("..");
        if (index0 != std::string::npos) {
            if (index != index0 && index != index0 + 1 && index != index0 + 2) {
                return true;
            }
            suggestions.emplace_back(astNode->tokens, std::make_shared<NormalId>(str, "范围"));
            return true;
        }
        size_t index1 = str.find('.');
        if (index1 != std::string::npos) {
            if (index != index1 && index != index1 + 1) {
                return true;
            }
            std::string text = str.substr(0, index1).append(".").append(str.substr(index1));
            suggestions.emplace_back(astNode->tokens, std::make_shared<NormalId>(text, "范围"));
            return true;
        }
        size_t index2 = index - TokenUtil::getStartIndex(astNode->tokens);
        std::string text = str.substr(0, index2).append("..").append(str.substr(index2));
        suggestions.emplace_back(astNode->tokens, std::make_shared<NormalId>(text, "范围"));
        return true;
    }

    void NodeRange::collectStructure(const ASTNode *astNode, StructureBuilder &structure, bool isMustHave) const {
        structure.append(isMustHave, description.value_or("范围"));
    }

} // CHelper::Node