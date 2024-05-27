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
#include "param/NodeNormalId.h"
#include "param/NodePosition.h"
#include "param/NodeRange.h"
#include "param/NodeRelativeFloat.h"
#include "param/NodeRepeat.h"
#include "param/NodeString.h"
#include "param/NodeTargetSelector.h"
#include "param/NodeText.h"
#include "param/NodeXpInteger.h"
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
    static std::unique_ptr<NodeType> create(const std::string &nodeName, bool isMustAfterWhiteSpace = true) {
        return std::make_unique<NodeType>(
                nodeName,
                [isMustAfterWhiteSpace](const nlohmann::json &j, std::unique_ptr<NodeBase> &t) {
                    t = std::make_unique<T>();
                    j.get_to((T &) *t);
                    if (HEDLEY_UNLIKELY(!t->isMustAfterWhiteSpace.has_value())) {
                        t->isMustAfterWhiteSpace = isMustAfterWhiteSpace;
                    }
                },
                [](nlohmann::json &j, const std::unique_ptr<NodeBase> &t) {
                    j = (T &) *t;
                },
                [isMustAfterWhiteSpace](BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t) {
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
        return std::make_unique<NodeType>(
                nodeName,
                [nodeName](const nlohmann::json &j, std::unique_ptr<NodeBase> &t) {
                    if (HEDLEY_UNLIKELY(!NodeType::canLoadNodeJson)) {
                        //这些节点在注册Json数据的时候创建，在命令注册的时候创建就抛出错误
                        throw Exception::UnknownNodeType(nodeName);
                    }
                    t = std::make_unique<T>();
                    j.get_to((T &) *t);
                    if (HEDLEY_UNLIKELY(!t->isMustAfterWhiteSpace.has_value())) {
                        t->isMustAfterWhiteSpace = false;
                    }
                },
                [](nlohmann::json &j, const std::unique_ptr<NodeBase> &t) {
                    j = (T &) *t;
                },
                [nodeName](BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t) {
                    if (HEDLEY_UNLIKELY(!NodeType::canLoadNodeJson)) {
                        //这些节点在注册Json数据的时候创建，在命令注册的时候创建就抛出错误
                        throw Exception::UnknownNodeType(nodeName);
                    }
                    t = std::make_unique<T>();
                    binaryReader.decode((T &) *t);
                    if (HEDLEY_UNLIKELY(!t->isMustAfterWhiteSpace.has_value())) {
                        t->isMustAfterWhiteSpace = false;
                    }
                },
                [](BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t) {
                    binaryWriter.encode((T &) *t);
                });
    }

    std::vector<NodeType *> NodeType::NODE_TYPES;

    bool NodeType::canLoadNodeJson = false;

    std::unique_ptr<NodeType> NodeType::UNKNOWN = std::make_unique<NodeType>(
            "UNKNOWN",
            [](const nlohmann::json &j, std::unique_ptr<NodeBase> &t) {
                throw Exception::UnknownNodeType(NodeType::UNKNOWN->nodeName);
            },
            [](nlohmann::json &j, const std::unique_ptr<NodeBase> &t) {
                throw Exception::UnknownNodeType(NodeType::UNKNOWN->nodeName);
            },
            [](BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t) {
                throw Exception::UnknownNodeType(NodeType::UNKNOWN->nodeName);
            },
            [](BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t) {
                throw Exception::UnknownNodeType(NodeType::UNKNOWN->nodeName);
            });
    std::unique_ptr<NodeType> NodeType::BLOCK = create<NodeBlock>("BLOCK");
    std::unique_ptr<NodeType> NodeType::BOOLEAN = create<NodeBoolean>("BOOLEAN");
    std::unique_ptr<NodeType> NodeType::COMMAND = create<NodeCommand>("COMMAND");
    std::unique_ptr<NodeType> NodeType::COMMAND_NAME = create<NodeCommandName>("COMMAND_NAME");
    std::unique_ptr<NodeType> NodeType::FLOAT = create<NodeFloat>("FLOAT");
    std::unique_ptr<NodeType> NodeType::INTEGER = create<NodeInteger>("INTEGER");
    std::unique_ptr<NodeType> NodeType::ITEM = create<NodeItem>("ITEM");
    std::unique_ptr<NodeType> NodeType::LF = std::make_unique<NodeType>(
            "LF",
            [](const nlohmann::json &j, std::unique_ptr<NodeBase> &t) {
                //这个节点只有一个实例，不可以生成多个实例
                throw Exception::UnknownNodeType(NodeType::UNKNOWN->nodeName);
            },
            [](nlohmann::json &j, const std::unique_ptr<NodeBase> &t) {
                //这个节点只有一个实例，不可以生成多个实例
                throw Exception::UnknownNodeType(NodeType::UNKNOWN->nodeName);
            },
            [](BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t) {
                //这个节点只有一个实例，不可以生成多个实例
                throw Exception::UnknownNodeType(NodeType::UNKNOWN->nodeName);
            },
            [](BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t) {
                //这个节点只有一个实例，不可以生成多个实例
                throw Exception::UnknownNodeType(NodeType::UNKNOWN->nodeName);
            });
    std::unique_ptr<NodeType> NodeType::NAMESPACE_ID = create<NodeNamespaceId>("NAMESPACE_ID");
    std::unique_ptr<NodeType> NodeType::NORMAL_ID = create<NodeNormalId>("NORMAL_ID");
    std::unique_ptr<NodeType> NodeType::PER_COMMAND = std::make_unique<NodeType>(
            "PER_COMMAND",
            [](const nlohmann::json &j, std::unique_ptr<NodeBase> &t) {
                throw Exception::UnknownNodeType(NodeType::PER_COMMAND->nodeName);
            },
            [](nlohmann::json &j, const std::unique_ptr<NodeBase> &t) {
                throw Exception::UnknownNodeType(NodeType::PER_COMMAND->nodeName);
            },
            [](BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t) {
                throw Exception::UnknownNodeType(NodeType::PER_COMMAND->nodeName);
            },
            [](BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t) {
                throw Exception::UnknownNodeType(NodeType::PER_COMMAND->nodeName);
            });
    std::unique_ptr<NodeType> NodeType::POSITION = create<NodePosition>("POSITION", false);
    std::unique_ptr<NodeType> NodeType::RELATIVE_FLOAT = create<NodeRelativeFloat>("RELATIVE_FLOAT", false);
    std::unique_ptr<NodeType> NodeType::REPEAT = std::make_unique<NodeType>(
            "REPEAT",
            [](const nlohmann::json &j, std::unique_ptr<NodeBase> &t) {
                if (HEDLEY_UNLIKELY(NodeType::canLoadNodeJson)) {
                    //这个节点只能在命令注册的时候创建，在注册Json数据的时候创建就抛出错误
                    throw Exception::UnknownNodeType(NodeType::REPEAT->nodeName);
                }
                t = std::make_unique<NodeRepeat>();
                j.get_to((NodeRepeat &) *t);
                if (HEDLEY_UNLIKELY(!t->isMustAfterWhiteSpace.has_value())) {
                    t->isMustAfterWhiteSpace = false;
                }
            },
            [](nlohmann::json &j, const std::unique_ptr<NodeBase> &t) {
                j = (NodeRepeat &) *t;
            },
            [](BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t) {
                if (HEDLEY_UNLIKELY(NodeType::canLoadNodeJson)) {
                    //这个节点只能在命令注册的时候创建，在注册Json数据的时候创建就抛出错误
                    throw Exception::UnknownNodeType(NodeType::REPEAT->nodeName);
                }
                t = std::make_unique<NodeRepeat>();
                binaryReader.decode((NodeRepeat &) *t);
                if (HEDLEY_UNLIKELY(!t->isMustAfterWhiteSpace.has_value())) {
                    t->isMustAfterWhiteSpace = false;
                }
            },
            [](BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t) {
                binaryWriter.encode((NodeRepeat &) *t);
            });
    std::unique_ptr<NodeType> NodeType::STRING = create<NodeString>("STRING");
    std::unique_ptr<NodeType> NodeType::TARGET_SELECTOR = create<NodeTargetSelector>("TARGET_SELECTOR");
    std::unique_ptr<NodeType> NodeType::TEXT = create<NodeText>("TEXT");
    std::unique_ptr<NodeType> NodeType::RANGE = create<NodeRange>("RANGE");
    std::unique_ptr<NodeType> NodeType::XP_INTEGER = create<NodeXpInteger>("XP_INTEGER");
    std::unique_ptr<NodeType> NodeType::JSON = std::make_unique<NodeType>(
            "JSON",
            [](const nlohmann::json &j, std::unique_ptr<NodeBase> &t) {
                if (HEDLEY_UNLIKELY(NodeType::canLoadNodeJson)) {
                    //这个节点只能在命令注册的时候创建，在注册Json数据的时候创建就抛出错误
                    throw Exception::UnknownNodeType(NodeType::JSON->nodeName);
                }
                t = std::make_unique<NodeJson>();
                j.get_to((NodeJson &) *t);
                if (HEDLEY_UNLIKELY(!t->isMustAfterWhiteSpace.has_value())) {
                    t->isMustAfterWhiteSpace = true;
                }
            },
            [](nlohmann::json &j, const std::unique_ptr<NodeBase> &t) {
                j = (NodeJson &) *t;
            },
            [](BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t) {
                if (HEDLEY_UNLIKELY(NodeType::canLoadNodeJson)) {
                    //这个节点只能在命令注册的时候创建，在注册Json数据的时候创建就抛出错误
                    throw Exception::UnknownNodeType(NodeType::JSON->nodeName);
                }
                binaryReader.decode((std::unique_ptr<NodeJson> &) t);
                if (HEDLEY_UNLIKELY(!t->isMustAfterWhiteSpace.has_value())) {
                    t->isMustAfterWhiteSpace = true;
                }
            },
            [](BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t) {
                binaryWriter.encode((std::unique_ptr<NodeJson> &) t);
            });
    std::unique_ptr<NodeType> NodeType::JSON_OBJECT = createJson<NodeJsonObject>("JSON_OBJECT");
    std::unique_ptr<NodeType> NodeType::JSON_LIST = createJson<NodeJsonList>("JSON_LIST");
    std::unique_ptr<NodeType> NodeType::JSON_STRING = createJson<NodeJsonString>("JSON_STRING");
    std::unique_ptr<NodeType> NodeType::JSON_INTEGER = createJson<NodeJsonInteger>("JSON_INTEGER");
    std::unique_ptr<NodeType> NodeType::JSON_FLOAT = createJson<NodeJsonFloat>("JSON_FLOAT");
    std::unique_ptr<NodeType> NodeType::JSON_BOOLEAN = createJson<NodeJsonBoolean>("JSON_BOOLEAN");
    std::unique_ptr<NodeType> NodeType::JSON_NULL = createJson<NodeJsonNull>("JSON_NULL");

    void registerNodeType(const std::unique_ptr<NodeType> &nodeType) {
        NodeType::NODE_TYPES.push_back(nodeType.get());
    }

    void NodeType::init() {
        //因为节点是静态创建的，在创建节点的时候添加到列表有时会出问题，所以改为手动添加
        static bool isInit = false;
        if (HEDLEY_UNLIKELY(isInit)) {
            return;
        }
        isInit = true;
        registerNodeType(UNKNOWN);
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
        registerNodeType(JSON_OBJECT);
        registerNodeType(JSON_LIST);
        registerNodeType(JSON_STRING);
        registerNodeType(JSON_INTEGER);
        registerNodeType(JSON_FLOAT);
        registerNodeType(JSON_BOOLEAN);
        registerNodeType(JSON_NULL);
        size_t size = NodeType::NODE_TYPES.size();
        for (int i = 0; i < size; ++i) {
            NodeType::NODE_TYPES[i]->id = i;
        }
    }

}// namespace CHelper::Node