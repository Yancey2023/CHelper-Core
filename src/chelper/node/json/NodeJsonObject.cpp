//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonObject.h"
#include "../util/NodeSingleSymbol.h"

namespace CHelper::Node {

    static std::unique_ptr<NodeOr> getNodeElement1(const std::vector<std::unique_ptr<NodeBase>> &data) {
        if (HEDLEY_UNLIKELY(data.empty())) {
            return nullptr;
        }
        std::vector<const NodeBase *> nodeElementData;
        nodeElementData.reserve(data.size());
        for (const auto &item: data) {
            nodeElementData.push_back(item.get());
        }
        return std::make_unique<NodeOr>("JSON_OBJECT_ELEMENT", "JSON对象键值对", std::move(nodeElementData), false);
    }

    static NodeOr getNodeElement2(const std::unique_ptr<NodeOr> &nodeElement1) {
        std::vector<const NodeBase *> nodeElementData;
        if (HEDLEY_LIKELY(nodeElement1 != nullptr)) {
            nodeElementData.reserve(2);
            nodeElementData.push_back(nodeElement1.get());
        }
        nodeElementData.push_back(NodeJsonEntry::getNodeJsonAllEntry());
        return {"JSON_OBJECT_ELEMENT", "JSON对象键值对", std::move(nodeElementData), false, true};
    }

    static NodeList getNodeList(const NodeOr &nodeElement2) {
        static std::unique_ptr<NodeBase> nodLeft = std::make_unique<NodeSingleSymbol>(
                "JSON_LIST_LEFT", "JSON列表左括号", '{');
        static std::unique_ptr<NodeBase> nodeRight = std::make_unique<NodeSingleSymbol>(
                "JSON_LIST_RIGHT", "JSON列表右括号", '}');
        static std::unique_ptr<NodeBase> nodeSeparator = std::make_unique<NodeSingleSymbol>(
                "JSON_LIST_SEPARATOR", "JSON列表分隔符", ',');
        return {"JSON_OBJECT", "JSON对象",
                nodLeft.get(), &nodeElement2,
                nodeSeparator.get(), nodeRight.get()};
    }

    NodeJsonObject::NodeJsonObject(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description,
                                   std::vector<std::unique_ptr<NodeBase>> data)
        : NodeBase(id, description, false),
          data(std::move(data)),
          nodeElement1(getNodeElement1(this->data)),
          nodeElement2(getNodeElement2(nodeElement1)),
          nodeList(getNodeList(nodeElement2)) {}

    static std::vector<std::unique_ptr<NodeBase>> getDataFromJson(const nlohmann::json &j) {
        const auto &jsonData = j.at("data");
        std::vector<std::unique_ptr<NodeBase>> data;
        data.reserve(jsonData.size());
        for (const auto &item: jsonData) {
            data.push_back(std::make_unique<NodeJsonEntry>(item));
        }
        return std::move(data);
    }

    static std::vector<std::unique_ptr<NodeBase>> readDataFromBinary(BinaryReader &binaryReader) {
        size_t size = binaryReader.readSize();
        std::vector<std::unique_ptr<NodeBase>> data;
        data.reserve(size);
        for (int i = 0; i < size; ++i) {
            data.push_back(std::make_unique<NodeJsonEntry>(binaryReader));
        }
        return std::move(data);
    }

    NodeJsonObject::NodeJsonObject(const nlohmann::json &j,
                                   [[maybe_unused]] const CPack &cpack)
        : NodeBase(j, false),
          data(getDataFromJson(j)),
          nodeElement1(getNodeElement1(data)),
          nodeElement2(getNodeElement2(nodeElement1)),
          nodeList(getNodeList(nodeElement2)) {}

    NodeJsonObject::NodeJsonObject(BinaryReader &binaryReader,
                                   [[maybe_unused]] const CPack &cpack)
        : NodeBase(binaryReader),
          data(readDataFromBinary(binaryReader)),
          nodeElement1(getNodeElement1(data)),
          nodeElement2(getNodeElement2(nodeElement1)),
          nodeList(getNodeList(nodeElement2)) {}

    NodeType *NodeJsonObject::getNodeType() const {
        return NodeType::JSON_OBJECT.get();
    }

    void NodeJsonObject::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::encode(j, "data", data);
    }

    void NodeJsonObject::writeBinToFile(BinaryWriter &binaryWriter) const {
        NodeBase::writeBinToFile(binaryWriter);
        binaryWriter.encode(data);
    }

    ASTNode NodeJsonObject::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getByChildNode(tokenReader, cpack, &nodeList);
    }

}// namespace CHelper::Node