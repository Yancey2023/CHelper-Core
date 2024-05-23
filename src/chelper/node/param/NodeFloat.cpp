//
// Created by Yancey on 2023/11/11.
//

#include "NodeFloat.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeFloat::NodeFloat(const std::optional<std::string> &id,
                         const std::optional<std::string> &description,
                         const std::optional<float> &min,
                         const std::optional<float> &max)
        : NodeBase(id, description, false),
          min(min),
          max(max) {}

    NodeFloat::NodeFloat(const nlohmann::json &j,
                         [[maybe_unused]] const CPack &cpack)
        : NodeBase(j, true),
          min(JsonUtil::read<std::optional<float>>(j, "min")),
          max(JsonUtil::read<std::optional<float>>(j, "max")) {}

    NodeFloat::NodeFloat(BinaryReader &binaryReader,
                         [[maybe_unused]] const CPack &cpack)
        : NodeBase(binaryReader) {
        auto flag = binaryReader.read<uint8_t>();
        if ((flag >> 0) & 0x01) {
            min = binaryReader.read<float>();
        }
        if ((flag >> 1) & 0x01) {
            max = binaryReader.read<float>();
        }
    }

    NodeType *NodeFloat::getNodeType() const {
        return NodeType::FLOAT.get();
    }

    void NodeFloat::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::encode(j, "min", min);
        JsonUtil::encode(j, "max", max);
    }

    void NodeFloat::writeBinToFile(BinaryWriter &binaryWriter) const {
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

    ASTNode NodeFloat::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return tokenReader.readFloatASTNode(this);
    }

    bool NodeFloat::collectIdError(const ASTNode *astNode,
                                   std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->isError())) {
            return true;
        }
        std::string str = TokenUtil::toString(astNode->tokens);
        char *end;
        float value = std::strtof(str.c_str(), &end);
        if (HEDLEY_UNLIKELY(end == str.c_str() || *end != '\0' ||
                            value == HUGE_VALF || value == -HUGE_VALF ||
                            (min.has_value() && value < min) ||
                            (max.has_value() && value > max))) {
            idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("数值不在范围")
                                                                                   .append("[")
                                                                                   .append(std::to_string(min.value_or(std::numeric_limits<float>::lowest())))
                                                                                   .append(", ")
                                                                                   .append(std::to_string(max.value_or(std::numeric_limits<float>::max())))
                                                                                   .append("]")
                                                                                   .append("内 -> ")
                                                                                   .append(str)));
        }
        return true;
    }

    void NodeFloat::collectStructure(const ASTNode *astNode,
                                     StructureBuilder &structure,
                                     bool isMustHave) const {
        structure.append(isMustHave, description.value_or("数字"));
    }

}// namespace CHelper::Node
