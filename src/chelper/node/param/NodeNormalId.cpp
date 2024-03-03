//
// Created by Yancey on 2023/11/11.
//

#include "NodeNormalId.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeNormalId::NodeNormalId(const std::optional<std::string> &id,
                               const std::optional<std::string> &description,
                               const std::optional<std::string> &key,
                               const std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> &contents,
                               bool allowsMissingID,
                               ASTNode(*getNormalIdASTNode)(const NodeBase *node, TokenReader &tokenReader))
            : NodeBase(id, description, false),
              key(key),
              contents(contents),
              allowsMissingID(allowsMissingID),
              getNormalIdASTNode(getNormalIdASTNode) {}

    static std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> getIdContentFromCPack(const nlohmann::json &j,
                                                                                         const CPack &cpack,
                                                                                         const std::optional<std::string> &key) {
        if (key.has_value()) {
            auto it = cpack.normalIds.find(key.value());
            if (it == cpack.normalIds.end()) {
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
              key(FROM_JSON_OPTIONAL(j, key, std::string)),
              contents(getIdContentFromCPack(j, cpack, key)),
              allowsMissingID(false),
              getNormalIdASTNode([](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                  return tokenReader.readStringASTNode(node);
              }) {}

    std::shared_ptr<NodeType> NodeNormalId::getNodeType() const {
        return NodeType::NORMAL_ID;
    }

    void NodeNormalId::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON_OPTIONAL(j, key)
        if (!key.has_value()) {
            nlohmann::json content;
            for (const auto &item: *contents) {
                content.push_back(*item);
            }
            j["contents"] = content;
        }
    }

    ASTNode NodeNormalId::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        DEBUG_GET_NODE_BEGIN(this)
        auto result = getNormalIdASTNode(this, tokenReader);
        DEBUG_GET_NODE_END(this)
        if (allowsMissingID) {
            if (result.isError()) {
                tokenReader.restore();
                tokenReader.push();
                return ASTNode::simpleNode(this, tokenReader.collect());
            }
        } else if (result.tokens.isEmpty()) {
            tokenReader.pop();
            return ASTNode::andNode(this, {result}, result.tokens, ErrorReason::incomplete(
                    result.tokens, "ID为空"));
        }
        tokenReader.pop();
        return result;
    }

    bool NodeNormalId::collectIdError(const ASTNode *astNode,
                                      std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = TokenUtil::toString(astNode->tokens);
        for (const auto &item: *contents) {
            if (str == item->name) {
                return true;
            }
        }
        idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("找不到ID -> ").append(str)));
        return true;
    }

    bool NodeNormalId::collectSuggestions(const ASTNode *astNode,
                                          size_t index,
                                          std::vector<Suggestion> &suggestions) const {
        std::string str = TokenUtil::toString(astNode->tokens)
                .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));
        std::vector<std::shared_ptr<NormalId>> nameStartOf, nameContain, descriptionContain;
        for (const auto &item: *contents) {
            //通过名字进行搜索
            size_t index1 = item->name.find(str);
            if (index1 != std::string::npos) {
                if (index1 == 0) {
                    nameStartOf.push_back(item);
                } else {
                    nameContain.push_back(item);
                }
                continue;
            }
            //通过介绍进行搜索
            if (item->description.has_value()) {
                size_t index2 = item->description.value().find(str);
                if (index2 != std::string::npos) {
                    descriptionContain.push_back(item);
                }
            }
        }
        for (const auto &item: nameStartOf) {
            suggestions.emplace_back(astNode->tokens, item);
        }
        for (const auto &item: nameContain) {
            suggestions.emplace_back(astNode->tokens, item);
        }
        for (const auto &item: descriptionContain) {
            suggestions.emplace_back(astNode->tokens, item);
        }
        return true;
    }

    void NodeNormalId::collectStructure(const ASTNode *astNode,
                                        StructureBuilder &structure,
                                        bool isMustHave) const {
        structure.append(isMustHave, description.value_or("ID"));
    }

} // CHelper::Node