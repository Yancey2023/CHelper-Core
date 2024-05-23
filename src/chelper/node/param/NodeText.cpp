//
// Created by Yancey on 2023/12/2.
//

#include "NodeText.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeText::NodeText(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       const std::shared_ptr<NormalId> &data,
                       const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getTextASTNode)
        : NodeBase(id, description, false),
          data(data),
          getTextASTNode(getTextASTNode) {}

    NodeText::NodeText(const nlohmann::json &j,
                       [[maybe_unused]] const CPack &cpack)
        : NodeBase(j, true),
          data(std::make_shared<NormalId>(j.at("data"))),
          getTextASTNode([](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
              return tokenReader.readUntilWhitespace(node);
          }) {}

    NodeText::NodeText(BinaryReader &binaryReader,
                       [[maybe_unused]] const CPack &cpack)
        : NodeBase(binaryReader),
          data(binaryReader.read<std::shared_ptr<NormalId>>()),
          getTextASTNode([](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
              return tokenReader.readUntilWhitespace(node);
          }) {}

    NodeType *NodeText::getNodeType() const {
        return NodeType::TEXT.get();
    }

    void NodeText::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::encode(j, "data", data);
    }

    void NodeText::writeBinToFile(BinaryWriter &binaryWriter) const {
        NodeBase::writeBinToFile(binaryWriter);
        binaryWriter.encode(data);
    }

    ASTNode NodeText::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        DEBUG_GET_NODE_BEGIN(this)
        auto result = getTextASTNode(this, tokenReader);
        DEBUG_GET_NODE_END(this)
        std::string str = TokenUtil::toString(result.tokens);
        if (HEDLEY_UNLIKELY(str != data->name)) {
            VectorView<Token> tokens = result.tokens;
            if (HEDLEY_UNLIKELY(str.empty())) {
                return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::contentError(tokens, "命令不完整"));
            } else {
                return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::contentError(tokens, "找不到含义 -> " + std::move(str)));
            }
        }
        return result;
    }

    bool NodeText::collectSuggestions(const ASTNode *astNode,
                                      size_t index,
                                      std::vector<Suggestions> &suggestions) const {
        std::string str = TokenUtil::toString(astNode->tokens)
                                  .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));
        //通过名字进行搜索
        size_t index1 = data->name.find(str);
        if (HEDLEY_LIKELY(index1 != std::string::npos)) {
            suggestions.push_back(Suggestions::singleSuggestion({astNode->tokens, data}));
            return true;
        }
        //通过介绍进行搜索
        if (HEDLEY_LIKELY(data->description.has_value())) {
            size_t index2 = data->description.value().find(str);
            if (HEDLEY_LIKELY(index2 != std::string::npos)) {
                suggestions.push_back(Suggestions::singleSuggestion({astNode->tokens, data}));
            }
        }
        return true;
    }

    void NodeText::collectStructure(const ASTNode *astNode,
                                    StructureBuilder &structure,
                                    bool isMustHave) const {
        structure.appendWhiteSpace().append(data->name);
    }

}// namespace CHelper::Node