//
// Created by Yancey on 2024/2/14.
//

#include "NodeTargetSelectorVariable.h"
#include "../param/NodeString.h"

namespace CHelper::Node {

    NodeTargetSelectorVariable::NodeTargetSelectorVariable(const std::optional<std::string> &id,
                                                           const std::optional<std::string> &description,
                                                           const bool isMustPlayer,
                                                           const bool isMustNPC,
                                                           const bool isOnlyOne)
            : NodeBase(id, description),
              isMustPlayer(isMustPlayer),
              isMustNPC(isMustNPC),
              isOnlyOne(isOnlyOne) {}

    ASTNode NodeTargetSelectorVariable::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        //TODO 在资源包中支持自定义目标选择器变量
        //目标选择器变量在ID检查阶段进行检测
        return getStringASTNode(tokenReader);
    }

    bool NodeTargetSelectorVariable::collectIdError(const ASTNode *astNode,
                                                    const CPack &cpack,
                                                    std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = astNode->tokens.size() == 0 ? "" : astNode->tokens[0].content;
        if (str != "p" && str != "r" && str != "a" &&
            str != "e" && str != "s" && str != "c" &&
            str != "v" && str != "initiator") {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, "找不到目标选择器变量 -> @" + str));
        }
        return true;
    }

} // CHelper::Node