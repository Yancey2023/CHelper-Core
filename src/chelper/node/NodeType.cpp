//
// Created by Yancey on 2023/12/10.
//

#include "NodeType.h"

#include "param/NodeBlock.h"
#include "param/NodeBoolean.h"
#include "param/NodeCommand.h"
#include "param/NodeCommandName.h"
#include "param/NodeInteger.h"
#include "param/NodeItem.h"
#include "param/NodeNormalId.h"
#include "param/NodePosition.h"
#include "param/NodeRelativeFloat.h"
#include "param/NodeString.h"
#include "param/NodeTargetSelector.h"
#include "param/NodeText.h"
#include "param/NodeFloat.h"
#include "param/NodeRange.h"
#include "param/NodeJson.h"
#include "json/NodeJsonObject.h"
#include "json/NodeJsonList.h"
#include "json/NodeJsonString.h"
#include "json/NodeJsonInteger.h"
#include "json/NodeJsonBoolean.h"
#include "json/NodeJsonFloat.h"
#include "json/NodeJsonNull.h"
#include "param/NodeXpInteger.h"
#include "param/NodeRepeat.h"

namespace CHelper::Node {

    NodeType::NodeType(std::string nodeName,
                       std::function<std::unique_ptr<NodeBase>(const nlohmann::json &j,
                                                               const CPack &cpack)> createNodeByJson)
            : nodeName(std::move(nodeName)),
              createNodeByJson(std::move(createNodeByJson)) {}

    template<class T>
    static std::unique_ptr<NodeType> create(const std::string &nodeName) {
        return std::make_unique<NodeType>(
                nodeName, [](const nlohmann::json &j, const CPack &cpack) -> std::unique_ptr<NodeBase> {
                    return std::make_unique<T>(j, cpack);
                });
    }

    template<class T>
    static std::unique_ptr<NodeType> createJson(const std::string &nodeName) {
        return std::make_unique<NodeType>(
                nodeName, [&nodeName](const nlohmann::json &j, const CPack &cpack) -> std::unique_ptr<NodeBase> {
                    if (HEDLEY_UNLIKELY(!NodeType::canLoadNodeJson)) {
                        //这些节点在注册Json数据的时候创建，在命令注册的时候创建就抛出错误
                        throw Exception::UnknownNodeType(nodeName);
                    }
                    return std::make_unique<T>(j, cpack);
                });
    }

    std::vector<NodeType *> NodeType::NODE_TYPES;

    bool NodeType::canLoadNodeJson = false;

    std::unique_ptr<NodeType> NodeType::UNKNOWN = std::make_unique<NodeType>(
            "UNKNOWN", [](const nlohmann::json &j, const CPack &cpack) -> std::unique_ptr<NodeBase> {
                throw Exception::UnknownNodeType(
                        NodeType::UNKNOWN->nodeName);
            });
    std::unique_ptr<NodeType> NodeType::BLOCK = create<NodeBlock>("BLOCK");
    std::unique_ptr<NodeType> NodeType::BOOLEAN = create<NodeBoolean>("BOOLEAN");
    std::unique_ptr<NodeType> NodeType::COMMAND = create<NodeCommand>("COMMAND");
    std::unique_ptr<NodeType> NodeType::COMMAND_NAME = create<NodeCommandName>("COMMAND_NAME");
    std::unique_ptr<NodeType> NodeType::FLOAT = create<NodeFloat>("FLOAT");
    std::unique_ptr<NodeType> NodeType::INTEGER = create<NodeInteger>("INTEGER");
    std::unique_ptr<NodeType> NodeType::ITEM = create<NodeItem>("ITEM");
    std::unique_ptr<NodeType> NodeType::LF = std::make_unique<NodeType>(
            "LF", [](const nlohmann::json &j, const CPack &cpack) -> std::unique_ptr<NodeBase> {
                //这个节点只有一个实例，不可以生成多个实例
                throw Exception::UnknownNodeType(NodeType::LF->nodeName);
            });
    std::unique_ptr<NodeType> NodeType::NAMESPACE_ID = create<NodeNamespaceId>("NAMESPACE_ID");
    std::unique_ptr<NodeType> NodeType::NORMAL_ID = create<NodeNormalId>("NORMAL_ID");
    std::unique_ptr<NodeType> NodeType::PER_COMMAND = create<NodePerCommand>("PER_COMMAND");
    std::unique_ptr<NodeType> NodeType::POSITION = create<NodePosition>("POSITION");
    std::unique_ptr<NodeType> NodeType::RELATIVE_FLOAT = create<NodeRelativeFloat>("RELATIVE_FLOAT");
    std::unique_ptr<NodeType> NodeType::REPEAT = std::make_unique<NodeType>(
            "REPEAT", [](const nlohmann::json &j, const CPack &cpack) -> std::unique_ptr<NodeBase> {
                if (HEDLEY_UNLIKELY(NodeType::canLoadNodeJson)) {
                    //这个节点只能在命令注册的时候创建，在注册Json数据的时候创建就抛出错误
                    throw Exception::UnknownNodeType(NodeType::REPEAT->nodeName);
                }
                return std::make_unique<NodeRepeat>(j, cpack);
            });
    std::unique_ptr<NodeType> NodeType::STRING = create<NodeString>("STRING");
    std::unique_ptr<NodeType> NodeType::TARGET_SELECTOR = create<NodeTargetSelector>("TARGET_SELECTOR");
    std::unique_ptr<NodeType> NodeType::TEXT = create<NodeText>("TEXT");
    std::unique_ptr<NodeType> NodeType::RANGE = create<NodeRange>("RANGE");
    std::unique_ptr<NodeType> NodeType::XP_INTEGER = create<NodeXpInteger>("XP_INTEGER");
    std::unique_ptr<NodeType> NodeType::JSON = std::make_unique<NodeType>(
            "JSON", [](const nlohmann::json &j, const CPack &cpack) -> std::unique_ptr<NodeBase> {
                if (HEDLEY_UNLIKELY(NodeType::canLoadNodeJson)) {
                    //这个节点只能在命令注册的时候创建，在注册Json数据的时候创建就抛出错误
                    throw Exception::UnknownNodeType(NodeType::JSON->nodeName);
                }
                return std::make_unique<NodeJson>(j, cpack);
            });
    std::unique_ptr<NodeType> NodeType::JSON_OBJECT = createJson<NodeJsonObject>("JSON_OBJECT");
    std::unique_ptr<NodeType> NodeType::JSON_LIST = createJson<NodeJsonList>("JSON_LIST");
    std::unique_ptr<NodeType> NodeType::JSON_STRING = createJson<NodeJsonString>("JSON_STRING");
    std::unique_ptr<NodeType> NodeType::JSON_INTEGER = createJson<NodeJsonInteger>("JSON_INTEGER");
    std::unique_ptr<NodeType> NodeType::JSON_FLOAT = createJson<NodeJsonFloat>("JSON_FLOAT");
    std::unique_ptr<NodeType> NodeType::JSON_BOOLEAN = createJson<NodeJsonBoolean>("JSON_BOOLEAN");
    std::unique_ptr<NodeType> NodeType::JSON_NULL = createJson<NodeJsonNull>("JSON_NULL");

    inline void registerNodeType(const std::unique_ptr<NodeType> &nodeType) {
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
    }

} // CHelper::Node