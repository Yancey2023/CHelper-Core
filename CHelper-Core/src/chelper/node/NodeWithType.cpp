//
// Created by Yancey on 2025-06-21.
//

#include <chelper/node/NodeType.h>
#include <chelper/node/NodeWithType.h>

#define CHELPER_DELETE(v1)                                                           \
    case Node::NodeTypeId::v1:                                                       \
        delete static_cast<NodeTypeDetail<Node::NodeTypeId::v1>::Type *>(item.data); \
        break;

namespace CHelper::Node {

#ifdef CHelperDebug
    NodeWithType::~NodeWithType() {
        if (data != nullptr) {
            --data->times;
            data = nullptr;
        }
    }

    NodeWithType::NodeWithType(NodeWithType &node)
        : nodeTypeId(node.nodeTypeId),
          data(node.data) {
        if (data != nullptr) {
            ++data->times;
        }
        if (data != nullptr && nodeTypeId > MAX_TYPE_ID) {
            throw std::runtime_error("invalid node type id");
        }
    }

    NodeWithType::NodeWithType(const NodeWithType &node)
        : nodeTypeId(node.nodeTypeId),
          data(node.data) {
        if (data != nullptr) {
            ++data->times;
        }
        if (data != nullptr && nodeTypeId > MAX_TYPE_ID) {
            throw std::runtime_error("invalid node type id");
        }
    }

    NodeWithType::NodeWithType(NodeWithType &&node)// NOLINT(*-noexcept-move-constructor)
        : nodeTypeId(node.nodeTypeId),
          data(node.data) {
        node.data = nullptr;
        if (data != nullptr && nodeTypeId > MAX_TYPE_ID) {
            throw std::runtime_error("invalid node type id");
        }
    }

    NodeWithType &NodeWithType::operator=(const NodeWithType &node) {// NOLINT(*-unhandled-self-assignment)
        nodeTypeId = node.nodeTypeId;
        data = node.data;
        if (data != nullptr) {
            ++data->times;
        }
        if (data != nullptr && nodeTypeId > MAX_TYPE_ID) {
            throw std::runtime_error("invalid node type id");
        }
        return *this;
    }

    NodeWithType &NodeWithType::operator=(NodeWithType &&node) {// NOLINT(*-noexcept-move-constructor)
        nodeTypeId = node.nodeTypeId;
        data = node.data;
        node.data = nullptr;
        if (data != nullptr && nodeTypeId > MAX_TYPE_ID) {
            throw std::runtime_error("invalid node type id");
        }
        return *this;
    }
#endif

    FreeableNodeWithTypes::~FreeableNodeWithTypes() {
#ifdef CHelperDebug
        for (int64_t i = static_cast<int64_t>(nodes.size()) - 1; i >= 0; --i) {
            auto &item = nodes[i];
#else
        for (auto &item: nodes) {
#endif
            if (item.data == nullptr) {
                continue;
            }
#ifdef CHelperDebug
            --item.data->times;
#endif
            switch (item.nodeTypeId) {
                CODEC_PASTE(CHELPER_DELETE, CHELPER_NODE_TYPES);
            }
            item.data = nullptr;
        }
    }

}// namespace CHelper::Node
