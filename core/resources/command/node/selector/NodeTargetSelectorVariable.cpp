//
// Created by Yancey666 on 2024/2/14.
//

#include "NodeTargetSelectorVariable.h"
#include "../NodeString.h"

namespace CHelper::Node {

    NODE_TYPE("TARGET_SELECTOR_VARIABLE", NodeTargetSelectorVariable)

    NodeTargetSelectorVariable::NodeTargetSelectorVariable(const std::optional<std::string> &id,
                                                           const std::optional<std::string> &description,
                                                           const bool isMustPlayer,
                                                           const bool isMustNPC,
                                                           const bool isOnlyOne)
            : NodeBase(id, description),
              isMustPlayer(isMustPlayer),
              isMustNPC(isMustNPC),
              isOnlyOne(isOnlyOne) {}

    NodeTargetSelectorVariable::NodeTargetSelectorVariable(const nlohmann::json &j,
                                                           const CPack &cpack)
            : NodeBase(j, cpack),
              isMustPlayer(FROM_JSON(j, isMustPlayer, bool)),
              isMustNPC(FROM_JSON(j, isMustNPC, bool)),
              isOnlyOne(FROM_JSON(j, isOnlyOne, bool)) {}

    void NodeTargetSelectorVariable::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, isMustPlayer);
        TO_JSON(j, isMustNPC);
        TO_JSON(j, isOnlyOne);
    }

    ASTNode NodeTargetSelectorVariable::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        //TODO 在资源包中支持自定义目标选择器变量
        //目标选择器变量在ID检查阶段进行检测
        return getStringASTNode(tokenReader);
    }

} // CHelper::Node