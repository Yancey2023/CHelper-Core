//
// Created by Yancey on 2023/12/10.
//

#include <chelper/node/NodeType.h>
#include <chelper/node/json/NodeJsonBoolean.h>
#include <chelper/node/json/NodeJsonList.h>
#include <chelper/node/json/NodeJsonNull.h>
#include <chelper/node/json/NodeJsonObject.h>
#include <chelper/node/json/NodeJsonString.h>
#include <chelper/node/param/NodeBlock.h>
#include <chelper/node/param/NodeBoolean.h>
#include <chelper/node/param/NodeCommand.h>
#include <chelper/node/param/NodeCommandName.h>
#include <chelper/node/param/NodeItem.h>
#include <chelper/node/param/NodeJson.h>
#include <chelper/node/param/NodeLF.h>
#include <chelper/node/param/NodeNormalId.h>
#include <chelper/node/param/NodePosition.h>
#include <chelper/node/param/NodeRange.h>
#include <chelper/node/param/NodeRelativeFloat.h>
#include <chelper/node/param/NodeRepeat.h>
#include <chelper/node/param/NodeString.h>
#include <chelper/node/param/NodeTargetSelector.h>
#include <chelper/node/param/NodeText.h>
#include <chelper/node/param/NodeXpInteger.h>
#include <chelper/node/template/NodeNumber.h>
#include <chelper/node/util/NodeAny.h>
#include <chelper/node/util/NodeEntry.h>
#include <chelper/node/util/NodeSingleSymbol.h>

namespace CHelper::Node {

#if CHelperOnlyReadBinary == true
    NodeType::NodeType(std::u16string nodeName,
                       std::function<void(BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t)> decodeByBinary)
        : nodeName(std::move(nodeName)),
          decodeByBinary(std::move(decodeByBinary)) {}
#else
    NodeType::NodeType(std::u16string nodeName,
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
    static std::unique_ptr<NodeType> createParam(const std::u16string &nodeName,
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
    static std::unique_ptr<NodeType> createJson(const std::u16string &nodeName) {
        return createParam<T>(nodeName, false, {NodeCreateStage::JSON_NODE});
    }

    template<class T>
    static std::unique_ptr<NodeType> createNone(const std::u16string &nodeName) {
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

    std::unique_ptr<NodeType> NodeType::BLOCK = createParam<NodeBlock>(u"BLOCK");
    std::unique_ptr<NodeType> NodeType::BOOLEAN = createParam<NodeBoolean>(u"BOOLEAN");
    std::unique_ptr<NodeType> NodeType::COMMAND = createParam<NodeCommand>(u"COMMAND");
    std::unique_ptr<NodeType> NodeType::COMMAND_NAME = createParam<NodeCommandName>(u"COMMAND_NAME");
    std::unique_ptr<NodeType> NodeType::FLOAT = createParam<NodeFloat>(u"FLOAT");
    std::unique_ptr<NodeType> NodeType::INTEGER = createParam<NodeInteger>(u"INTEGER");
    std::unique_ptr<NodeType> NodeType::ITEM = createParam<NodeItem>(u"ITEM");
    std::unique_ptr<NodeType> NodeType::LF = createNone<NodeLF>(u"LF");
    std::unique_ptr<NodeType> NodeType::NAMESPACE_ID = createParam<NodeNamespaceId>(u"NAMESPACE_ID");
    std::unique_ptr<NodeType> NodeType::NORMAL_ID = createParam<NodeNormalId>(u"NORMAL_ID");
    std::unique_ptr<NodeType> NodeType::PER_COMMAND = createNone<NodePerCommand>(u"PER_COMMAND");
    std::unique_ptr<NodeType> NodeType::POSITION = createParam<NodePosition>(u"POSITION", false);
    std::unique_ptr<NodeType> NodeType::RELATIVE_FLOAT = createParam<NodeRelativeFloat>(u"RELATIVE_FLOAT", false);
    std::unique_ptr<NodeType> NodeType::REPEAT = createParam<NodeRepeat>(u"REPEAT", false,
                                                                         {NodeCreateStage::JSON_NODE, NodeCreateStage::COMMAND_PARAM_NODE});
    std::unique_ptr<NodeType> NodeType::STRING = createParam<NodeString>(u"STRING");
    std::unique_ptr<NodeType> NodeType::TARGET_SELECTOR = createParam<NodeTargetSelector>(u"TARGET_SELECTOR");
    std::unique_ptr<NodeType> NodeType::TEXT = createParam<NodeText>(u"TEXT");
    std::unique_ptr<NodeType> NodeType::RANGE = createParam<NodeRange>(u"RANGE");
    std::unique_ptr<NodeType> NodeType::XP_INTEGER = createParam<NodeXpInteger>(u"XP_INTEGER");
    std::unique_ptr<NodeType> NodeType::JSON = createParam<NodeJson>(u"JSON", true,
                                                                     {NodeCreateStage::REPEAT_NODE, NodeCreateStage::COMMAND_PARAM_NODE});
    std::unique_ptr<NodeType> NodeType::JSON_BOOLEAN = createJson<NodeJsonBoolean>(u"JSON_BOOLEAN");
    std::unique_ptr<NodeType> NodeType::JSON_ELEMENT = createNone<NodeJsonElement>(u"JSON_ELEMENT");
    std::unique_ptr<NodeType> NodeType::JSON_ENTRY = createNone<NodeJsonEntry>(u"JSON_ENTRY");
    std::unique_ptr<NodeType> NodeType::JSON_FLOAT = createJson<NodeJsonFloat>(u"JSON_FLOAT");
    std::unique_ptr<NodeType> NodeType::JSON_INTEGER = createJson<NodeJsonInteger>(u"JSON_INTEGER");
    std::unique_ptr<NodeType> NodeType::JSON_LIST = createJson<NodeJsonList>(u"JSON_LIST");
    std::unique_ptr<NodeType> NodeType::JSON_NULL = createJson<NodeJsonNull>(u"JSON_NULu");
    std::unique_ptr<NodeType> NodeType::JSON_OBJECT = createJson<NodeJsonObject>(u"JSON_OBJECT");
    std::unique_ptr<NodeType> NodeType::JSON_STRING = createJson<NodeJsonString>(u"JSON_STRING");
    std::unique_ptr<NodeType> NodeType::AND = createNone<NodeAnd>(u"AND");
    std::unique_ptr<NodeType> NodeType::ANY = createNone<NodeAny>(u"ANY");
    std::unique_ptr<NodeType> NodeType::ENTRY = createNone<NodeEntry>(u"ENTRY");
    std::unique_ptr<NodeType> NodeType::EQUAL_ENTRY = createNone<NodeEqualEntry>(u"EQUAL_ENTRY");
    std::unique_ptr<NodeType> NodeType::LIST = createNone<NodeList>(u"LIST");
    std::unique_ptr<NodeType> NodeType::OR = createNone<NodeOr>(u"OR");
    std::unique_ptr<NodeType> NodeType::SINGLE_SYMBOL = createNone<NodeSingleSymbol>(u"SINGLE_SYMBOu");

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