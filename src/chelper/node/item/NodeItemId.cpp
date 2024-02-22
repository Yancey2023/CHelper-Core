//
// Created by Yancey666 on 2024/2/22.
//

#include "NodeItemId.h"
#include "../../resources/CPack.h"
#include "../../util/StringUtil.h"

namespace CHelper::Node {

    NODE_TYPE_INSTANCE("ITEM_ID", NodeItemId, "item id")

    ASTNode NodeItemId::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return getStringASTNode(tokenReader);
    }

    bool NodeItemId::collectIdError(const ASTNode *astNode,
                                    const CPack &cpack,
                                    std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = astNode->tokens[0].content;
        std::string_view nameSpace = "minecraft";
        std::string_view value = str;
        size_t index = str.find(':');
        if (index != std::string::npos) {
            nameSpace = value.substr(0, index);
            value = value.substr(index + 1);
        }
        for (const auto &item: cpack.blockIds) {
            if (value == item->name && nameSpace == item->nameSpace.value_or("minecraft")) {
                return true;
            }
        }
        idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, "找不到ID -> " + str));
        return true;
    }

    bool NodeItemId::collectSuggestions(const ASTNode *astNode,
                                        const CPack &cpack,
                                        std::vector<Suggestion> &suggestions) const {
        if (astNode->isError()) {
            return true;
        }
        std::string_view str = astNode->tokens[0].content;
        //省略minecraft命名空间
        for (const auto &item: cpack.itemIds) {
            if ((!item->nameSpace.has_value() || item->nameSpace.value() == "minecraft") &&
                StringUtil::isStartOf(item->name, str)) {
                suggestions.emplace_back(astNode->tokens, item);
            }
        }
        //带有命名空间
        for (const auto &item: cpack.itemIds) {
            if (StringUtil::isStartOf(item->idWithNamespace->name, str)) {
                suggestions.emplace_back(astNode->tokens, item->idWithNamespace);
            }
        }
        return true;
    }

} // CHelper::Node