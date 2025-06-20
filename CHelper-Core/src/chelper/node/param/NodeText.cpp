//
// Created by Yancey on 2023/12/2.
//

#include <chelper/lexer/Lexer.h>
#include <chelper/node/NodeType.h>
#include <chelper/node/param/NodeText.h>

namespace CHelper::Node {

    NodeText::NodeText(const std::optional<std::string> &id,
                       const std::optional<std::u16string> &description,
                       const std::shared_ptr<NormalId> &data,
                       const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getTextASTNode)
        : NodeSerializable(id, description, false),
          data(data),
          getTextASTNode(getTextASTNode) {}

    void NodeText::init(const CPack &cpack) {
        getTextASTNode = [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
            return tokenReader.readUntilSpace(node);
        };
    }

    NodeTypeId::NodeTypeId NodeText::getNodeType() const {
        return NodeTypeId::TEXT;
    }

    ASTNode NodeText::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        DEBUG_GET_NODE_BEGIN(this)
        auto result = getTextASTNode(this, tokenReader);
        DEBUG_GET_NODE_END(this)
        std::u16string_view str = result.tokens.toString();
        if (HEDLEY_UNLIKELY(str != data->name)) {
            TokensView tokens = result.tokens;
            if (HEDLEY_UNLIKELY(str.empty())) {
                return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::contentError(tokens, u"命令不完整"));
            } else {
                return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::contentError(tokens, fmt::format(u"找不到含义 -> {}", str)));
            }
        }
        return result;
    }

    void NodeText::collectStructure(const ASTNode *astNode,
                                    StructureBuilder &structure,
                                    bool isMustHave) const {
        structure.appendSpace().append(data->name);
    }

    bool NodeText::collectSyntax(const ASTNode *astNode,
                                 SyntaxResult &syntaxResult) const {
        if (id != "TARGET_SELECTOR_ARGUMENT_EQUAL" && id != "TARGET_SELECTOR_ARGUMENT_NOT_EQUAL") {
            syntaxResult.update(astNode->tokens, SyntaxTokenType::LITERAL);
        } else {
            syntaxResult.update(astNode->tokens, SyntaxTokenType::SYMBOL);
        }
        return true;
    }

}// namespace CHelper::Node