//
// Created by Yancey on 2023/11/10.
//

#include <chelper/node/CommandNode.h>

namespace CHelper::Node {

#define CHELPER_DELETE(v1)                                                           \
    case Node::NodeTypeId::v1:                                                       \
        delete reinterpret_cast<NodeTypeDetail<Node::NodeTypeId::v1>::Type *>(this); \
        break;

    NodeBase::NodeBase(NodeTypeId::NodeTypeId nodeTypeId)
        : nodeTypeId(nodeTypeId) {}

    NodeBase::~NodeBase() {
        if (nodeTypeId > MAX_TYPE_ID) {
#ifdef CHelperDebug
                SPDLOG_ERROR("Unknown node type id: {}", static_cast<uint8_t>(nodeTypeId));
#else
                HEDLEY_UNREACHABLE();
#endif
        }
    }

    NodeSerializable::NodeSerializable(NodeTypeId::NodeTypeId nodeTypeId,
                                       const std::optional<std::string> &id,
                                       const std::optional<std::u16string> &description,
                                       bool isMustAfterSpace)
        : NodeBase(nodeTypeId),
          id(id),
          description(description),
          isMustAfterSpace(isMustAfterSpace) {}

    bool NodeSerializable::getIsMustAfterSpace() const {
        return isMustAfterSpace.value_or(false);
    }

    std::unique_ptr<Node::NodeSingleSymbol> NodeBlock::nodeBlockStateLeftBracket = std::make_unique<Node::NodeSingleSymbol>(u'[', u"方块状态左括号");

    std::unique_ptr<Node::NodeSingleSymbol> NodeCommand::nodeCommandStart = std::make_unique<Node::NodeSingleSymbol>(u'/', u"命令开始字符");

    NodeCommand::NodeCommand(const std::optional<std::string> &id,
                             const std::optional<std::u16string> &description,
                             std::vector<std::unique_ptr<Node::NodePerCommand>> *commands)
        : NodeSerializable(NodeTypeId::COMMAND, id, description, false),
          commands(commands) {}

    std::shared_ptr<NodeBase> NodeItem::nodeCount = NodeInteger::make("ITEM_COUNT", u"物品数量", 0, std::nullopt);
    std::shared_ptr<NodeBase> NodeItem::nodeAllData = NodeInteger::make("ITEM_DATA", u"物品附加值", -1, std::nullopt);

    NodeJson::NodeJson(const std::optional<std::string> &id,
                       const std::optional<std::u16string> &description,
                       std::string key)
        : NodeSerializable(NodeTypeId::JSON, id, description, false),
          key(std::move(key)) {}

    NodeLF::NodeLF(const std::optional<std::string> &id,
                   const std::optional<std::u16string> &description)
        : NodeSerializable(NodeTypeId::LF, id, description, false) {}

    NodeWrapped *NodeLF::getInstance() {
        static NodeLF INSTANCE = NodeLF("LF", u"命令终止");
        static std::unique_ptr<NodeWrapped> INSTANCE_WRAPPED = std::make_unique<NodeWrapped>(&INSTANCE);
        return INSTANCE_WRAPPED.get();
    }

    NodeNamespaceId::NodeNamespaceId(const std::optional<std::string> &id,
                                     const std::optional<std::u16string> &description,
                                     const std::optional<std::string> &key,
                                     bool ignoreError)
        : NodeSerializable(NodeTypeId::NAMESPACE_ID, id, description, false),
          key(key),
          ignoreError(ignoreError) {}

    NodeNormalId::NodeNormalId(
            const std::optional<std::string> &id,
            const std::optional<std::u16string> &description,
            const std::string &key,
            bool ignoreError,
            bool allowMissingID,
            const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getNormalIdASTNode)
        : NodeSerializable(NodeTypeId::NORMAL_ID, id, description, false),
          key(key),
          ignoreError(ignoreError),
          allowMissingID(allowMissingID),
          getNormalIdASTNode(getNormalIdASTNode) {}

    NodeNormalId::NodeNormalId(
            const std::optional<std::string> &id,
            const std::optional<std::u16string> &description,
            const std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> &contents,
            bool ignoreError,
            bool allowMissingID,
            const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getNormalIdASTNode)
        : NodeSerializable(NodeTypeId::NORMAL_ID, id, description, false),
          contents(contents),
          ignoreError(ignoreError),
          allowMissingID(allowMissingID),
          getNormalIdASTNode(getNormalIdASTNode) {
#ifdef CHelperDebug
        if (HEDLEY_UNLIKELY(contents == nullptr)) {
            throw std::runtime_error("contents should not be nullptr");
        }
#endif
        customContents = contents;
    }

    NodePosition::NodePosition(const std::optional<std::string> &id,
                               const std::optional<std::u16string> &description)
        : NodeSerializable(NodeTypeId::POSITION, id, description, false) {}

    NodeRange::NodeRange(const std::optional<std::string> &id,
                         const std::optional<std::u16string> &description)
        : NodeSerializable(NodeTypeId::RANGE, id, description, false) {}

    std::unique_ptr<NodeSingleSymbol> NodeRelativeFloat::nodeRelativeNotation = std::make_unique<NodeSingleSymbol>(
            u'~', u"相对坐标（~x ~y ~z）", false);
    std::unique_ptr<NodeSingleSymbol> NodeRelativeFloat::nodeCaretNotation = std::make_unique<NodeSingleSymbol>(
            u'^', u"局部坐标（^左 ^上 ^前）", false);
    std::unique_ptr<NodeOr> NodeRelativeFloat::nodePreSymbol = std::make_unique<NodeOr>(
            std::vector<const NodeBase *>{nodeRelativeNotation.get(), nodeCaretNotation.get()}, false);

    NodeRelativeFloat::NodeRelativeFloat(const std::optional<std::string> &id,
                                         const std::optional<std::u16string> &description,
                                         bool canUseCaretNotation)
        : NodeSerializable(NodeTypeId::RELATIVE_FLOAT, id, description, false),
          canUseCaretNotation(canUseCaretNotation) {}

    NodeString::NodeString(const std::optional<std::string> &id,
                           const std::optional<std::u16string> &description,
                           bool allowMissingString,
                           const bool canContainSpace,
                           const bool ignoreLater)
        : NodeSerializable(NodeTypeId::STRING, id, description, false),
          allowMissingString(allowMissingString),
          canContainSpace(canContainSpace),
          ignoreLater(ignoreLater) {}

    std::shared_ptr<NodeBase> NodeTargetSelector::nodePlayerName = std::make_shared<NodeString>(
            "TARGET_SELECTOR_PLAYER_NAME", u"玩家名字", false, true, false);
    std::shared_ptr<NodeBase> NodeTargetSelector::nodeWildcard = std::make_shared<NodeSingleSymbol>(
            u'*', u"所有正被记分板跟踪的实体");
    std::shared_ptr<NodeBase> NodeTargetSelector::nodeAt = std::make_shared<NodeSingleSymbol>(
            u'@', u"@符号");
    std::shared_ptr<NodeBase> NodeTargetSelector::nodeTargetSelectorVariable = std::make_shared<NodeNormalId>(
            "TARGET_SELECTOR_VARIABLE", u"目标选择器变量",
            std::make_shared<std::vector<std::shared_ptr<NormalId>>>(std::vector<std::shared_ptr<NormalId>>{
                    NormalId::make(u"@e", u"选择所有实体(只选择活着的实体)"),
                    NormalId::make(u"@a", u"选择所有玩家(无论死活)"),
                    NormalId::make(u"@r", u"选择一名随机玩家(可通过type选择非玩家实体)(只选择活着的实体)"),
                    NormalId::make(u"@p", u"选择最近的玩家(若距离相同，会在其中选择最晚进入服务器的玩家)"),
                    NormalId::make(u"@s", u"命令的执行者(只选择唯一一个实体)(包括已死亡玩家)"),
                    NormalId::make(u"@initiator", u"选择当前与该NPC进行交互(在NPC内置的命令界面中使用)")}),
            true, false,
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readStringASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    std::shared_ptr<NodeBase> NodeTargetSelector::nodeLeft = std::make_shared<NodeSingleSymbol>(u'[', u"目标选择器参数左括号");

    NodeText::NodeText(const std::optional<std::string> &id,
                       const std::optional<std::u16string> &description,
                       const std::shared_ptr<NormalId> &data,
                       const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getTextASTNode)
        : NodeSerializable(NodeTypeId::TEXT, id, description, false),
          data(data),
          getTextASTNode(getTextASTNode) {}

    NodeAnd::NodeAnd(const std::vector<const NodeBase *> &childNodes)
        : NodeBase(NodeTypeId::TEXT),
          childNodes(childNodes) {
#ifdef CHelperDebug
        for (const auto &item: this->childNodes) {
            if (HEDLEY_UNLIKELY(item == nullptr)) {
                throw std::runtime_error("null node in node or");
            }
            if (HEDLEY_UNLIKELY(item->nodeTypeId > MAX_TYPE_ID)) {
                throw std::runtime_error("unknown node type");
            }
        }
#endif
    }

    NodeAny::NodeAny()
        : NodeBase(NodeTypeId::ANY) {}

    NodeAny *NodeAny::getNodeAny() {
        static NodeAny node;
        return &node;
    }

    NodeEntry::NodeEntry(NodeBase *nodeKey,
                         NodeBase *nodeSeparator,
                         NodeBase *nodeValue)
        : NodeBase(NodeTypeId::ENTRY),
          nodeKey(nodeKey),
          nodeSeparator(nodeSeparator),
          nodeValue(nodeValue) {}

    EqualData::EqualData(std::u16string name,
                         const std::optional<std::u16string> &description,
                         bool canUseNotEqual,
                         const NodeBase *nodeValue)
        : name(std::move(name)),
          description(description),
          canUseNotEqual(canUseNotEqual),
          nodeValue(nodeValue) {}

    std::unique_ptr<NodeBase> NodeEqualEntry::nodeEqual = std::make_unique<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_EQUAL", u"等于",
            NormalId::make(u"=", u"等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readSymbolASTNode(node);
            });
    std::unique_ptr<NodeBase> NodeEqualEntry::nodeNotEqual = std::make_unique<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_NOT_EQUAL", u"不等于",
            NormalId::make(u"=!", u"不等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readSymbolASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    std::unique_ptr<NodeBase> NodeEqualEntry::nodeEqualOrNotEqual = std::make_unique<NodeOr>(
            std::vector<const NodeBase *>{
                    nodeEqual.get(), nodeNotEqual.get()},
            false);

    NodeEqualEntry::NodeEqualEntry(std::vector<EqualData> equalDatas)
        : NodeBase(NodeTypeId::EQUAL_ENTRY),
          equalDatas(std::move(equalDatas)) {
        nodeKeyContent = std::make_shared<std::vector<std::shared_ptr<NormalId>>>();
        for (const auto &item: this->equalDatas) {
            nodeKeyContent->push_back(NormalId::make(item.name, item.description));
        }
        nodeKey = std::make_unique<NodeNormalId>("KEY", u"参数名", nodeKeyContent, true);
    }

    NodeList::NodeList(NodeBase *nodeLeft,
                       NodeBase *nodeElement,
                       NodeBase *nodeSeparator,
                       NodeBase *nodeRight)
        : NodeBase(NodeTypeId::LIST),
          nodeLeft(nodeLeft),
          nodeElement(nodeElement),
          nodeSeparator(nodeSeparator),
          nodeRight(nodeRight),
          nodeElementOrRight(std::vector<const NodeBase *>{nodeElement, nodeRight}, false),
          nodeSeparatorOrRight(std::vector<const NodeBase *>{nodeSeparator, nodeRight}, false) {
#ifdef CHelperDebug
        if (HEDLEY_UNLIKELY(
                    nodeLeft == nullptr || nodeElement == nullptr || nodeSeparator == nullptr || nodeRight == nullptr)) {
            throw std::runtime_error("NodeOr has a null child node");
        }
#endif
    }

    NodeOr::NodeOr(std::vector<const NodeBase *> childNodes,
                   const bool isAttachToEnd,
                   const bool isUseFirst,
                   const bool noSuggestion,
                   const char16_t *defaultErrorReason,
                   ASTNodeId::ASTNodeId nodeId)
        : NodeBase(NodeTypeId::OR),
          childNodes(std::move(childNodes)),
          isAttachToEnd(isAttachToEnd),
          isUseFirst(isUseFirst),
          noSuggestion(noSuggestion),
          defaultErrorReason(defaultErrorReason),
          nodeId(nodeId) {
#ifdef CHelperDebug
        for (const auto &item: this->childNodes) {
            if (HEDLEY_UNLIKELY(item == nullptr)) {
                throw std::runtime_error("null node in node or");
            }
            if (HEDLEY_UNLIKELY(item->nodeTypeId > MAX_TYPE_ID)) {
                throw std::runtime_error("unknown node type");
            }
        }
#endif
    }

    static std::shared_ptr<NormalId> getNormalId(char16_t symbol, const std::optional<std::u16string> &description) {
        std::u16string name;
        name.push_back(symbol);
        return NormalId::make(name, description);
    }

    NodeSingleSymbol::NodeSingleSymbol(char16_t symbol,
                                       const std::optional<std::u16string> &description,
                                       bool isAddSpace)
        : NodeBase(NodeTypeId::SINGLE_SYMBOL),
          symbol(symbol),
          normalId(getNormalId(symbol, description)),
          isAddSpace(isAddSpace) {}

    NodeWrapped::NodeWrapped(NodeSerializable *innerNode)
        : NodeBase(NodeTypeId::WRAPPED),
          innerNode(innerNode) {}

    NodeBase *NodeJsonElement::getNodeJsonElement() {
        static std::unique_ptr<NodeBase> jsonString = std::make_unique<NodeJsonString>(
                "JSON_STRING", u"JSON字符串");
        static std::unique_ptr<NodeBase> jsonInteger = NodeJsonInteger::make(
                "JSON_INTEGER", u"JSON整数", std::nullopt, std::nullopt);
        static std::unique_ptr<NodeBase> jsonFloat = NodeJsonFloat::make(
                "JSON_FLOAT", u"JSON小数", std::nullopt, std::nullopt);
        static std::unique_ptr<NodeBase> jsonNull = std::make_unique<NodeJsonNull>(
                "JSON_NULu", u"JSON空值");
        static std::unique_ptr<NodeBase> jsonBoolean = NodeJsonBoolean::make(
                "JSON_BOOLEAN", u"JSON布尔值", std::nullopt, std::nullopt);
        static std::unique_ptr<NodeBase> jsonList = std::make_unique<NodeJsonList>(
                "JSON_LIST", u"JSON列表");
        static std::unique_ptr<NodeBase> jsonObject = std::make_unique<NodeJsonObject>(
                "JSON_OBJECT", u"JSON对象");
        static std::unique_ptr<NodeBase> jsonElement = std::make_unique<NodeOr>(
                std::vector<const NodeBase *>{
                        jsonBoolean.get(), jsonFloat.get(),
                        jsonInteger.get(), jsonNull.get(),
                        jsonString.get(), jsonList.get(),
                        jsonObject.get()},
                false, false, true,
                u"类型不匹配，当前内容不是有效的JSON元素");
        return jsonElement.get();
    }

    static std::unique_ptr<NodeBase> nodeEntrySeparator = std::make_unique<NodeSingleSymbol>(u':', u"冒号");
    static std::unique_ptr<NodeBase> jsonString = std::make_unique<NodeJsonString>("JSON_STRING", u"JSON字符串");

    std::unique_ptr<NodeBase> NodeJsonEntry::nodeAllEntry = std::make_unique<NodeEntry>(
            jsonString.get(), nodeEntrySeparator.get(),
            NodeJsonElement::getNodeJsonElement());

    NodeJsonEntry::NodeJsonEntry(const std::optional<std::string> &id,
                                 const std::optional<std::u16string> &description,
                                 std::u16string key,
                                 std::vector<std::string> value)
        : NodeSerializable(NodeTypeId::JSON_ENTRY, id, description, false),
          key(std::move(key)),
          value(std::move(value)) {}

    NodeBase *NodeJsonEntry::getNodeJsonAllEntry() {
        static NodeJsonEntry nodeJsonAllEntry("NODE_JSON_ENTRY", u"JSON对象键值对");
        return &nodeJsonAllEntry;
    }

    std::unique_ptr<NodeBase> NodeJsonList::nodeLeft = std::make_unique<NodeSingleSymbol>(u'[', u"JSON列表左括号");
    std::unique_ptr<NodeBase> NodeJsonList::nodeRight = std::make_unique<NodeSingleSymbol>(u']', u"JSON列表右括号");
    std::unique_ptr<NodeBase> NodeJsonList::nodeSeparator = std::make_unique<NodeSingleSymbol>(u',', u"JSON列表分隔符");
    std::unique_ptr<NodeBase> NodeJsonList::nodeAllList = std::make_unique<NodeList>(
            nodeLeft.get(), NodeJsonElement::getNodeJsonElement(), nodeSeparator.get(), nodeRight.get());

    NodeJsonList::NodeJsonList(const std::optional<std::string> &id,
                               const std::optional<std::u16string> &description,
                               std::string data)
        : NodeSerializable(NodeTypeId::JSON_LIST, id, description, false),
          data(std::move(data)) {}

    NodeJsonNull::NodeJsonNull(const std::optional<std::string> &id,
                               const std::optional<std::u16string> &description)
        : NodeSerializable(NodeTypeId::JSON_NULL, id, description, false) {}

    NodeJsonObject::NodeJsonObject(const std::optional<std::string> &id,
                                   const std::optional<std::u16string> &description)
        : NodeSerializable(NodeTypeId::JSON_OBJECT, id, description, false) {
        nodeElement1 = nullptr;
        std::vector<const NodeBase *> nodeElementData;
        nodeElementData.push_back(NodeJsonEntry::getNodeJsonAllEntry());
        nodeElement2 = std::make_unique<NodeOr>(std::move(nodeElementData), false, true);
        static std::unique_ptr<NodeBase> nodeListLeft = std::make_unique<NodeSingleSymbol>(u'{', u"JSON列表左括号");
        static std::unique_ptr<NodeBase> nodeListRight = std::make_unique<NodeSingleSymbol>(u'}', u"JSON列表右括号");
        static std::unique_ptr<NodeBase> nodeListSeparator = std::make_unique<NodeSingleSymbol>(u',', u"JSON列表分隔符");
        nodeList = std::make_unique<NodeList>(
                nodeListLeft.get(), nodeElement2.get(),
                nodeListSeparator.get(), nodeListRight.get());
    }

    NodeJsonString::NodeJsonString(const std::optional<std::string> &id,
                                   const std::optional<std::u16string> &description)
        : NodeSerializable(NodeTypeId::JSON_STRING, id, description, false) {
        nodeData = std::make_unique<NodeOr>(std::vector<const NodeBase *>(), false);
    }

}// namespace CHelper::Node
