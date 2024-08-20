//
// Created by Yancey on 2023/11/11.
//

#include "NodeInteger.h"

namespace CHelper::Node {

    NodeType *NodeInteger::getNodeType() const {
        return NodeType::INTEGER.get();
    }

    std::unique_ptr<NodeInteger> NodeInteger::make(const std::optional<std::u16string> &id,
                                                   const std::optional<std::u16string> &description,
                                                   const std::optional<int32_t> &min,
                                                   const std::optional<int32_t> &max) {
        auto result = std::make_unique<NodeInteger>();
        result->id = id;
        result->description = description;
        result->isMustAfterWhiteSpace = false;
        result->min = min;
        result->max = max;
        return std::move(result);
    }

    CODEC_NODE(NodeInteger, min, max)

}// namespace CHelper::Node
