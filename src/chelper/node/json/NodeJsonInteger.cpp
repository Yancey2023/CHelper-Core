//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonInteger.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeJsonInteger::NodeJsonInteger(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description,
                                     const std::optional<int32_t> &min,
                                     const std::optional<int32_t> &max)
        : NodeBase(id, description, false),
          min(min),
          max(max) {}

    NodeJsonInteger::NodeJsonInteger(const nlohmann::json &j,
                                     [[maybe_unused]] const CPack &cpack)
        : NodeBase(j, false),
          min(JsonUtil::read<std::optional<int32_t>>(j, "min")),
          max(JsonUtil::read<std::optional<int32_t>>(j, "max")) {}

    NodeJsonInteger::NodeJsonInteger(BinaryReader &binaryReader,
                                     [[maybe_unused]] const CPack &cpack)
        : NodeBase(binaryReader) {
        auto flag = binaryReader.read<uint8_t>();
        if ((flag >> 0) & 0x01) {
            min = binaryReader.read<int32_t>();
        }
        if ((flag >> 1) & 0x01) {
            max = binaryReader.read<int32_t>();
        }
    }

    NodeType *NodeJsonInteger::getNodeType() const {
        return NodeType::JSON_INTEGER.get();
    }

    void NodeJsonInteger::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::encode(j, "min", min);
        JsonUtil::encode(j, "max", max);
    }

    void NodeJsonInteger::writeBinToFile(BinaryWriter &binaryWriter) const {
        NodeBase::writeBinToFile(binaryWriter);
        uint8_t flag = 0x00;
        if (min.has_value()) {
            flag |= (0x01 << 0);
        }
        if (max.has_value()) {
            flag |= (0x01 << 1);
        }
        binaryWriter.encode(flag);
        if (min.has_value()) {
            binaryWriter.encode(min.value());
        }
        if (max.has_value()) {
            binaryWriter.encode(max.value());
        }
    }

    ASTNode NodeJsonInteger::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return tokenReader.readIntegerASTNode(this);
    }

    bool NodeJsonInteger::collectIdError(const ASTNode *astNode,
                                         std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->isError())) {
            return true;
        }
        std::string str = TokenUtil::toString(astNode->tokens);
        char *end;
        std::intmax_t value = std::strtoimax(str.c_str(), &end, 10);
        if (HEDLEY_UNLIKELY(end == str.c_str() || *end != '\0' ||
                            value == HUGE_VALF || value == -HUGE_VALF ||
                            value < min.value_or(std::numeric_limits<int32_t>::lowest()) ||
                            value > max.value_or(std::numeric_limits<int32_t>::max()))) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("数值不在范围")
                                                                                   .append("[")
                                                                                   .append(std::to_string(min.value_or(std::numeric_limits<int32_t>::lowest())))
                                                                                   .append(", ")
                                                                                   .append(std::to_string(max.value_or(std::numeric_limits<int32_t>::max())))
                                                                                   .append("]")
                                                                                   .append("内 -> ")
                                                                                   .append(str)));
        }
        return true;
    }

}// namespace CHelper::Node
