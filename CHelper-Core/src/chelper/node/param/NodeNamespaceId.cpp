//
// Created by Yancey on 2023/11/12.
//

#include <chelper/node/NodeType.h>
#include <chelper/node/param/NodeNamespaceId.h>

namespace CHelper::Node {

    NodeNamespaceId::NodeNamespaceId(const std::optional<std::string> &id,
                                     const std::optional<std::u16string> &description,
                                     const std::optional<std::string> &key,
                                     bool ignoreError)
        : NodeSerializable(id, description, false),
          key(key),
          ignoreError(ignoreError) {}

    void NodeNamespaceId::init(const CPack &cpack) {
        if (HEDLEY_LIKELY(contents.has_value())) {
            customContents = contents.value();
        } else if (HEDLEY_LIKELY(key.has_value())) {
            customContents = cpack.getNamespaceId(key.value());
        }
        if (HEDLEY_UNLIKELY(customContents == nullptr)) {
            if (HEDLEY_UNLIKELY(key.has_value())) {
                Profile::push("linking contents to {}", FORMAT_ARG(key.value()));
                Profile::push("failed to find namespace id in the cpack -> {}", FORMAT_ARG(key.value()));
                throw std::runtime_error("failed to find namespace id");
            } else {
                throw std::runtime_error("missing content");
            }
        }
    }

    NodeTypeId::NodeTypeId NodeNamespaceId::getNodeType() const {
        return NodeTypeId::NAMESPACE_ID;
    }

    ASTNode NodeNamespaceId::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        // namespace:id
        // 字符串中已经包含冒号，因为冒号不是结束字符
        DEBUG_GET_NODE_BEGIN(this)
        auto result = tokenReader.readStringASTNode(this);
        DEBUG_GET_NODE_END(this)
        if (HEDLEY_UNLIKELY(result.tokens.isEmpty())) {
            TokensView tokens = result.tokens;
            return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::incomplete(tokens, u"命令不完整"));
        }
        if (HEDLEY_UNLIKELY(!ignoreError.value_or(false))) {
            TokensView tokens = result.tokens;
            std::u16string_view str = tokens.toString();
            XXH64_hash_t strHash = XXH3_64bits(str.data(), str.size() * sizeof(decltype(str)::value_type));
            if (HEDLEY_UNLIKELY(std::all_of(customContents->begin(), customContents->end(), [&strHash](const auto &item) {
                    return !item->fastMatch(strHash) && !item->getIdWithNamespace()->fastMatch(strHash);
                }))) {
                return ASTNode::andNode(this, {std::move(result)}, tokens, ErrorReason::incomplete(tokens, fmt::format(u"找不到含义 -> {}", str)));
            }
        }
        return result;
    }

}// namespace CHelper::Node