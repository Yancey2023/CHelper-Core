//
// Created by Yancey on 2023/12/10.
//

#include "NodeString.h"
#include "../util/NodeSingleSymbol.h"

namespace CHelper::Node {

    std::shared_ptr<NodeSingleSymbol> doubleQuotationMark = std::make_shared<NodeSingleSymbol>(
            "DOUBLE_QUOTATION_MARK", "双引号", '"');

    NodeString::NodeString(const std::optional<std::string> &id,
                           const std::optional<std::string> &description,
                           const bool canContainSpace,
                           const bool ignoreLater)
            : NodeBase(id, description, false),
              canContainSpace(canContainSpace),
              ignoreLater(ignoreLater) {}

    NodeString::NodeString(const nlohmann::json &j,
                           const CPack &cpack)
            : NodeBase(j),
              canContainSpace(FROM_JSON(j, canContainSpace, bool)),
              ignoreLater(FROM_JSON(j, ignoreLater, bool)) {}

    NodeType NodeString::getNodeType() const {
        return NodeType::STRING;
    }

    ASTNode NodeString::getASTNode(TokenReader &tokenReader) const {
        if (ignoreLater) {
            //后面的所有内容都算作这个字符串
            tokenReader.push();
            tokenReader.skipToLF();
            return ASTNode::simpleNode(this, tokenReader.collect());
        }
        if (canContainSpace) {
            tokenReader.push();
            ASTNode doubleQuotationMarkStart = doubleQuotationMark->getASTNode(tokenReader);
            if (!doubleQuotationMarkStart.isError()) {
                //使用双引号
                ASTNode stringNode = tokenReader.getStringASTNode(this);
                ASTNode doubleQuotationMarkEnd = doubleQuotationMark->getASTNode(tokenReader);
                return ASTNode::andNode(this, {doubleQuotationMarkStart, stringNode, doubleQuotationMarkEnd},
                                        tokenReader.collect());
            }
            tokenReader.restore();
        }
        //普通字符串
        return tokenReader.getStringASTNode(this);
    }

    void NodeString::collectStructure(const ASTNode *astNode,
                                      StructureBuilder &structure,
                                      bool isMustHave) const {
        if (astNode == nullptr || astNode->id == "string") {
            structure.append(isMustHave, description.value_or("字符串"));
        }
    }

} // CHelper::Node