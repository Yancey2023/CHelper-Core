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
#include "param/NodeLF.h"
#include "param/NodeNormalId.h"
#include "param/NodePosition.h"
#include "param/NodeRelativeFloat.h"
#include "param/NodeString.h"
#include "param/NodeTargetSelector.h"
#include "param/NodeText.h"
#include "param/NodeFloat.h"
#include "param/NodeRange.h"

namespace CHelper::Node {

    NodeType::NodeType(std::string nodeName,
                       std::shared_ptr<NodeBase>(*createNodeByJson)(const nlohmann::json &j, const CPack &cpack))
            : nodeName(std::move(nodeName)),
              createNodeByJson(createNodeByJson) {}

    std::vector<const NodeType *> NodeType::NODE_TYPES;

    NodeType NodeType::UNKNOWN("UNKNOWN",
                               [](const nlohmann::json &j, const CPack &cpack) -> std::shared_ptr<NodeBase> {
                                   return nullptr;
                               });

    NodeType NodeType::BLOCK = NodeType::create<NodeBlock>("BLOCK");

    NodeType NodeType::BOOLEAN = NodeType::create<NodeBoolean>("BOOLEAN");

    NodeType NodeType::COMMAND = NodeType::create<NodeCommand>("COMMAND");

    NodeType NodeType::COMMAND_NAME = NodeType::create<NodeCommandName>("COMMAND_NAME");

    NodeType NodeType::FLOAT = NodeType::create<NodeFloat>("FLOAT");

    NodeType NodeType::INTEGER = NodeType::create<NodeInteger>("INTEGER");

    NodeType NodeType::ITEM = NodeType::create<NodeItem>("ITEM");

    NodeType NodeType::LF("LF", [](const nlohmann::json &j,
                                   const CPack &cpack) -> std::shared_ptr<NodeBase> {
        return NodeLF::getInstance();
    });

    NodeType NodeType::NAMESPACE_ID = NodeType::create<NodeNamespaceId>("NAMESPACE_ID");

    NodeType NodeType::NORMAL_ID = NodeType::create<NodeNormalId>("NORMAL_ID");

    NodeType NodeType::PER_COMMAND = NodeType::create<NodePerCommand>("PER_COMMAND");

    NodeType NodeType::POSITION = NodeType::create<NodePosition>("POSITION");

    NodeType NodeType::RELATIVE_FLOAT = NodeType::create<NodeRelativeFloat>("RELATIVE_FLOAT");

    NodeType NodeType::STRING = NodeType::create<NodeString>("STRING");

    NodeType NodeType::TARGET_SELECTOR = NodeType::create<NodeTargetSelector>("TARGET_SELECTOR");

    NodeType NodeType::TEXT = NodeType::create<NodeText>("TEXT");
    NodeType NodeType::RANGE = NodeType::create<NodeRange>("RANGE");

    inline void registerNodeType(const NodeType &nodeType) {
        NodeType::NODE_TYPES.push_back(&nodeType);
    }

    void NodeType::init() {
        static bool isInit = false;
        if (isInit) {
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
        registerNodeType(STRING);
        registerNodeType(TARGET_SELECTOR);
        registerNodeType(TEXT);
    }


} // CHelper::Node