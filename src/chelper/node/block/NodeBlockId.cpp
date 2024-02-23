//
// Created by Yancey on 2023/12/18.
//

#include "NodeBlockId.h"
#include "../../resources/CPack.h"

namespace CHelper::Node {

    NODE_TYPE_INSTANCE("BLOCK_ID", NodeBlockId, "方块id")

    ASTNode NodeBlockId::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return getStringASTNode(tokenReader, "block id");
    }

    bool NodeBlockId::collectIdError(const ASTNode *astNode,
                                     const CPack &cpack,
                                     std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->id != "block id") {
            return false;
        }
        if (astNode->isError()) {
            return true;
        }
        std::string str = astNode->tokens.size() == 0 ? "" : astNode->tokens[0].content;
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

    bool NodeBlockId::collectSuggestions(const ASTNode *astNode,
                                         const CPack &cpack,
                                         std::vector<Suggestion> &suggestions) const {
        if (astNode->id != "block id") {
            return false;
        }
        if (astNode->isError()) {
            return true;
        }
        std::string str = astNode->tokens.size() == 0 ? "" : astNode->tokens[0].content;
        //省略minecraft命名空间
        for (const auto &item: cpack.blockIds) {
            if ((!item->nameSpace.has_value() || item->nameSpace.value() == "minecraft") &&
                StringUtil::isStartOf(item->name, str)) {
                suggestions.emplace_back(astNode->tokens, item);
            }
        }
        //带有命名空间
        for (const auto &item: cpack.blockIds) {
            if (StringUtil::isStartOf(item->idWithNamespace->name, str)) {
                suggestions.emplace_back(astNode->tokens, item->idWithNamespace);
            }
        }
        return true;
    }

    void NodeBlockId::collectStructure(const ASTNode *astNode,
                                       StructureBuilder &structure,
                                       bool isMustHave) const {
        if (astNode->id != "block id") {
            return;
        }
        structure.append(true, "方块ID");
    }

} // CHelper::Node