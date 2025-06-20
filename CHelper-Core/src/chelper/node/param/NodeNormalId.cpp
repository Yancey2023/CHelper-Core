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

}// namespace CHelper::Node