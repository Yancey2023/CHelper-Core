//
// Created by Yancey on 2024/2/28.
//

#include <chelper/node/json/NodeJsonInteger.h>

namespace CHelper::Node {

    NodeType *NodeJsonInteger::getNodeType() const {
        return NodeType::JSON_INTEGER.get();
    }

    std::unique_ptr<NodeJsonInteger> NodeJsonInteger::make(const std::optional<std::u16string> &id,
                                                           const std::optional<std::u16string> &description,
                                                           const std::optional<int32_t> &min,
                                                           const std::optional<int32_t> &max) {
        auto result = std::make_unique<NodeJsonInteger>();
        result->id = id;
        result->description = description;
        result->isMustAfterWhiteSpace = false;
        result->min = min;
        result->max = max;
        return std::move(result);
    }

    CODEC_NODE(NodeJsonInteger, min, max)

}// namespace CHelper::Node
