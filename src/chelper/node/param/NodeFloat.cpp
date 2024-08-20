//
// Created by Yancey on 2023/11/11.
//

#include "NodeFloat.h"

namespace CHelper::Node {

    NodeType *NodeFloat::getNodeType() const {
        return NodeType::FLOAT.get();
    }

    std::unique_ptr<NodeFloat> NodeFloat::make(const std::optional<std::u16string> &id,
                                               const std::optional<std::u16string> &description,
                                               const std::optional<float> &min,
                                               const std::optional<float> &max) {
        auto result = std::make_unique<NodeFloat>();
        result->id = id;
        result->description = description;
        result->isMustAfterWhiteSpace = false;
        result->min = min;
        result->max = max;
        return std::move(result);
    }

    CODEC_NODE(NodeFloat, min, max)

}// namespace CHelper::Node
