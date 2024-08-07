//
// Created by Yancey on 2023/11/11.
//

#include "NodeNormalId.h"

#include "../../lexer/Lexer.h"

namespace CHelper::Node {

    NodeNormalId::NodeNormalId(
            const std::optional<std::string> &id,
            const std::optional<std::string> &description,
            const std::string &key,
            bool ignoreError,
            bool allowMissingID,
            const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getNormalIdASTNode)
        : NodeBase(id, description, false),
          key(key),
          ignoreError(ignoreError),
          allowMissingID(allowMissingID),
          getNormalIdASTNode(getNormalIdASTNode) {}

    NodeNormalId::NodeNormalId(
            const std::optional<std::string> &id,
            const std::optional<std::string> &description,
            bool ignoreError,
            const std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> &contents,
            bool allowMissingID,
            const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getNormalIdASTNode)
        : NodeBase(id, description, false),
          ignoreError(ignoreError),
          contents(contents),
          allowMissingID(allowMissingID),
          getNormalIdASTNode(getNormalIdASTNode) {
#if CHelperDebug == true
        if (HEDLEY_UNLIKELY(contents == nullptr)) {
            throw std::runtime_error("contents should not be nullptr");
        }
#endif
        customContents = contents;
    }

    void NodeNormalId::init(const CPack &cpack) {
        if (HEDLEY_UNLIKELY(getNormalIdASTNode == nullptr)) {
            getNormalIdASTNode = [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readUntilWhitespace(node);
            };
        }
        if (HEDLEY_LIKELY(contents.has_value())) {
            customContents = contents.value();
        } else if (HEDLEY_LIKELY(key.has_value())) {
            customContents = cpack.getNormalId(key.value());
        }
        if (HEDLEY_UNLIKELY(customContents == nullptr)) {
            if (HEDLEY_LIKELY(key.has_value())) {
                Profile::push("linking contents to {}", key.value());
                Profile::push("failed to find normal id in the cpack -> ", key.value());
                throw std::runtime_error("failed to find normal id");
            } else {
                throw std::runtime_error("missing content");
            }
        }
    }

    NodeType *NodeNormalId::getNodeType() const {
        return NodeType::NORMAL_ID.get();
    }

    ASTNode NodeNormalId::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        DEBUG_GET_NODE_BEGIN(this)
        ASTNode result = getNormalIdASTNode(this, tokenReader);
        DEBUG_GET_NODE_END(this)
        if (HEDLEY_UNLIKELY(allowMissingID)) {
            if (HEDLEY_UNLIKELY(result.isError())) {
                tokenReader.restore();
                tokenReader.push();
                return ASTNode::simpleNode(this, tokenReader.collect());
            }
            tokenReader.pop();
            return result;
        }
        tokenReader.pop();
        if (HEDLEY_UNLIKELY(result.tokens.isEmpty())) {
            TokensView tokens = result.tokens;
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::incomplete(tokens, "命令不完整"));
        }
        if (HEDLEY_UNLIKELY(!ignoreError.value_or(true))) {
            TokensView tokens = result.tokens;
            std::string_view str = tokens.toString();
            size_t strHash = std::hash<std::string_view>{}(str);
            if (HEDLEY_UNLIKELY(std::all_of(customContents->begin(), customContents->end(), [&strHash](const auto &item) {
                    return !item->fastMatch(strHash);
                }))) {
                return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::incomplete(tokens, "找不到含义 -> " + std::string(str)));
            }
        }
        return result;
    }

    bool NodeNormalId::collectIdError(const ASTNode *astNode,
                                      std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->isError())) {
            return true;
        }
        std::string_view str = astNode->tokens.toString();
        size_t strHash = std::hash<std::string_view>{}(str);
        if (HEDLEY_UNLIKELY(std::all_of(customContents->begin(), customContents->end(), [&strHash](const auto &item) {
                return !item->fastMatch(strHash);
            }))) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("找不到ID -> ").append(str)));
        }
        return true;
    }

    bool NodeNormalId::collectSuggestions(const ASTNode *astNode,
                                          size_t index,
                                          std::vector<Suggestions> &suggestions) const {
        KMPMatcher kmpMatcher(astNode->tokens.toString().substr(0, index - astNode->tokens.getStartIndex()));
        std::vector<std::shared_ptr<NormalId>> nameStartOf, nameContain, descriptionContain;
        for (const auto &item: *customContents) {
            //通过名字进行搜索
            size_t index1 = kmpMatcher.match(item->name);
            if (HEDLEY_UNLIKELY(index1 != std::string::npos)) {
                if (HEDLEY_UNLIKELY(index1 == 0)) {
                    nameStartOf.push_back(item);
                } else {
                    nameContain.push_back(item);
                }
                continue;
            }
            //通过介绍进行搜索
            if (HEDLEY_UNLIKELY(
                        item->description.has_value() && kmpMatcher.match(item->description.value()) != std::string::npos)) {
                descriptionContain.push_back(item);
            }
        }
        Suggestions suggestions1;
        suggestions1.suggestions.reserve(nameStartOf.size() + nameContain.size() + descriptionContain.size());
        size_t start = astNode->tokens.getStartIndex();
        size_t end = astNode->tokens.getEndIndex();
        std::transform(nameStartOf.begin(), nameStartOf.end(),
                       std::back_inserter(suggestions1.suggestions),
                       [&start, &end, this](const auto &item) {
                           return Suggestion(start, end, isAfterWhitespace(), item);
                       });
        std::transform(nameContain.begin(), nameContain.end(),
                       std::back_inserter(suggestions1.suggestions),
                       [&start, &end, this](const auto &item) {
                           return Suggestion(start, end, isAfterWhitespace(), item);
                       });
        std::transform(descriptionContain.begin(), descriptionContain.end(),
                       std::back_inserter(suggestions1.suggestions),
                       [&start, &end, this](const auto &item) {
                           return Suggestion(start, end, isAfterWhitespace(), item);
                       });
        suggestions1.markFiltered();
        suggestions.push_back(std::move(suggestions1));
        return true;
    }

    void NodeNormalId::collectStructure(const ASTNode *astNode,
                                        StructureBuilder &structure,
                                        bool isMustHave) const {
        structure.append(isMustHave, description.value_or("ID"));
    }

    bool NodeNormalId::collectColor(const ASTNode *astNode,
                                    ColoredString &coloredString,
                                    const Theme &theme) const {
        if (key.has_value()) {
            coloredString.setColor(astNode->tokens, theme.colorId);
        } else if (id != "TARGET_SELECTOR_VARIABLE") {
            coloredString.setColor(astNode->tokens, theme.colorLiteral);
        } else {
            coloredString.setColor(astNode->tokens, theme.colorTargetSelector);
        }
        return true;
    }

    CODEC_NODE(NodeNormalId, key, ignoreError, contents)

}// namespace CHelper::Node