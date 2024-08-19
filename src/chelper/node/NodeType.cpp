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

#if CHelperOnlyReadBinary == true
    NodeType::NodeType(std::wstring nodeName,
                       std::function<void(BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t)> decodeByBinary)
        : nodeName(std::move(nodeName)),
          decodeByBinary(std::move(decodeByBinary)) {}
#else
    NodeType::NodeType(std::wstring nodeName,
                       std::function<void(const nlohmann::json &j, std::unique_ptr<NodeBase> &t)> decodeByJson,
                       std::function<void(nlohmann::json &j, const std::unique_ptr<NodeBase> &t)> encodeByJson,
                       std::function<void(BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t)> decodeByBinary,
                       std::function<void(BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t)> encodeByBinary)
        : nodeName(std::move(nodeName)),
          decodeByJson(std::move(decodeByJson)),
          encodeByJson(std::move(encodeByJson)),
          decodeByBinary(std::move(decodeByBinary)),
          encodeByBinary(std::move(encodeByBinary)) {}
#endif

    template<class T>
    static std::unique_ptr<NodeType> createParam(const std::wstring &nodeName,
                                                 bool isMustAfterWhiteSpace = true,
                                                 const std::vector<NodeCreateStage::NodeCreateStage> &nodeCreateStage =
                                                         {NodeCreateStage::JSON_NODE, NodeCreateStage::REPEAT_NODE, NodeCreateStage::COMMAND_PARAM_NODE}) {
        return std::make_unique<NodeType>(
                nodeName,
#if CHelperOnlyReadBinary != true
                [nodeName, isMustAfterWhiteSpace, nodeCreateStage](const nlohmann::json &j, std::unique_ptr<NodeBase> &t) {
                    if (std::find(nodeCreateStage.begin(), nodeCreateStage.end(), NodeType::currentCreateStage) == nodeCreateStage.end()) {
                        Profile::push("unknown node type -> {}", nodeName);
                        throw std::runtime_error("unknown node type");
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
#endif
                [nodeName, isMustAfterWhiteSpace, nodeCreateStage](BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t) {
                    if (std::find(nodeCreateStage.begin(), nodeCreateStage.end(), NodeType::currentCreateStage) == nodeCreateStage.end()) {
                        Profile::push("unknown node type -> {}", nodeName);
                        throw std::runtime_error("unknown node type");
                    }
                    t = std::make_unique<T>();
                    binaryReader.decode((T &) *t);
                    if (HEDLEY_UNLIKELY(!t->isMustAfterWhiteSpace.has_value())) {
                        t->isMustAfterWhiteSpace = isMustAfterWhiteSpace;
                    }
                }
#if CHelperOnlyReadBinary != true
                ,
                [](BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t) {
                    binaryWriter.encode((T &) *t);
                }
#endif
                );
    }

    template<class T>
    static std::unique_ptr<NodeType> createJson(const std::wstring &nodeName) {
        return createParam<T>(nodeName, false, {NodeCreateStage::JSON_NODE});
    }

    template<class T>
    static std::unique_ptr<NodeType> createNone(const std::wstring &nodeName) {
        return std::make_unique<NodeType>(
                nodeName,
#if CHelperOnlyReadBinary != true
                [nodeName](const nlohmann::json &j, std::unique_ptr<NodeBase> &t) {
                    Profile::push("unknown node type -> {}", nodeName);
                    throw std::runtime_error("unknown node type");
                },
                [](nlohmann::json &j, const std::unique_ptr<NodeBase> &t) {
                    j = (T &) *t;
                },
#endif
                [nodeName](BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t) {
                    Profile::push("unknown node type -> {}", nodeName);
                    throw std::runtime_error("unknown node type");
                }
#if CHelperOnlyReadBinary != true
                ,
                [](BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t) {
                    binaryWriter.encode((T &) *t);
                }
#endif
                );
    }

    std::vector<NodeType *> NodeType::NODE_TYPES;

    NodeCreateStage::NodeCreateStage NodeType::currentCreateStage = NodeCreateStage::NONE;

    std::unique_ptr<NodeType> NodeType::BLOCK = createParam<NodeBlock>(L"BLOCK");
    std::unique_ptr<NodeType> NodeType::BOOLEAN = createParam<NodeBoolean>(L"BOOLEAN");
    std::unique_ptr<NodeType> NodeType::COMMAND = createParam<NodeCommand>(L"COMMAND");
    std::unique_ptr<NodeType> NodeType::COMMAND_NAME = createParam<NodeCommandName>(L"COMMAND_NAME");
    std::unique_ptr<NodeType> NodeType::FLOAT = createParam<NodeFloat>(L"FLOAT");
    std::unique_ptr<NodeType> NodeType::INTEGER = createParam<NodeInteger>(L"INTEGER");
    std::unique_ptr<NodeType> NodeType::ITEM = createParam<NodeItem>(L"ITEM");
    std::unique_ptr<NodeType> NodeType::LF = createNone<NodeLF>(L"LF");
    std::unique_ptr<NodeType> NodeType::NAMESPACE_ID = createParam<NodeNamespaceId>(L"NAMESPACE_ID");
    std::unique_ptr<NodeType> NodeType::NORMAL_ID = createParam<NodeNormalId>(L"NORMAL_ID");
    std::unique_ptr<NodeType> NodeType::PER_COMMAND = createNone<NodePerCommand>(L"PER_COMMAND");
    std::unique_ptr<NodeType> NodeType::POSITION = createParam<NodePosition>(L"POSITION", false);
    std::unique_ptr<NodeType> NodeType::RELATIVE_FLOAT = createParam<NodeRelativeFloat>(L"RELATIVE_FLOAT", false);
    std::unique_ptr<NodeType> NodeType::REPEAT = createParam<NodeRepeat>(L"REPEAT", false,
                                                                         {NodeCreateStage::JSON_NODE, NodeCreateStage::COMMAND_PARAM_NODE});
    std::unique_ptr<NodeType> NodeType::STRING = createParam<NodeString>(L"STRING");
    std::unique_ptr<NodeType> NodeType::TARGET_SELECTOR = createParam<NodeTargetSelector>(L"TARGET_SELECTOR");
    std::unique_ptr<NodeType> NodeType::TEXT = createParam<NodeText>(L"TEXT");
    std::unique_ptr<NodeType> NodeType::RANGE = createParam<NodeRange>(L"RANGE");
    std::unique_ptr<NodeType> NodeType::XP_INTEGER = createParam<NodeXpInteger>(L"XP_INTEGER");
    std::unique_ptr<NodeType> NodeType::JSON = createParam<NodeJson>(L"JSON", true,
                                                                     {NodeCreateStage::REPEAT_NODE, NodeCreateStage::COMMAND_PARAM_NODE});
    std::unique_ptr<NodeType> NodeType::JSON_BOOLEAN = createJson<NodeJsonBoolean>(L"JSON_BOOLEAN");
    std::unique_ptr<NodeType> NodeType::JSON_ELEMENT = createNone<NodeJsonElement>(L"JSON_ELEMENT");
    std::unique_ptr<NodeType> NodeType::JSON_ENTRY = createNone<NodeJsonEntry>(L"JSON_ENTRY");
    std::unique_ptr<NodeType> NodeType::JSON_FLOAT = createJson<NodeJsonFloat>(L"JSON_FLOAT");
    std::unique_ptr<NodeType> NodeType::JSON_INTEGER = createJson<NodeJsonInteger>(L"JSON_INTEGER");
    std::unique_ptr<NodeType> NodeType::JSON_LIST = createJson<NodeJsonList>(L"JSON_LIST");
    std::unique_ptr<NodeType> NodeType::JSON_NULL = createJson<NodeJsonNull>(L"JSON_NULL");
    std::unique_ptr<NodeType> NodeType::JSON_OBJECT = createJson<NodeJsonObject>(L"JSON_OBJECT");
    std::unique_ptr<NodeType> NodeType::JSON_STRING = createJson<NodeJsonString>(L"JSON_STRING");
    std::unique_ptr<NodeType> NodeType::AND = createNone<NodeAnd>(L"AND");
    std::unique_ptr<NodeType> NodeType::ANY = createNone<NodeAny>(L"ANY");
    std::unique_ptr<NodeType> NodeType::ENTRY = createNone<NodeEntry>(L"ENTRY");
    std::unique_ptr<NodeType> NodeType::EQUAL_ENTRY = createNone<NodeEqualEntry>(L"EQUAL_ENTRY");
    std::unique_ptr<NodeType> NodeType::LIST = createNone<NodeList>(L"LIST");
    std::unique_ptr<NodeType> NodeType::OR = createNone<NodeOr>(L"OR");
    std::unique_ptr<NodeType> NodeType::SINGLE_SYMBOL = createNone<NodeSingleSymbol>(L"SINGLE_SYMBOL");

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