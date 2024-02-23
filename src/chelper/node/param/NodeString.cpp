//
// Created by Yancey on 2023/12/10.
//

#include "NodeString.h"

namespace CHelper::Node {

    NODE_TYPE("STRING", NodeString)

    NodeString::NodeString(const std::optional<std::string> &id,
                           const std::optional<std::string> &description,
                           const bool canContainSpace,
                           const bool ignoreLater)
            : NodeBase(id, description),
              canContainSpace(canContainSpace),
              ignoreLater(ignoreLater) {}

    NodeString::NodeString(const nlohmann::json &j,
                           const CPack &cpack)
            : NodeBase(j, cpack),
              canContainSpace(FROM_JSON(j, canContainSpace, bool)),
              ignoreLater(FROM_JSON(j, ignoreLater, bool)) {}

    ASTNode NodeString::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        if (ignoreLater) {
            //后面的所有内容都算作这个字符串
            tokenReader.push();
            tokenReader.skipToLF();
            return ASTNode::simpleNode(this, tokenReader.collect());
        }
        if (canContainSpace) {
            tokenReader.push();
            ASTNode doubleQuotationMarkStart = getSymbolASTNode(tokenReader, '"');
            if (!doubleQuotationMarkStart.isError()) {
                //使用双引号
                ASTNode stringNode = getStringASTNode(tokenReader);
                ASTNode doubleQuotationMarkEnd = getSymbolASTNode(tokenReader, '"');
                return ASTNode::andNode(this, {doubleQuotationMarkStart, stringNode, doubleQuotationMarkEnd},
                                        tokenReader.collect());
            }
            tokenReader.restore();
        }
        //普通字符串
        return getStringASTNode(tokenReader);
    }

    void NodeString::collectStructure(const ASTNode *astNode,
                                      StructureBuilder &structure,
                                      bool isMustHave) const {
        if (astNode->id == "string") {
            structure.append(isMustHave, description.value_or("字符串"));
        }
    }

} // CHelper::Node