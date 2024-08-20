//
// Created by Yancey on 2024/2/28.
//

#include <chelper/node/json/NodeJsonFloat.h>

namespace CHelper::Node {

    NodeType *NodeJsonFloat::getNodeType() const {
        return NodeType::JSON_FLOAT.get();
    }

    std::unique_ptr<NodeJsonFloat> NodeJsonFloat::make(const std::optional<std::u16string> &id,
                                                       const std::optional<std::u16string> &description,
                                                       const std::optional<float> &min,
                                                       const std::optional<float> &max) {
        auto result = std::make_unique<NodeJsonFloat>();
        result->id = id;
        result->description = description;
        result->isMustAfterWhiteSpace = false;
        result->min = min;
        result->max = max;
        return std::move(result);
    }

    CODEC_NODE(NodeJsonFloat, min, max)

}// namespace CHelper::Node
