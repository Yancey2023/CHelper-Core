//
// Created by Yancey on 2025-06-20.
//

#include <chelper/node/CommandNode.h>
#include <chelper/node/NodeInitialization.h>
#include <chelper/resources/CPack.h>
#include <chelper/serialization/Serialization.h>

#define CHELPER_INIT(v1)                                                                                                                                                        \
    case Node::NodeTypeId::v1:                                                                                                                                                  \
        NodeInitialization<typename Node::NodeTypeDetail<Node::NodeTypeId::v1>::Type>::init(*reinterpret_cast<NodeTypeDetail<Node::NodeTypeId::v1>::Type *>(node.data), cpack); \
        break;

namespace CHelper::Node {

    template<>
    struct NodeInitialization<NodeBlock> {
        static void init(NodeBlock &node, const CPack &cpack) {
            node.blockIds = cpack.blockIds;
            node.nodeBlockId = NodeNamespaceId("BLOCK_ID", u"方块ID", "block", true);
            initNode(node.nodeBlockId, cpack);
        }
    };

    template<>
    struct NodeInitialization<NodeCommand> {
        static void init(NodeCommand &node, const CPack &cpack) {
            node.commands = cpack.commands.get();
        }
    };

    template<>
    struct NodeInitialization<NodeCommandName> {
        static void init(NodeCommandName &node, const CPack &cpack) {
            node.commands = cpack.commands.get();
        }
    };

    template<>
    struct NodeInitialization<NodeIntegerWithUnit> {
        static void init(NodeIntegerWithUnit &node, const CPack &cpack) {
            static NodeInteger nodeInteger("INTEGER", u"整数", std::nullopt, std::nullopt);
            node.nodeUnits = NodeNormalId("UNITS", u"单位", node.units, false);
            node.nodeIntegerWithUnit = NodeAnd({nodeInteger, node.nodeUnits});
            node.nodeIntegerMaybeHaveUnit = NodeOr({node.nodeIntegerWithUnit, nodeInteger}, false, true);
        }
    };

    template<>
    struct NodeInitialization<NodeItem> {
        static void init(NodeItem &node, const CPack &cpack) {
            node.itemIds = cpack.itemIds;
            node.nodeItemId = NodeNamespaceId("ITEM_ID", u"物品ID", "item", true);
            node.nodeComponent = NodeJson("ITEM_COMPONENT", u"物品组件", "components");
            initNode(node.nodeItemId, cpack);
            initNode(node.nodeComponent, cpack);
        }
    };

    template<>
    struct NodeInitialization<NodeJson> {
        ;
        static void init(NodeJson &node, const CPack &cpack) {
            for (const auto &item: cpack.jsonNodes) {
                if (HEDLEY_UNLIKELY(item.id == node.key)) {
                    node.nodeJson = item;
                    return;
                }
            }
            Profile::push("linking contents to {}", FORMAT_ARG(node.key));
            Profile::push("failed to find json data in the cpack -> {}", FORMAT_ARG(node.key));
            throw std::runtime_error("failed to find json data");
        }
    };

    template<>
    struct NodeInitialization<NodeNamespaceId> {
        static void init(NodeNamespaceId &node, const CPack &cpack) {
            if (HEDLEY_LIKELY(node.contents.has_value())) {
                node.customContents = node.contents.value();
            } else if (HEDLEY_LIKELY(node.key.has_value())) {
                node.customContents = cpack.getNamespaceId(node.key.value());
            }
            if (HEDLEY_UNLIKELY(node.customContents == nullptr)) {
                if (HEDLEY_UNLIKELY(node.key.has_value())) {
                    Profile::push("linking contents to {}", FORMAT_ARG(node.key.value()));
                    Profile::push("failed to find namespace id in the cpack -> {}", FORMAT_ARG(node.key.value()));
                    throw std::runtime_error("failed to find namespace id");
                } else {
                    throw std::runtime_error("missing content");
                }
            }
        }
    };

    template<>
    struct NodeInitialization<NodeNormalId> {
        static void init(NodeNormalId &node, const CPack &cpack) {
            if (HEDLEY_UNLIKELY(node.getNormalIdASTNode == nullptr)) {
                node.getNormalIdASTNode = [](const NodeWithType &node, TokenReader &tokenReader) -> ASTNode {
                    return tokenReader.readUntilSpace(node);
                };
            }
            if (HEDLEY_LIKELY(node.contents.has_value())) {
                node.customContents = node.contents.value();
            } else if (HEDLEY_LIKELY(node.key.has_value())) {
                node.customContents = cpack.getNormalId(node.key.value());
            }
            if (HEDLEY_UNLIKELY(node.customContents == nullptr)) {
                if (HEDLEY_LIKELY(node.key.has_value())) {
                    Profile::push("linking contents to {}", FORMAT_ARG(node.key.value()));
                    Profile::push("failed to find normal id in the cpack -> ", FORMAT_ARG(node.key.value()));
                    throw std::runtime_error("failed to find normal id");
                } else {
                    throw std::runtime_error("missing content");
                }
            }
        }
    };

    template<>
    struct NodeInitialization<NodePerCommand> {
        static void init(NodePerCommand &node, const CPack &cpack) {
            for (const auto &item: node.wrappedNodes) {
                Profile::push(R"(init node {}: "{}")", FORMAT_ARG(getNodeTypeName(item.nodeTypeId)), FORMAT_ARG(item.getNodeSerializable().id.value_or("UNKNOWN")));
                initNode(item.innerNode, cpack);
                Profile::pop();
            }
            for (const auto &item: node.wrappedNodes) {
                if (HEDLEY_UNLIKELY(item.nextNodes.empty())) {
                    Profile::push("dismiss child node ids, the parent node is {} (in command {})",
                                  FORMAT_ARG(item.getNodeSerializable().id.value_or("UNKNOWN")),
                                  FORMAT_ARG(utf8::utf16to8(fmt::format(u"", StringUtil::join(node.name, u",")))));
                    throw std::runtime_error("dismiss child node ids");
                }
            }
#ifdef CHelperDebug
            for (const auto &item: node.wrappedNodes) {
                bool flag1 = item.innerNode.nodeTypeId == NodeTypeId::POSITION ||
                             item.innerNode.nodeTypeId == NodeTypeId::RELATIVE_FLOAT;
                for (const auto &item2: item.nextNodes) {
                    if (HEDLEY_UNLIKELY(item2 == NodeLF::getInstance())) {
                        continue;
                    }
                    bool flag2 = item2->innerNode.nodeTypeId == NodeTypeId::POSITION ||
                                 item2->innerNode.nodeTypeId == NodeTypeId::RELATIVE_FLOAT;
                    if (HEDLEY_UNLIKELY(flag1 && flag2 == item2->getNodeSerializable().isMustAfterSpace)) {
                        Profile::push(R"({} should be {} in node "{}")",
                                      "isMustAfterSpace",
                                      item2->getNodeSerializable().isMustAfterSpace ? "false" : "true",
                                      item2->getNodeSerializable().id.value_or("UNKNOWN"));
                        throw std::runtime_error("value is wrong");
                    }
                }
            }
#endif
        }
    };

    template<>
    struct NodeInitialization<NodeRepeat> {
        static void init(NodeRepeat &node, const CPack &cpack) {
            const auto &it = cpack.repeatNodes.find(node.key);
            if (HEDLEY_LIKELY(it != cpack.repeatNodes.end())) {
                node.repeatData = it->second.first;
                node.nodeElement = it->second.second;
                return;
            }
            Profile::push("link repeat data {} to content", FORMAT_ARG(node.key));
            Profile::push("fail to find repeat data by id {}", FORMAT_ARG(node.key));
            throw std::runtime_error("fail to find repeat data");
        }
    };

    template<>
    struct NodeInitialization<NodeTargetSelector> {
        static void init(NodeTargetSelector &node, const CPack &cpack) {
            static NodeSingleSymbol nodeLeft(u'[', u"目标选择器参数左括号");
            static NodeSingleSymbol nodeRight(u']', u"目标选择器参数右括号");
            static NodeInteger nodeIntegerInTargetSelector("TARGET_SELECTOR_ARGUMENT_INTEGER", u"目标选择器参数(整数)", std::nullopt, std::nullopt);
            static NodeFloat nodeFloat("TARGET_SELECTOR_ARGUMENT_FLOAT", u"目标选择器参数(小数)", std::nullopt, std::nullopt);
            static NodeSingleSymbol nodeScoreValueLeft(u'{', u"目标选择器scores参数左括号");
            static NodeSingleSymbol nodeScoreValueRight(u'}', u"目标选择器scores参数右括号");
            static NodeSingleSymbol nodeScoreValueSeparator(u',', u"目标选择器scores参数分隔符");
            static NodeRange nodeScoreRange("TARGET_SELECTOR_ARGUMENT_SCORES_RANGE", u"目标选择器scores参数值的分数范围)");
            static NodeEntry nodeScoreValueElement(NodeTargetSelector::nodeString, NodeEqualEntry::nodeEqualOrNotEqual, nodeScoreRange);
            static NodeList nodeScore(nodeScoreValueLeft, nodeScoreValueElement, nodeScoreValueSeparator, nodeScoreValueRight);
            static NodeSingleSymbol nodeHasItemValueLeft1(u'{', u"目标选择器hasitem参数左括号");
            static NodeSingleSymbol nodeHasItemValueRight1(u'}', u"目标选择器hasitem参数右括号");
            static NodeSingleSymbol nodeHasItemValueSeparator(u',', u"目标选择器hasitem参数分隔符");
            static NodeSingleSymbol nodeHasItemValueLeft2(u'[', u"目标选择器hasitem参数左括号");
            static NodeSingleSymbol nodeHasItemValueRight2(u']', u"目标选择器haspermission参数右括号");
            static NodeSingleSymbol nodeHasPermissionValueLeft(u'{', u"目标选择器haspermission参数左括号");
            static NodeSingleSymbol nodeHasPermissionValueRight(u'}', u"目标选择器haspermission参数右括号");
            static NodeSingleSymbol nodeHasPermissionValueSeparator(u',', u"目标选择器haspermission参数分隔符");
            static NodeNormalId nodeHasPermissionState(
                    "PERMISSION_STATUS", u"权限状态",
                    std::make_shared<std::vector<std::shared_ptr<NormalId>>>(std::vector<std::shared_ptr<NormalId>>{
                            NormalId::make(u"enabled", u"启用"),
                            NormalId::make(u"disabled", u"禁用")}),
                    false);
            static NodeEqualEntry nodeHasPermissionEntry(
                    std::vector<EqualData>{
                            {u"camera", u"玩家能否转动相机视角", false, nodeHasPermissionState},
                            {u"movement", u"玩家能否移动", false, nodeHasPermissionState}});
            static NodeList nodeHasPermission(nodeHasPermissionValueLeft, nodeHasPermissionEntry, nodeHasPermissionValueSeparator, nodeHasPermissionValueRight);
            static NodeSingleSymbol nodeHasPropertyValueLeft('{', u"目标选择器has_property参数左括号");
            static NodeSingleSymbol nodeHasPropertyValueRight('}', u"目标选择器has_property参数右括号");
            static NodeSingleSymbol nodeHasPropertyValueSeparator(',', u"目标选择器has_property参数分隔符");
            static NodeOr nodeAny({NodeTargetSelector::nodeBoolean, nodeScoreRange, NodeTargetSelector::nodeString, nodeFloat}, false);
            static NodeEntry nodeHasPropertyEntry(NodeTargetSelector::nodeString, NodeEqualEntry::nodeEqualOrNotEqual, nodeAny);
            static NodeList nodeHasProperty(nodeHasPropertyValueLeft, nodeHasPropertyEntry, nodeHasPropertyValueSeparator, nodeHasPropertyValueRight);
            static NodeInteger nodeHasItemData("TARGET_SELECTOR_ARGUMENT_HASITEM_ITEM_DATA", u"目标选择器hasitem参数物品数据值", 0, 32767);
            static NodeRange nodeHasItemQuantity("TARGET_SELECTOR_ARGUMENT_HASITEM_ITEM_QUANTITY", u"目标选择器hasitem限制范围内的所有槽位中符合条件的物品的总和数量");
            static NodeRange nodeHasItemSlotRange("TARGET_SELECTOR_ARGUMENT_HASITEM_SLOT_SLOT_RANGE", u"目标选择器hasitem要检测的槽位范围");
            node.nodeItem = NodeNamespaceId("ITEM_ID", u"物品ID", "item", true),
            node.nodeFamily = NodeNormalId("FAMILIES", u"族", "entityFamily", true);
            node.nodeGameMode = NodeNormalId("GAME_MODES", u"游戏模式", "gameMode", true),
            node.nodeSlot = NodeNormalId("SLOT", u"物品栏", "entitySlot", true),
            node.nodeEntities = NodeNamespaceId("ENTITIES", u"实体", "entity", true),
            node.nodeHasItemElement = NodeEqualEntry(
                    std::vector<EqualData>{
                            {u"item", u"要检测的物品", false, node.nodeItem},
                            {u"data", u"要检测的物品的数据值", false, nodeHasItemData},
                            {u"quantity", u"限制范围内的所有槽位中符合条件的物品的总和数量", false, nodeHasItemQuantity},
                            {u"location", u"要检测的物品栏", false, node.nodeSlot},
                            {u"slot", u"用于指定要检测的槽位范围", true, nodeHasItemSlotRange}}),
            node.nodeHasItemList1 = NodeList(nodeHasItemValueLeft1, node.nodeHasItemElement, nodeHasItemValueSeparator, nodeHasItemValueRight1),
            node.nodeHasItemList2 = NodeList(nodeHasItemValueLeft2, node.nodeHasItemList1, nodeHasItemValueSeparator, nodeHasItemValueRight2),
            node.nodeHasItem = NodeOr({node.nodeHasItemList1, node.nodeHasItemList2}, false),
            node.nodeArgument = NodeEqualEntry(
                    std::vector<EqualData>{
                            {u"x", u"x坐标", false, NodeTargetSelector::nodeRelativeFloat},
                            {u"y", u"y坐标", false, NodeTargetSelector::nodeRelativeFloat},
                            {u"z", u"z坐标", false, NodeTargetSelector::nodeRelativeFloat},
                            {u"r", u"最大半径", false, nodeFloat},
                            {u"rm", u"最小半径", false, nodeFloat},
                            {u"dx", u"x坐标差异(检查实体的腿部位置)", false, nodeFloat},
                            {u"dy", u"y坐标差异(检查实体的腿部位置)", false, nodeFloat},
                            {u"dz", u"z坐标差异(检查实体的腿部位置)", false, nodeFloat},
                            {u"scores", u"分数", false, nodeScore},
                            {u"tag", u"标签", true, NodeTargetSelector::nodeString},
                            {u"name", u"名字", true, NodeTargetSelector::nodeString},
                            {u"type", u"实体类型", true, node.nodeEntities},
                            {u"family", u"族", true, node.nodeFamily},
                            {u"rx", u"最大垂直旋转", false, nodeFloat},
                            {u"rxm", u"最小垂直旋转", false, nodeFloat},
                            {u"ry", u"最大水平旋转", false, nodeFloat},
                            {u"rym", u"最小水平旋转", false, nodeFloat},
                            {u"hasitem", u"物品栏", false, node.nodeHasItem},
                            {u"haspermission", u"权限", false, nodeHasPermission},
                            {u"has_property", u"属性", false, nodeHasProperty},
                            {u"l", u"最大经验等级", false, nodeFloat},
                            {u"lm", u"最小经验等级", false, nodeFloat},
                            {u"m", u"游戏模式", true, node.nodeGameMode},
                            {u"c", u"目标数量(按照距离排序)", false, nodeIntegerInTargetSelector},
                    }),
            node.nodeArguments = NodeList(nodeLeft, node.nodeArgument, NodeTargetSelector::nodeSeparator, nodeRight);
            node.nodeOptionalArguments = NodeOptional(node.nodeArguments);
            node.nodeTargetSelectorVariableWithArgument = NodeAnd({NodeTargetSelector::nodeTargetSelectorVariable, node.nodeOptionalArguments});
            std::vector<NodeWithType> nodes;
            nodes.reserve(node.isWildcard ? 3 : 2);
            if (node.isWildcard) {
                nodes.emplace_back(Node::NodeTargetSelector::nodeWildcard);
            }
            nodes.emplace_back(node.nodeTargetSelectorVariableWithArgument);
            nodes.emplace_back(NodeTargetSelector::nodePlayerName);
            node.nodeTargetSelector = NodeOr(std::move(nodes), false);
            initNode(node.nodeItem, cpack);
            initNode(node.nodeFamily, cpack);
            initNode(node.nodeGameMode, cpack);
            initNode(node.nodeSlot, cpack);
            initNode(node.nodeEntities, cpack);
            initNode(node.nodeHasItemElement, cpack);
            initNode(node.nodeHasItemList1, cpack);
            initNode(node.nodeHasItemList2, cpack);
            initNode(node.nodeHasItem, cpack);
            initNode(node.nodeArgument, cpack);
            initNode(node.nodeArguments, cpack);
            initNode(node.nodeTargetSelectorVariableWithArgument, cpack);
            initNode(node.nodeTargetSelector, cpack);
        }
    };

    template<>
    struct NodeInitialization<NodeText> {
        static void init(NodeText &node, const CPack &cpack) {
            node.getTextASTNode = [](const NodeWithType &node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readUntilSpace(node);
            };
        }
    };

    template<>
    struct NodeInitialization<NodeAny> {
        static void init(NodeAny &node, const CPack &cpack) {
            if (HEDLEY_UNLIKELY(!node.node.has_value())) {
                static NodeSingleSymbol nodeLeft1(u'{', u"左括号");
                static NodeSingleSymbol nodeRight1(u'}', u"右括号");
                static NodeSingleSymbol nodeLeft2(u'[', u"左括号");
                static NodeSingleSymbol nodeRight2(u']', u"右括号");
                static NodeRange nodeRange("RANGE", u"范围");
                static NodeEntry nodeEntry(NodeTargetSelector::nodeString, NodeEqualEntry::nodeEqualOrNotEqual, *NodeAny::getNodeAny());
                static NodeList nodeObject(nodeLeft1, nodeEntry, NodeTargetSelector::nodeSeparator, nodeRight1);
                static NodeList nodeList(nodeLeft2, *NodeAny::getNodeAny(), NodeTargetSelector::nodeSeparator, nodeRight2);
                node.node = NodeOr({NodeTargetSelector::nodeRelativeFloat, NodeTargetSelector::nodeBoolean, NodeTargetSelector::nodeString, nodeObject, nodeRange, nodeList}, false);
            }
        }
    };

    template<>
    struct NodeInitialization<NodeWrapped> {
        static void init(NodeWrapped &node, const CPack &cpack) {
            initNode(node.innerNode, cpack);
        }
    };

    template<>
    struct NodeInitialization<NodeJsonEntry> {
        static void init(NodeJsonEntry &node, const CPack &cpack) {
        }
        static void init(NodeJsonEntry &node, const std::vector<NodeWithType> &dataList) {
            std::vector<NodeWithType> valueNodes;
            for (const auto &item: node.value) {
                bool notFind = true;
                for (const auto &item2: dataList) {
                    if (HEDLEY_UNLIKELY(reinterpret_cast<const NodeSerializable *>(item2.data)->id == item)) {
                        valueNodes.emplace_back(item2);
                        notFind = false;
                        break;
                    }
                }
                if (notFind) {
                    Profile::push("linking contents to {}", FORMAT_ARG(item));
                    Profile::push("failed to find node id -> {}", FORMAT_ARG(item));
                    Profile::push("unknown node id -> {} (in node \"{}\")", FORMAT_ARG(node.id.value_or("UNKNOWN")), FORMAT_ARG(item));
                    throw std::runtime_error("unknown node id");
                }
            }
            node.nodeKey = NodeText("JSON_OBJECT_ENTRY_KEY", u"JSON对象键",
                                    NormalId::make(fmt::format(u"\"{}\"", node.key), node.description));
            node.nodeValue = NodeOr(std::move(valueNodes), false);
            node.nodeEntry = NodeEntry(node.nodeKey, Node::NodeJsonEntry::nodeSeparator, node.nodeValue);
        }
    };

    template<>
    struct NodeInitialization<NodeJsonList> {
        static void init(NodeJsonList &node, const CPack &cpack) {
        }
        static void init(NodeJsonList &node, const std::vector<NodeWithType> &dataList) {
            for (const auto &item: dataList) {
                if (HEDLEY_UNLIKELY(reinterpret_cast<const NodeSerializable *>(item.data)->id == node.data)) {
                    node.nodeList = NodeList(Node::NodeJsonList::nodeLeft, item, Node::NodeJsonList::nodeSeparator, Node::NodeJsonList::nodeRight);
                    return;
                }
            }
            Profile::push("linking contents to {}", FORMAT_ARG(node.data));
            Profile::push("failed to find node id -> {}", FORMAT_ARG(node.data));
            Profile::push("unknown node id -> {} (in node \"{}\")", FORMAT_ARG(node.id.value_or("UNKNOWN")));
            throw std::runtime_error("unknown node id");
        }
    };

    template<>
    struct NodeInitialization<NodeJsonElement> {
        static void init(NodeJsonElement &node, const CPack &cpack) {
            Profile::push("linking startNode \"{}\" to nodes", FORMAT_ARG(node.startNodeId));
            for (const auto &item: node.nodes.nodes) {
                initNode(item, cpack);
            }
            if (HEDLEY_LIKELY(node.startNodeId != "LF")) {
                for (auto &item: node.nodes.nodes) {
                    if (HEDLEY_UNLIKELY(reinterpret_cast<const NodeSerializable *>(item.data)->id == node.startNodeId)) {
                        node.start = item;
                        break;
                    }
                }
            }
            if (HEDLEY_UNLIKELY(node.start.data == nullptr)) {
                Profile::push("unknown node id -> {} (in node \"{}\")", FORMAT_ARG(node.startNodeId));
            }
            for (auto &item: node.nodes.nodes) {
                if (HEDLEY_UNLIKELY(item.nodeTypeId == NodeTypeId::JSON_LIST)) {
                    NodeInitialization<NodeJsonList>::init(*reinterpret_cast<NodeJsonList *>(item.data), node.nodes.nodes);
                } else if (HEDLEY_UNLIKELY(item.nodeTypeId == NodeTypeId::JSON_OBJECT)) {
                    for (auto &item2: reinterpret_cast<NodeJsonObject *>(item.data)->data) {
                        NodeInitialization<NodeJsonEntry>::init(item2, node.nodes.nodes);
                    }
                }
            }
            Profile::pop();
        }
    };

    template<>
    struct NodeInitialization<NodeJsonObject> {
        static void init(NodeJsonObject &node, const CPack &cpack) {
            if (HEDLEY_UNLIKELY(node.data.empty())) {
                node.nodeElement1 = std::nullopt;
            } else {
                std::vector<NodeWithType> nodeElementData;
                nodeElementData.reserve(node.data.size());
                for (const auto &item: node.data) {
                    nodeElementData.emplace_back(item);
                }
                node.nodeElement1 = NodeOr(std::move(nodeElementData), false);
            }
            std::vector<NodeWithType> nodeElementData;
            if (HEDLEY_LIKELY(node.nodeElement1.has_value())) {
                nodeElementData.reserve(2);
                nodeElementData.emplace_back(node.nodeElement1.value());
            }
            nodeElementData.emplace_back(NodeJsonEntry::getNodeJsonAllEntry());
            node.nodeElement2 = NodeOr(std::move(nodeElementData), false, true);
            static NodeSingleSymbol nodeListLeft(u'{', u"JSON列表左括号");
            static NodeSingleSymbol nodeListRight(u'}', u"JSON列表右括号");
            static NodeSingleSymbol nodeListSeparator(u',', u"JSON列表分隔符");
            node.nodeList = NodeList(nodeListLeft, node.nodeElement2, nodeListSeparator, nodeListRight);
        }
    };

    template<>
    struct NodeInitialization<NodeJsonString> {
        static void init(NodeJsonString &node, const CPack &cpack) {
            if (HEDLEY_UNLIKELY(node.data.has_value())) {
                for (const auto &item: node.data.value().nodes) {
                    initNode(item, cpack);
                }
            }
            std::vector<NodeWithType> nodeDataElement;
            if (HEDLEY_UNLIKELY(node.data.has_value())) {
                nodeDataElement.reserve(node.data.value().nodes.size());
                for (const auto &item: node.data.value().nodes) {
                    nodeDataElement.push_back(item);
                }
            }
            node.nodeData = NodeOr(std::move(nodeDataElement), false);
        }
    };

    void initNode(Node::NodeWithType node, const CPack &cpack) {
        switch (node.nodeTypeId) {
            CODEC_PASTE(CHELPER_INIT, CHELPER_NODE_TYPES)
            default:
                HEDLEY_UNREACHABLE();
        }
    }

}// namespace CHelper::Node
