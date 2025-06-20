//
// Created by Yancey on 2023/11/11.
//

#include <chelper/node/NodeType.h>
#include <chelper/node/param/NodeNormalId.h>

namespace CHelper::Node {

    NodeNormalId::NodeNormalId(
            const std::optional<std::string> &id,
            const std::optional<std::u16string> &description,
            const std::string &key,
            bool ignoreError,
            bool allowMissingID,
            const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getNormalIdASTNode)
        : NodeSerializable(id, description, false),
          key(key),
          ignoreError(ignoreError),
          allowMissingID(allowMissingID),
          getNormalIdASTNode(getNormalIdASTNode) {}

    NodeNormalId::NodeNormalId(
            const std::optional<std::string> &id,
            const std::optional<std::u16string> &description,
            const std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> &contents,
            bool ignoreError,
            bool allowMissingID,
            const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getNormalIdASTNode)
        : NodeSerializable(id, description, false),
          contents(contents),
          ignoreError(ignoreError),
          allowMissingID(allowMissingID),
          getNormalIdASTNode(getNormalIdASTNode) {
#ifdef CHelperDebug
        if (HEDLEY_UNLIKELY(contents == nullptr)) {
            throw std::runtime_error("contents should not be nullptr");
        }
#endif
        customContents = contents;
    }

    void NodeNormalId::init(const CPack &cpack) {
        if (HEDLEY_UNLIKELY(getNormalIdASTNode == nullptr)) {
            getNormalIdASTNode = [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readUntilSpace(node);
            };
        }
        if (HEDLEY_LIKELY(contents.has_value())) {
            customContents = contents.value();
        } else if (HEDLEY_LIKELY(key.has_value())) {
            customContents = cpack.getNormalId(key.value());
        }
        if (HEDLEY_UNLIKELY(customContents == nullptr)) {
            if (HEDLEY_LIKELY(key.has_value())) {
                Profile::push("linking contents to {}", FORMAT_ARG(key.value()));
                Profile::push("failed to find normal id in the cpack -> ", FORMAT_ARG(key.value()));
                throw std::runtime_error("failed to find normal id");
            } else {
                throw std::runtime_error("missing content");
            }
        }
    }

    NodeTypeId::NodeTypeId NodeNormalId::getNodeType() const {
        return NodeTypeId::NORMAL_ID;
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
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::incomplete(tokens, u"命令不完整"));
        }
        if (HEDLEY_UNLIKELY(!ignoreError.value_or(true))) {
            TokensView tokens = result.tokens;
            std::u16string_view str = tokens.toString();
            XXH64_hash_t strHash = XXH3_64bits(str.data(), str.size() * sizeof(decltype(str)::value_type));
            if (HEDLEY_UNLIKELY(std::all_of(customContents->begin(), customContents->end(), [&strHash](const auto &item) {
                    return !item->fastMatch(strHash);
                }))) {
                return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::incomplete(tokens, fmt::format(u"找不到含义 -> {}", str)));
            }
        }
        return result;
    }

    bool NodeNormalId::collectIdError(const ASTNode *astNode,
                                      std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->isError())) {
            return true;
        }
        std::u16string_view str = astNode->tokens.toString();
        XXH64_hash_t strHash = XXH3_64bits(str.data(), str.size() * sizeof(decltype(str)::value_type));
        if (HEDLEY_UNLIKELY(std::all_of(customContents->begin(), customContents->end(), [&strHash](const auto &item) {
                return !item->fastMatch(strHash);
            }))) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, fmt::format(u"找不到ID -> {}", str)));
        }
        return true;
    }

    void NodeNormalId::collectStructure(const ASTNode *astNode,
                                        StructureBuilder &structure,
                                        bool isMustHave) const {
        structure.append(isMustHave, description.value_or(u"ID"));
    }

}// namespace CHelper::Node