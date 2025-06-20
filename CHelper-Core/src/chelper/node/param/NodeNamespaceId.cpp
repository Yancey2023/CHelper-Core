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

}// namespace CHelper::Node