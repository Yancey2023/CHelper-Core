//
// Created by Yancey on 2023/11/11.
//

#include "NodeNormalId.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeNormalId::NodeNormalId(const std::optional<std::string> &id,
                               const std::optional<std::string> &description,
                               const std::optional<std::string> &key,
                               bool ignoreError,
                               const std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> &contents,
                               bool allowsMissingID,
                               ASTNode(*getNormalIdASTNode)(const NodeBase *node, TokenReader &tokenReader))
            : NodeBase(id, description, false),
              key(key),
              ignoreError(ignoreError),
              contents(contents),
              allowsMissingID(allowsMissingID),
              getNormalIdASTNode(getNormalIdASTNode) {}

    static std::shared_ptr<std::vector<std::shared_ptr<NormalId>>>
    getIdContentFromCPack(const nlohmann::json &j,
                          const CPack &cpack,
                          const std::optional<std::string> &key) {
        if (HEDLEY_LIKELY(key.has_value())) {
            auto it = cpack.normalIds.find(key.value());
            if (HEDLEY_UNLIKELY(it == cpack.normalIds.end())) {
                Profile::push(ColorStringBuilder()
                                      .red("linking contents to ")
                                      .purple(key.value())
                                      .build());
                Profile::push(ColorStringBuilder()
                                      .red("failed to find normal id in the cpack")
                                      .normal(" -> ")
                                      .purple(key.value())
                                      .build());
                throw Exception::NodeLoadFailed();
            }
            return it->second;
        } else {
            nlohmann::json jsonArray = j.at("contents");
            auto contents = std::make_shared<std::vector<std::shared_ptr<NormalId>>>();
            contents->reserve(jsonArray.size());
            for (const auto &item: jsonArray) {
                contents->push_back(std::make_shared<NormalId>(item));
            }
            return contents;
        }
    }

    NodeNormalId::NodeNormalId(const nlohmann::json &j,
                               const CPack &cpack)
            : NodeBase(j, true),
              key(JsonUtil::fromJsonOptionalLikely<std::string>(j, "key")),
              ignoreError(JsonUtil::fromJson<bool>(j, "ignoreError")),
              contents(getIdContentFromCPack(j, cpack, key)),
              allowsMissingID(false),
              getNormalIdASTNode([](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                  return tokenReader.readStringASTNode(node);
              }) {}

    NodeType *NodeNormalId::getNodeType() const {
        return NodeType::NORMAL_ID.get();
    }

    void NodeNormalId::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::toJsonOptionalLikely(j, "key", key);
        JsonUtil::toJson(j, "ignoreError", ignoreError);
        if (HEDLEY_UNLIKELY(!key.has_value())) {
            JsonUtil::toJson(j, "contents", *contents);
        }
    }

    ASTNode NodeNormalId::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        DEBUG_GET_NODE_BEGIN(this)
        ASTNode result = getNormalIdASTNode(this, tokenReader);
        DEBUG_GET_NODE_END(this)
        if (HEDLEY_UNLIKELY(allowsMissingID)) {
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
            VectorView <Token> tokens = result.tokens;
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::incomplete(
                    tokens, "命令不完整"));
        }
        if (HEDLEY_UNLIKELY(!ignoreError)) {
            VectorView <Token> tokens = result.tokens;
            std::string str = TokenUtil::toString(tokens);
            size_t strHash = std::hash<std::string>{}(str);
            if (HEDLEY_UNLIKELY(std::all_of(contents->begin(), contents->end(), [&strHash](const auto &item) {
                return !item->fastMatch(strHash);
            }))) {
                return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::incomplete(
                        tokens, "找不到含义 -> " + std::move(str)));
            }
        }
        return result;
    }

    bool NodeNormalId::collectIdError(const ASTNode *astNode,
                                      std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->isError())) {
            return true;
        }
        std::string str = TokenUtil::toString(astNode->tokens);
        size_t strHash = std::hash<std::string>{}(str);
        if (HEDLEY_UNLIKELY(std::all_of(contents->begin(), contents->end(), [&strHash](const auto &item) {
            return !item->fastMatch(strHash);
        }))) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("找不到ID -> ").append(str)));
        }
        return true;
    }

    bool NodeNormalId::collectSuggestions(const ASTNode *astNode,
                                          size_t index,
                                          std::vector<Suggestions> &suggestions) const {
        std::string str = TokenUtil::toString(astNode->tokens)
                .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));
        std::vector<std::shared_ptr<NormalId>> nameStartOf, nameContain, descriptionContain;
        for (const auto &item: *contents) {
            //通过名字进行搜索
            size_t index1 = item->name.find(str);
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
                    item->description.has_value() && item->description.value().find(str) != std::string::npos)) {
                descriptionContain.push_back(item);
            }
        }
        Suggestions suggestions1;
        suggestions1.suggestions.reserve(nameStartOf.size() + nameContain.size() + descriptionContain.size());
        size_t start = TokenUtil::getStartIndex(astNode->tokens);
        size_t end = TokenUtil::getEndIndex(astNode->tokens);
        std::transform(nameStartOf.begin(), nameStartOf.end(),
                       std::back_inserter(suggestions1.suggestions),
                       [&start, &end](const auto &item) {
                           return Suggestion(start, end, item);
                       });
        std::transform(nameContain.begin(), nameContain.end(),
                       std::back_inserter(suggestions1.suggestions),
                       [&start, &end](const auto &item) {
                           return Suggestion(start, end, item);
                       });
        std::transform(descriptionContain.begin(), descriptionContain.end(),
                       std::back_inserter(suggestions1.suggestions),
                       [&start, &end](const auto &item) {
                           return Suggestion(start, end, item);
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

} // CHelper::Node