//
// Created by Yancey on 2025-06-20.
//

#include <chelper/node/NodeInitialization.h>
#include <chelper/resources/CPack.h>
#include <chelper/serialization/Serialization.h>

#define CHELPER_INIT(v1)                                                                                                             \
    case Node::NodeTypeId::v1:                                                                                                       \
        NodeInitialization<Node::NodeTypeId::v1>::init(reinterpret_cast<NodeTypeDetail<Node::NodeTypeId::v1>::Type &>(node), cpack); \
        break;

namespace CHelper::Node {

    template<>
    struct NodeInitialization<NodeTypeId::COMMAND> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::COMMAND>::Type;
        static void init(NodeType &node, const CPack &cpack) {
            node.commands = cpack.commands.get();
        }
    };

    template<>
    struct NodeInitialization<NodeTypeId::COMMAND_NAME> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::COMMAND_NAME>::Type;
        static void init(NodeType &node, const CPack &cpack) {
            node.commands = cpack.commands.get();
        }
    };

    static std::shared_ptr<NodeBase> nodeInteger = NodeInteger::make(
            "INTEGER", u"整数", std::nullopt, std::nullopt);

    template<>
    struct NodeInitialization<NodeTypeId::INTEGER_WITH_UNIT> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::INTEGER_WITH_UNIT>::Type;
        static void init(NodeType &node, const CPack &cpack) {
            node.nodeUnits = std::make_unique<NodeNormalId>("UNITS", u"单位", node.units, false);
            node.nodeIntegerWithUnit = std::make_unique<NodeAnd>(
                    std::vector<const NodeBase *>{nodeInteger.get(), node.nodeUnits.get()});
            node.nodeIntegerMaybeHaveUnit = std::make_unique<NodeOr>(
                    std::vector<const NodeBase *>{node.nodeIntegerWithUnit.get(), nodeInteger.get()},
                    false, true);
        }
    };

    template<>
    struct NodeInitialization<NodeTypeId::ITEM> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::ITEM>::Type;
        static void init(NodeType &node, const CPack &cpack) {
            node.itemIds = cpack.itemIds;
            node.nodeItemId = std::make_unique<NodeNamespaceId>("ITEM_ID", u"物品ID", "item", true);
            node.nodeComponent = std::make_unique<NodeJson>("ITEM_COMPONENT", u"物品组件", "components");
            initNode(*node.nodeItemId, cpack);
            initNode(*node.nodeComponent, cpack);
        }
    };

    template<>
    struct NodeInitialization<NodeTypeId::JSON> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::JSON>::Type;
        static void init(NodeType &node, const CPack &cpack) {
            for (const auto &item: cpack.jsonNodes) {
                if (HEDLEY_UNLIKELY(item->id == node.key)) {
                    node.nodeJson = item.get();
                    return;
                }
            }
            Profile::push("linking contents to {}", FORMAT_ARG(node.key));
            Profile::push("failed to find json data in the cpack -> {}", FORMAT_ARG(node.key));
            throw std::runtime_error("failed to find json data");
        }
    };

    template<>
    struct NodeInitialization<NodeTypeId::NAMESPACE_ID> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::NAMESPACE_ID>::Type;
        static void init(NodeType &node, const CPack &cpack) {
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
    struct NodeInitialization<NodeTypeId::NORMAL_ID> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::NORMAL_ID>::Type;
        static void init(NodeType &node, const CPack &cpack) {
            if (HEDLEY_UNLIKELY(node.getNormalIdASTNode == nullptr)) {
                node.getNormalIdASTNode = [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
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
    struct NodeInitialization<NodeTypeId::PER_COMMAND> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::PER_COMMAND>::Type;
        static void init(NodeType &node, const CPack &cpack) {
            for (const auto &item: node.wrappedNodes) {
                Profile::push(R"(init node {}: "{}")", FORMAT_ARG(NodeTypeHelper::getName(item.nodeTypeId)), FORMAT_ARG(item.innerNode->id.value_or("UNKNOWN")));
                initNode(*item.innerNode, cpack);
                Profile::pop();
            }
            for (const auto &item: node.wrappedNodes) {
                if (HEDLEY_UNLIKELY(item.nextNodes.empty())) {
                    Profile::push("dismiss child node ids, the parent node is {} (in command {})",
                                  FORMAT_ARG(item.innerNode->id.value_or("UNKNOWN")),
                                  FORMAT_ARG(utf8::utf16to8(fmt::format(u"", StringUtil::join(node.name, u",")))));
                    throw std::runtime_error("dismiss child node ids");
                }
            }
#ifdef CHelperDebug
            for (const auto &item: node.wrappedNodes) {
                bool flag1 = item.innerNode->nodeTypeId == NodeTypeId::POSITION ||
                             item.innerNode->nodeTypeId == NodeTypeId::RELATIVE_FLOAT;
                for (const auto &item2: item.nextNodes) {
                    if (HEDLEY_UNLIKELY(item2 == NodeLF::getInstance())) {
                        continue;
                    }
                    bool flag2 = item2->innerNode->nodeTypeId == NodeTypeId::POSITION ||
                                 item2->innerNode->nodeTypeId == NodeTypeId::RELATIVE_FLOAT;
                    if (HEDLEY_UNLIKELY(flag1 && flag2 == item2->innerNode->isMustAfterSpace)) {
                        Profile::push(R"({} should be {} in node "{}")",
                                      "isMustAfterSpace",
                                      item2->innerNode->isMustAfterSpace ? "false" : "true",
                                      item2->innerNode->id.value_or("UNKNOWN"));
                        throw std::runtime_error("value is wrong");
                    }
                }
            }
#endif
        }
    };

    template<>
    struct NodeInitialization<NodeTypeId::REPEAT> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::REPEAT>::Type;
        static void init(NodeType &node, const CPack &cpack) {
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

    static std::shared_ptr<NodeBase> nodeRight = std::make_shared<NodeSingleSymbol>(u']', u"目标选择器参数右括号");
    static std::shared_ptr<NodeBase> nodeSeparator = std::make_shared<NodeSingleSymbol>(u',', u"目标选择器参数分隔符");
    static std::unique_ptr<NodeBase> nodeRelativeFloat = std::make_unique<NodeRelativeFloat>(
            "TARGET_SELECTOR_ARGUMENT_RELATIVE_FLOAT", u"目标选择器参数(相对坐标)", false);
    static std::unique_ptr<NodeBase> nodeIntegerInTargetSelector = NodeInteger::make(
            "TARGET_SELECTOR_ARGUMENT_INTEGER", u"目标选择器参数(整数)", std::nullopt, std::nullopt);
    static std::unique_ptr<NodeBase> nodeFloat = NodeFloat::make(
            "TARGET_SELECTOR_ARGUMENT_FLOAT", u"目标选择器参数(小数)", std::nullopt, std::nullopt);
    static std::unique_ptr<NodeBase> nodeString = std::make_unique<NodeString>(
            "TARGET_SELECTOR_ARGUMENT_STRING", u"目标选择器参数(字符串)", true, true, false);
    static std::unique_ptr<NodeBase> nodeScoreValueLeft = std::make_unique<NodeSingleSymbol>(u'{', u"目标选择器scores参数左括号");
    static std::unique_ptr<NodeBase> nodeScoreValueRight = std::make_unique<NodeSingleSymbol>(u'}', u"目标选择器scores参数右括号");
    static std::unique_ptr<NodeBase> nodeScoreValueSeparator = std::make_unique<NodeSingleSymbol>(u',', u"目标选择器scores参数分隔符");
    static std::unique_ptr<NodeBase> nodeScoreRange = std::make_unique<NodeRange>(
            "TARGET_SELECTOR_ARGUMENT_SCORES_RANGE", u"目标选择器scores参数值的分数范围)");
    static std::unique_ptr<NodeBase> nodeScoreValueElement = std::make_unique<NodeEntry>(
            nodeString.get(), NodeEqualEntry::nodeEqualOrNotEqual.get(), nodeScoreRange.get());
    static std::unique_ptr<NodeBase> nodeScore = std::make_unique<NodeList>(
            nodeScoreValueLeft.get(), nodeScoreValueElement.get(),
            nodeScoreValueSeparator.get(), nodeScoreValueRight.get());
    static std::unique_ptr<NodeBase> nodeHasItemValueLeft1 = std::make_unique<NodeSingleSymbol>(u'{', u"目标选择器hasitem参数左括号");
    static std::unique_ptr<NodeBase> nodeHasItemValueRight1 = std::make_unique<NodeSingleSymbol>(u'}', u"目标选择器hasitem参数右括号");
    static std::unique_ptr<NodeBase> nodeHasItemValueSeparator = std::make_unique<NodeSingleSymbol>(u',', u"目标选择器hasitem参数分隔符");
    static std::unique_ptr<NodeBase> nodeHasItemValueLeft2 = std::make_unique<NodeSingleSymbol>(u'[', u"目标选择器hasitem参数左括号");
    static std::unique_ptr<NodeBase> nodeHasItemValueRight2 = std::make_unique<NodeSingleSymbol>(u']', u"目标选择器haspermission参数右括号");
    static std::unique_ptr<NodeBase> nodeHasPermissionValueLeft = std::make_unique<NodeSingleSymbol>(u'{', u"目标选择器haspermission参数左括号");
    static std::unique_ptr<NodeBase> nodeHasPermissionValueRight = std::make_unique<NodeSingleSymbol>(u'}', u"目标选择器haspermission参数右括号");
    static std::unique_ptr<NodeBase> nodeHasPermissionValueSeparator = std::make_unique<NodeSingleSymbol>(u',', u"目标选择器haspermission参数分隔符");
    static std::unique_ptr<NodeBase> nodeHasPermissionState = std::make_unique<NodeNormalId>(
            "PERMISSION_STATUS", u"权限状态",
            std::make_shared<std::vector<std::shared_ptr<NormalId>>>(std::vector<std::shared_ptr<NormalId>>{
                    NormalId::make(u"enabled", u"启用"),
                    NormalId::make(u"disabled", u"禁用")}),
            false);
    static std::unique_ptr<NodeBase> nodeHasPermissionEntry = std::make_unique<NodeEqualEntry>(
            std::vector<EqualData>{
                    {u"camera", u"玩家能否转动相机视角", false, nodeHasPermissionState.get()},
                    {u"movement", u"玩家能否移动", false, nodeHasPermissionState.get()}});
    static std::unique_ptr<NodeBase> nodeHasPermission = std::make_unique<NodeList>(
            nodeHasPermissionValueLeft.get(), nodeHasPermissionEntry.get(),
            nodeHasPermissionValueSeparator.get(), nodeHasPermissionValueRight.get());
    static std::unique_ptr<NodeBase> nodeHasPropertyValueLeft = std::make_unique<NodeSingleSymbol>('{', u"目标选择器has_property参数左括号");
    static std::unique_ptr<NodeBase> nodeHasPropertyValueRight = std::make_unique<NodeSingleSymbol>('}', u"目标选择器has_property参数右括号");
    static std::unique_ptr<NodeBase> nodeHasPropertyValueSeparator = std::make_unique<NodeSingleSymbol>(',', u"目标选择器has_property参数分隔符");
    static std::unique_ptr<NodeBase> nodeBoolean = NodeBoolean::make(
            "BOOLEAN", u"布尔值", std::nullopt, std::nullopt);
    static std::unique_ptr<NodeBase> nodeAny = std::make_unique<NodeOr>(
            std::vector<const NodeBase *>{
                    nodeBoolean.get(), nodeScoreRange.get(), nodeString.get(), nodeFloat.get()},
            false);
    static std::unique_ptr<NodeBase> nodeHasPropertyEntry = std::make_unique<NodeEntry>(
            nodeString.get(), NodeEqualEntry::nodeEqualOrNotEqual.get(), nodeAny.get());
    static std::unique_ptr<NodeBase> nodeHasProperty = std::make_unique<NodeList>(
            nodeHasPropertyValueLeft.get(), nodeHasPropertyEntry.get(),
            nodeHasPropertyValueSeparator.get(), nodeHasPropertyValueRight.get());
    static std::unique_ptr<NodeBase> nodeHasItemData = NodeInteger::make(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_ITEM_DATA", u"目标选择器hasitem参数物品数据值", 0, 32767);
    static std::unique_ptr<NodeBase> nodeHasItemQuantity = std::make_unique<NodeRange>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_ITEM_QUANTITY", u"目标选择器hasitem限制范围内的所有槽位中符合条件的物品的总和数量");
    static std::unique_ptr<NodeBase> nodeHasItemSlotRange = std::make_unique<NodeRange>(
            "TARGET_SELECTOR_ARGUMENT_HASITEM_SLOT_SLOT_RANGE", u"目标选择器hasitem要检测的槽位范围");

    template<>
    struct NodeInitialization<NodeTypeId::TARGET_SELECTOR> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::TARGET_SELECTOR>::Type;
        static void init(NodeType &node, const CPack &cpack) {
            node.nodeItem = std::make_unique<NodeNamespaceId>("ITEM_ID", u"物品ID", "item", true),
            node.nodeFamily = std::make_unique<NodeNormalId>("FAMILIES", u"族", "entityFamily", true);
            node.nodeGameMode = std::make_unique<NodeNormalId>("GAME_MODES", u"游戏模式", "gameMode", true),
            node.nodeSlot = std::make_unique<NodeNormalId>("SLOT", u"物品栏", "entitySlot", true),
            node.nodeEntities = std::make_unique<NodeNamespaceId>("ENTITIES", u"实体", "entity", true),
            node.nodeHasItemElement = std::make_unique<NodeEqualEntry>(
                    std::vector<EqualData>{
                            {u"item", u"要检测的物品", false, node.nodeItem.get()},
                            {u"data", u"要检测的物品的数据值", false, nodeHasItemData.get()},
                            {u"quantity", u"限制范围内的所有槽位中符合条件的物品的总和数量", false, nodeHasItemQuantity.get()},
                            {u"location", u"要检测的物品栏", false, node.nodeSlot.get()},
                            {u"slot", u"用于指定要检测的槽位范围", true, nodeHasItemSlotRange.get()}}),
            node.nodeHasItemList1 = std::make_unique<NodeList>(
                    nodeHasItemValueLeft1.get(), node.nodeHasItemElement.get(),
                    nodeHasItemValueSeparator.get(), nodeHasItemValueRight1.get()),
            node.nodeHasItemList2 = std::make_unique<NodeList>(
                    nodeHasItemValueLeft2.get(), node.nodeHasItemList1.get(),
                    nodeHasItemValueSeparator.get(), nodeHasItemValueRight2.get()),
            node.nodeHasItem = std::make_unique<NodeOr>(
                    std::vector<const NodeBase *>{
                            node.nodeHasItemList1.get(), node.nodeHasItemList2.get()},
                    false),
            node.nodeArgument = std::make_unique<NodeEqualEntry>(
                    std::vector<EqualData>{
                            {u"x", u"x坐标", false, nodeRelativeFloat.get()},
                            {u"y", u"y坐标", false, nodeRelativeFloat.get()},
                            {u"z", u"z坐标", false, nodeRelativeFloat.get()},
                            {u"r", u"最大半径", false, nodeFloat.get()},
                            {u"rm", u"最小半径", false, nodeFloat.get()},
                            {u"dx", u"x坐标差异(检查实体的腿部位置)", false, nodeFloat.get()},
                            {u"dy", u"y坐标差异(检查实体的腿部位置)", false, nodeFloat.get()},
                            {u"dz", u"z坐标差异(检查实体的腿部位置)", false, nodeFloat.get()},
                            {u"scores", u"分数", false, nodeScore.get()},
                            {u"tag", u"标签", true, nodeString.get()},
                            {u"name", u"名字", true, nodeString.get()},
                            {u"type", u"实体类型", true, node.nodeEntities.get()},
                            {u"family", u"族", true, node.nodeFamily.get()},
                            {u"rx", u"最大垂直旋转", false, nodeFloat.get()},
                            {u"rxm", u"最小垂直旋转", false, nodeFloat.get()},
                            {u"ry", u"最大水平旋转", false, nodeFloat.get()},
                            {u"rym", u"最小水平旋转", false, nodeFloat.get()},
                            {u"hasitem", u"物品栏", false, node.nodeHasItem.get()},
                            {u"haspermission", u"权限", false, nodeHasPermission.get()},
                            {u"has_property", u"属性", false, nodeHasProperty.get()},
                            {u"l", u"最大经验等级", false, nodeFloat.get()},
                            {u"lm", u"最小经验等级", false, nodeFloat.get()},
                            {u"m", u"游戏模式", true, node.nodeGameMode.get()},
                            {u"c", u"目标数量(按照距离排序)", false, nodeIntegerInTargetSelector.get()},
                    }),
            node.nodeArguments = std::make_unique<NodeList>(
                    node.nodeLeft.get(), node.nodeArgument.get(), nodeSeparator.get(), nodeRight.get());
            initNode(*node.nodeItem, cpack);
            initNode(*node.nodeFamily, cpack);
            initNode(*node.nodeGameMode, cpack);
            initNode(*node.nodeSlot, cpack);
            initNode(*node.nodeEntities, cpack);
            initNode(*node.nodeHasItemElement, cpack);
            initNode(*node.nodeHasItemList1, cpack);
            initNode(*node.nodeHasItemList2, cpack);
            initNode(*node.nodeHasItem, cpack);
            initNode(*node.nodeArgument, cpack);
            initNode(*node.nodeArguments, cpack);
        }
    };

    template<>
    struct NodeInitialization<NodeTypeId::TEXT> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::TEXT>::Type;
        static void init(NodeType &node, const CPack &cpack) {
            node.getTextASTNode = [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readUntilSpace(node);
            };
        }
    };

    static std::unique_ptr<NodeBase> nodeLeft1 = std::make_unique<NodeSingleSymbol>(u'{', u"左括号");
    static std::unique_ptr<NodeBase> nodeRight1 = std::make_unique<NodeSingleSymbol>(u'}', u"右括号");
    static std::unique_ptr<NodeBase> nodeLeft2 = std::make_unique<NodeSingleSymbol>(u'[', u"左括号");
    static std::unique_ptr<NodeBase> nodeRight2 = std::make_unique<NodeSingleSymbol>(u']', u"右括号");
    static std::unique_ptr<NodeBase> nodeRange = std::make_unique<NodeRange>(
            "RANGE", u"范围");
    static std::unique_ptr<NodeBase> nodeEntry = std::make_unique<NodeEntry>(
            nodeString.get(), NodeEqualEntry::nodeEqualOrNotEqual.get(), NodeAny::getNodeAny());
    static std::unique_ptr<NodeBase> nodeObject = std::make_unique<NodeList>(
            nodeLeft1.get(), nodeEntry.get(),
            nodeSeparator.get(), nodeRight1.get());
    static std::unique_ptr<NodeBase> nodeList = std::make_unique<NodeList>(
            nodeLeft2.get(), NodeAny::getNodeAny(),
            nodeSeparator.get(), nodeRight2.get());

    template<>
    struct NodeInitialization<NodeTypeId::ANY> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::ANY>::Type;
        static void init(NodeType &node, const CPack &cpack) {
            if (HEDLEY_UNLIKELY(node.node == nullptr)) {
                node.node = std::make_unique<NodeOr>(
                        std::vector<const NodeBase *>{
                                nodeRelativeFloat.get(), nodeBoolean.get(),
                                nodeString.get(), nodeObject.get(),
                                nodeRange.get(), nodeList.get()},
                        false);
            }
        }
    };

    template<>
    struct NodeInitialization<NodeTypeId::WRAPPED> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::WRAPPED>::Type;
        static void init(NodeType &node, const CPack &cpack) {
            initNode(*node.innerNode, cpack);
        }
    };

    template<>
    struct NodeInitialization<NodeTypeId::JSON_ENTRY> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::JSON_ENTRY>::Type;
        static void init(NodeType &node, const CPack &cpack) {
        }
        static void init(NodeType &node, const std::vector<std::unique_ptr<NodeSerializable>> &dataList) {
            std::vector<const NodeBase *> valueNodes;
            for (const auto &item: node.value) {
                bool notFind = true;
                for (const auto &item2: dataList) {
                    if (HEDLEY_UNLIKELY(item2->id == item)) {
                        valueNodes.push_back(item2.get());
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
            node.nodeKey = std::make_unique<NodeText>(
                    "JSON_OBJECT_ENTRY_KEY", u"JSON对象键",
                    NormalId::make(fmt::format(u"\"{}\"", node.key), node.description));
            node.nodeValue = std::make_unique<NodeOr>(
                    std::move(valueNodes), false);
            nodeEntry = std::make_unique<NodeEntry>(
                    node.nodeKey.get(), nodeSeparator.get(), node.nodeValue.get());
        }
    };

    template<>
    struct NodeInitialization<NodeTypeId::JSON_LIST> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::JSON_LIST>::Type;
        static void init(NodeType &node, const CPack &cpack) {
        }
        static void init(NodeType &node, const std::vector<std::unique_ptr<NodeSerializable>> &dataList) {
            for (const auto &item: dataList) {
                if (HEDLEY_UNLIKELY(item->id == node.data)) {
                    nodeList = std::make_unique<NodeList>(Node::NodeJsonList::nodeLeft.get(), item.get(),
                                                          nodeSeparator.get(), nodeRight.get());
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
    struct NodeInitialization<NodeTypeId::JSON_ELEMENT> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::JSON_ELEMENT>::Type;
        static void init(NodeType &node, const CPack &cpack) {
            Profile::push("linking startNode \"{}\" to nodes", FORMAT_ARG(node.startNodeId));
            for (const auto &item: node.nodes) {
                initNode(*item, cpack);
            }
            if (HEDLEY_LIKELY(node.startNodeId != "LF")) {
                for (auto &item: node.nodes) {
                    if (HEDLEY_UNLIKELY(item->id == node.startNodeId)) {
                        node.start = item.get();
                        break;
                    }
                }
            }
            if (HEDLEY_UNLIKELY(node.start == nullptr)) {
                Profile::push("unknown node id -> {} (in node \"{}\")", FORMAT_ARG(node.startNodeId));
            }
            for (const auto &item: node.nodes) {
                if (HEDLEY_UNLIKELY(item->nodeTypeId == NodeTypeId::JSON_LIST)) {
                    NodeInitialization<NodeTypeId::JSON_LIST>::init(reinterpret_cast<NodeJsonList &>(*item), node.nodes);
                } else if (HEDLEY_UNLIKELY(item->nodeTypeId == NodeTypeId::JSON_OBJECT)) {
                    for (const auto &item2: reinterpret_cast<NodeJsonObject *>(item.get())->data) {
                        NodeInitialization<NodeTypeId::JSON_ENTRY>::init(*item2, node.nodes);
                    }
                }
            }
            Profile::pop();
        }
    };

    template<>
    struct NodeInitialization<NodeTypeId::JSON_OBJECT> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::JSON_OBJECT>::Type;
        static void init(NodeType &node, const CPack &cpack) {
            node.nodeElement1 = nullptr;
            std::vector<const NodeBase *> nodeElementData;
            nodeElementData.push_back(NodeJsonEntry::getNodeJsonAllEntry());
            node.nodeElement2 = std::make_unique<NodeOr>(std::move(nodeElementData), false, true);
            static std::unique_ptr<NodeBase> nodeObjectLeft = std::make_unique<NodeSingleSymbol>(u'{', u"JSON列表左括号");
            static std::unique_ptr<NodeBase> nodeObjectRight = std::make_unique<NodeSingleSymbol>(u'}', u"JSON列表右括号");
            static std::unique_ptr<NodeBase> nodeObjectSeparator = std::make_unique<NodeSingleSymbol>(u',', u"JSON列表分隔符");
            nodeList = std::make_unique<NodeList>(
                    nodeObjectLeft.get(), node.nodeElement2.get(),
                    nodeObjectSeparator.get(), nodeObjectRight.get());
        }
    };

    template<>
    struct NodeInitialization<NodeTypeId::JSON_STRING> {
        using NodeType = typename NodeTypeDetail<NodeTypeId::JSON_STRING>::Type;
        static void init(NodeType &node, const CPack &cpack) {
            if (HEDLEY_UNLIKELY(node.data.has_value())) {
                for (const auto &item:  node.data.value()) {
                    initNode(*item, cpack);
                }
            }
            std::vector<const NodeBase *> nodeDataElement;
            if (HEDLEY_UNLIKELY(node.data.has_value())) {
                nodeDataElement.reserve(node.data.value().size());
                for (const auto &item: node.data.value()) {
                    nodeDataElement.push_back(item.get());
                }
            }
            node.nodeData = std::make_unique<NodeOr>(std::move(nodeDataElement), false);
        }
    };

    void initNode(Node::NodeBase &node, const CPack &cpack) {
        switch (node.nodeTypeId) {
            CODEC_PASTE(CHELPER_INIT, CHELPER_NODE_TYPES)
            default:
                HEDLEY_UNREACHABLE();
        }
    }

}// namespace CHelper::Node
