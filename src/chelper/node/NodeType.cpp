//
// Created by Yancey on 2023/12/10.
//

#include "NodeType.h"

#include "param/NodeBlock.h"
#include "param/NodeBoolean.h"
#include "param/NodeCommand.h"
#include "param/NodeCommandName.h"
#include "param/NodeFloat.h"
#include "param/NodeInteger.h"
#include "param/NodeItem.h"
#include "param/NodeJson.h"
#include "param/NodeLF.h"
#include "param/NodeNormalId.h"
#include "param/NodePosition.h"
#include "param/NodeRange.h"
#include "param/NodeRelativeFloat.h"
#include "param/NodeRepeat.h"
#include "param/NodeString.h"
#include "param/NodeTargetSelector.h"
#include "param/NodeText.h"
#include "param/NodeXpInteger.h"
#include "util/NodeAny.h"
#include "util/NodeEntry.h"
#include "util/NodeSingleSymbol.h"
#include "json/NodeJsonBoolean.h"
#include "json/NodeJsonFloat.h"
#include "json/NodeJsonInteger.h"
#include "json/NodeJsonList.h"
#include "json/NodeJsonNull.h"
#include "json/NodeJsonObject.h"
#include "json/NodeJsonString.h"

namespace CHelper::Node {

    NodeType::NodeType(std::string nodeName,
                       std::function<void(const nlohmann::json &j, std::unique_ptr<NodeBase> &t)> decodeByJson,
                       std::function<void(nlohmann::json &j, const std::unique_ptr<NodeBase> &t)> encodeByJson,
                       std::function<void(BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t)> decodeByBinary,
                       std::function<void(BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t)> encodeByBinary)
        : nodeName(std::move(nodeName)),
          decodeByJson(std::move(decodeByJson)),
          encodeByJson(std::move(encodeByJson)),
          decodeByBinary(std::move(decodeByBinary)),
          encodeByBinary(std::move(encodeByBinary)) {}

    template<class T>
    static std::unique_ptr<NodeType> createParam(const std::string &nodeName,
                                                 bool isMustAfterWhiteSpace = true,
                                                 const std::vector<NodeCreateStage::NodeCreateStage> &nodeCreateStage =
                                                         {NodeCreateStage::JSON_NODE, NodeCreateStage::REPEAT_NODE, NodeCreateStage::COMMAND_PARAM_NODE}) {
        return std::make_unique<NodeType>(
                nodeName,
                [nodeName, isMustAfterWhiteSpace, nodeCreateStage](const nlohmann::json &j, std::unique_ptr<NodeBase> &t) {
                    if (std::find(nodeCreateStage.begin(), nodeCreateStage.end(), NodeType::currentCreateStage) == nodeCreateStage.end()) {
                        throw std::runtime_error("unknown node type -> " + nodeName);
                    }
                    t = std::make_unique<T>();
                    j.get_to((T &) *t);
                    if (HEDLEY_UNLIKELY(!t->isMustAfterWhiteSpace.has_value())) {
                        t->isMustAfterWhiteSpace = isMustAfterWhiteSpace;
                    }
                },
                [](nlohmann::json &j, const std::unique_ptr<NodeBase> &t) {
                    j = (T &) *t;
                },
                [nodeName, isMustAfterWhiteSpace, nodeCreateStage](BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t) {
                    if (std::find(nodeCreateStage.begin(), nodeCreateStage.end(), NodeType::currentCreateStage) == nodeCreateStage.end()) {
                        throw std::runtime_error("unknown node type -> " + nodeName);
                    }
                    t = std::make_unique<T>();
                    binaryReader.decode((T &) *t);
                    if (HEDLEY_UNLIKELY(!t->isMustAfterWhiteSpace.has_value())) {
                        t->isMustAfterWhiteSpace = isMustAfterWhiteSpace;
                    }
                },
                [](BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t) {
                    binaryWriter.encode((T &) *t);
                });
    }

    template<class T>
    static std::unique_ptr<NodeType> createJson(const std::string &nodeName) {
        return createParam<T>(nodeName, false, {NodeCreateStage::JSON_NODE});
    }

    template<class T>
    static std::unique_ptr<NodeType> createNone(const std::string &nodeName) {
        return std::make_unique<NodeType>(
                nodeName,
                [nodeName](const nlohmann::json &j, std::unique_ptr<NodeBase> &t) {
                    throw std::runtime_error("unknown node type -> " + nodeName);
                },
                [](nlohmann::json &j, const std::unique_ptr<NodeBase> &t) {
                    j = (T &) *t;
                },
                [nodeName](BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t) {
                    throw std::runtime_error("unknown node type -> " + nodeName);
                },
                [](BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t) {
                    binaryWriter.encode((T &) *t);
                });
    }

    std::vector<NodeType *> NodeType::NODE_TYPES;

    NodeCreateStage::NodeCreateStage NodeType::currentCreateStage = NodeCreateStage::NONE;

    std::unique_ptr<NodeType> NodeType::BLOCK = createParam<NodeBlock>("BLOCK");
    std::unique_ptr<NodeType> NodeType::BOOLEAN = createParam<NodeBoolean>("BOOLEAN");
    std::unique_ptr<NodeType> NodeType::COMMAND = createParam<NodeCommand>("COMMAND");
    std::unique_ptr<NodeType> NodeType::COMMAND_NAME = createParam<NodeCommandName>("COMMAND_NAME");
    std::unique_ptr<NodeType> NodeType::FLOAT = createParam<NodeFloat>("FLOAT");
    std::unique_ptr<NodeType> NodeType::INTEGER = createParam<NodeInteger>("INTEGER");
    std::unique_ptr<NodeType> NodeType::ITEM = createParam<NodeItem>("ITEM");
    std::unique_ptr<NodeType> NodeType::LF = createNone<NodeLF>("LF");
    std::unique_ptr<NodeType> NodeType::NAMESPACE_ID = createParam<NodeNamespaceId>("NAMESPACE_ID");
    std::unique_ptr<NodeType> NodeType::NORMAL_ID = createParam<NodeNormalId>("NORMAL_ID");
    std::unique_ptr<NodeType> NodeType::PER_COMMAND = createNone<NodePerCommand>("PER_COMMAND");
    std::unique_ptr<NodeType> NodeType::POSITION = createParam<NodePosition>("POSITION", false);
    std::unique_ptr<NodeType> NodeType::RELATIVE_FLOAT = createParam<NodeRelativeFloat>("RELATIVE_FLOAT", false);
    std::unique_ptr<NodeType> NodeType::REPEAT = createParam<NodeRepeat>("REPEAT", false,
                                                                         {NodeCreateStage::JSON_NODE, NodeCreateStage::COMMAND_PARAM_NODE});
    std::unique_ptr<NodeType> NodeType::STRING = createParam<NodeString>("STRING");
    std::unique_ptr<NodeType> NodeType::TARGET_SELECTOR = createParam<NodeTargetSelector>("TARGET_SELECTOR");
    std::unique_ptr<NodeType> NodeType::TEXT = createParam<NodeText>("TEXT");
    std::unique_ptr<NodeType> NodeType::RANGE = createParam<NodeRange>("RANGE");
    std::unique_ptr<NodeType> NodeType::XP_INTEGER = createParam<NodeXpInteger>("XP_INTEGER");
    std::unique_ptr<NodeType> NodeType::JSON = createParam<NodeJson>("JSON", true,
                                                                     {NodeCreateStage::REPEAT_NODE, NodeCreateStage::COMMAND_PARAM_NODE});
    std::unique_ptr<NodeType> NodeType::JSON_BOOLEAN = createJson<NodeJsonBoolean>("JSON_BOOLEAN");
    std::unique_ptr<NodeType> NodeType::JSON_ELEMENT = createNone<NodeJsonElement>("JSON_ELEMENT");
    std::unique_ptr<NodeType> NodeType::JSON_ENTRY = createNone<NodeJsonEntry>("JSON_ENTRY");
    std::unique_ptr<NodeType> NodeType::JSON_FLOAT = createJson<NodeJsonFloat>("JSON_FLOAT");
    std::unique_ptr<NodeType> NodeType::JSON_INTEGER = createJson<NodeJsonInteger>("JSON_INTEGER");
    std::unique_ptr<NodeType> NodeType::JSON_LIST = createJson<NodeJsonList>("JSON_LIST");
    std::unique_ptr<NodeType> NodeType::JSON_NULL = createJson<NodeJsonNull>("JSON_NULL");
    std::unique_ptr<NodeType> NodeType::JSON_OBJECT = createJson<NodeJsonObject>("JSON_OBJECT");
    std::unique_ptr<NodeType> NodeType::JSON_STRING = createJson<NodeJsonString>("JSON_STRING");
    std::unique_ptr<NodeType> NodeType::AND = createNone<NodeAnd>("AND");
    std::unique_ptr<NodeType> NodeType::ANY = createNone<NodeAny>("ANY");
    std::unique_ptr<NodeType> NodeType::ENTRY = createNone<NodeEntry>("ENTRY");
    std::unique_ptr<NodeType> NodeType::EQUAL_ENTRY = createNone<NodeEqualEntry>("EQUAL_ENTRY");
    std::unique_ptr<NodeType> NodeType::LIST = createNone<NodeList>("LIST");
    std::unique_ptr<NodeType> NodeType::OR = createNone<NodeOr>("OR");
    std::unique_ptr<NodeType> NodeType::SINGLE_SYMBOL = createNone<NodeSingleSymbol>("SINGLE_SYMBOL");

    void registerNodeType(const std::unique_ptr<NodeType> &nodeType) {
        NodeType::NODE_TYPES.push_back(nodeType.get());
    }

    void NodeType::init() {
        static bool isInit = false;
        if (HEDLEY_UNLIKELY(isInit)) {
            return;
        }
        isInit = true;
        registerNodeType(BLOCK);
        registerNodeType(BOOLEAN);
        registerNodeType(COMMAND);
        registerNodeType(COMMAND_NAME);
        registerNodeType(FLOAT);
        registerNodeType(INTEGER);
        registerNodeType(ITEM);
        registerNodeType(LF);
        registerNodeType(NAMESPACE_ID);
        registerNodeType(NORMAL_ID);
        registerNodeType(PER_COMMAND);
        registerNodeType(POSITION);
        registerNodeType(RELATIVE_FLOAT);
        registerNodeType(REPEAT);
        registerNodeType(STRING);
        registerNodeType(TARGET_SELECTOR);
        registerNodeType(TEXT);
        registerNodeType(RANGE);
        registerNodeType(XP_INTEGER);
        registerNodeType(JSON);
        registerNodeType(JSON_BOOLEAN);
        registerNodeType(JSON_ELEMENT);
        registerNodeType(JSON_ENTRY);
        registerNodeType(JSON_FLOAT);
        registerNodeType(JSON_INTEGER);
        registerNodeType(JSON_LIST);
        registerNodeType(JSON_NULL);
        registerNodeType(JSON_OBJECT);
        registerNodeType(JSON_STRING);
        size_t size = NodeType::NODE_TYPES.size();
        for (int i = 0; i < size; ++i) {
            NodeType::NODE_TYPES[i]->id = i;
        }
    }

}// namespace CHelper::Node