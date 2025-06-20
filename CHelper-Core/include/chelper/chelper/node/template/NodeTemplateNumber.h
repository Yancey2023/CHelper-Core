//
// Created by Yancey on 2024-08-20.
//

#ifndef CHELPER_NODETEMPLATENUMBER_H
#define CHELPER_NODETEMPLATENUMBER_H

#include "../NodeBase.h"

namespace CHelper::Node {

    template<class T, bool isJson>
    class NodeTemplateNumber : public NodeSerializable {
    public:
        std::optional<T> min, max;

        NodeTemplateNumber() = default;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override {
            if constexpr (std::is_same<T, int32_t>() && !isJson) {
                return NodeTypeId::INTEGER;
            } else if constexpr (std::is_same<T, float>() && !isJson) {
                return NodeTypeId::FLOAT;
            } else if constexpr (std::is_same<T, int32_t>() && isJson) {
                return NodeTypeId::JSON_INTEGER;
            } else if constexpr (std::is_same<T, float>() && isJson) {
                return NodeTypeId::JSON_FLOAT;
            }
        }

        static auto str2number(const std::string &str, char *&end) {
            if constexpr (std::numeric_limits<T>::is_integer) {
                return std::strtoimax(str.c_str(), &end, 10);
            } else if constexpr (std::is_same<T, float>()) {
                return std::strtof(str.c_str(), &end);
            } else if constexpr (std::is_same<T, double>()) {
                return std::strtod(str.c_str(), &end);
            } else if constexpr (std::is_same<T, long double>()) {
                return std::strtold(str.c_str(), &end);
            }
        }

        static std::unique_ptr<NodeTemplateNumber<T, isJson>> make(const std::optional<std::string> &id,
                                                                   const std::optional<std::u16string> &description,
                                                                   const std::optional<T> &min0,
                                                                   const std::optional<T> &max0) {
            auto result = std::make_unique<NodeTemplateNumber<T, isJson>>();
            result->id = id;
            result->description = description;
            result->isMustAfterSpace = false;
            result->min = min0;
            result->max = max0;
            return std::move(result);
        }
    };

    using NodeFloat = NodeTemplateNumber<float, false>;
    using NodeInteger = NodeTemplateNumber<int32_t, false>;
    using NodeJsonFloat = NodeTemplateNumber<float, true>;
    using NodeJsonInteger = NodeTemplateNumber<int32_t, true>;

}// namespace CHelper::Node

CODEC_NODE(CHelper::Node::NodeFloat, min, max)
CODEC_NODE(CHelper::Node::NodeInteger, min, max)
CODEC_NODE(CHelper::Node::NodeJsonInteger, min, max)
CODEC_NODE(CHelper::Node::NodeJsonFloat, min, max)

#endif//CHELPER_NODETEMPLATENUMBER_H
