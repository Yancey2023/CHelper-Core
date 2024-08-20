//
// Created by Yancey on 2023/12/2.
//

#include "NodeText.h"
#include "../../lexer/Lexer.h"

namespace CHelper::Node {

    NodeText::NodeText(const std::optional<std::u16string> &id,
                       const std::optional<std::u16string> &description,
                       const std::shared_ptr<NormalId> &data,
                       const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getTextASTNode)
        : NodeBase(id, description, false),
          data(data),
          getTextASTNode(getTextASTNode) {}


    void NodeText::init(const CPack &cpack) {
        getTextASTNode = [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
            return tokenReader.readUntilWhitespace(node);
        };
    }

    NodeType *NodeText::getNodeType() const {
        return NodeType::TEXT.get();
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
                return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::contentError(tokens, u"找不到含义 -> " + std::u16string(str)));
            }
        }
        return result;
    }

    bool NodeText::collectSuggestions(const ASTNode *astNode,
                                      size_t index,
                                      std::vector<Suggestions> &suggestions) const {
        std::u16string_view str = astNode->tokens.toString()
                                       .substr(0, index - astNode->tokens.getStartIndex());
        //通过名字进行搜索
        size_t index1 = data->name.find(str);
        if (HEDLEY_LIKELY(index1 != std::u16string::npos)) {
            suggestions.push_back(Suggestions::singleLiteralSuggestion({astNode->tokens, isAfterWhitespace(), data}));
            return true;
        }
        //通过介绍进行搜索
        if (HEDLEY_LIKELY(data->description.has_value())) {
            size_t index2 = data->description.value().find(str);
            if (HEDLEY_LIKELY(index2 != std::u16string::npos)) {
                suggestions.push_back(Suggestions::singleLiteralSuggestion({astNode->tokens, isAfterWhitespace(), data}));
            }
        }
        return true;
    }

    void NodeText::collectStructure(const ASTNode *astNode,
                                    StructureBuilder &structure,
                                    bool isMustHave) const {
        structure.appendWhiteSpace().append(data->name);
    }

    bool NodeText::collectColor(const ASTNode *astNode,
                                ColoredString &coloredString,
                                const Theme &theme) const {
        if (id != u"TARGET_SELECTOR_ARGUMENT_EQUAu" && id != u"TARGET_SELECTOR_ARGUMENT_NOT_EQUAu") {
            coloredString.setColor(astNode->tokens, theme.colorLiteral);
        } else {
            coloredString.setColor(astNode->tokens, theme.colorSymbol);
        }
        return true;
    }

    CODEC_NODE(NodeText, tokenTypes, data)

}// namespace CHelper::Node