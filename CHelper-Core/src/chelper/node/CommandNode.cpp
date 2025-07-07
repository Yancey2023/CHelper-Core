//
// Created by Yancey on 2023/11/10.
//

#include "chelper/node/NodeType.h"

#include <chelper/node/CommandNode.h>

namespace CHelper::Node {

    NodeSerializable::NodeSerializable(const std::optional<std::string> &id,
                                       const std::optional<std::u16string> &description,
                                       bool isMustAfterSpace)
        : id(id),
          description(description),
          isMustAfterSpace(isMustAfterSpace) {}

    bool NodeSerializable::getIsMustAfterSpace() const {
        return isMustAfterSpace.value_or(false);
    }

    NodeSingleSymbol NodeBlock::nodeBlockStateLeftBracket(u'[', u"方块状态左括号");

    NodeSingleSymbol NodeCommand::nodeCommandStart(u'/', u"命令开始字符");

    NodeCommand::NodeCommand(const std::optional<std::string> &id,
                             const std::optional<std::u16string> &description,
                             std::vector<Node::NodePerCommand> *commands)
        : NodeSerializable(id, description, false),
          commands(commands) {}

    NodeInteger NodeItem::nodeCount("ITEM_COUNT", u"物品数量", 0, std::nullopt);
    NodeInteger NodeItem::nodeAllData("ITEM_DATA", u"物品附加值", -1, std::nullopt);

    NodeJson::NodeJson(const std::optional<std::string> &id,
                       const std::optional<std::u16string> &description,
                       std::string key)
        : NodeSerializable(id, description, false),
          key(std::move(key)) {}

    NodeLF::NodeLF(const std::optional<std::string> &id,
                   const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {}

    NodeWrapped *NodeLF::getInstance() {
        static NodeLF INSTANCE("LF", u"命令终止");
        static NodeWrapped INSTANCE_WRAPPED(INSTANCE);
        return &INSTANCE_WRAPPED;
    }

    NodeNamespaceId::NodeNamespaceId(const std::optional<std::string> &id,
                                     const std::optional<std::u16string> &description,
                                     const std::optional<std::string> &key,
                                     bool ignoreError)
        : NodeSerializable(id, description, false),
          key(key),
          ignoreError(ignoreError) {}

    NodeNormalId::NodeNormalId(
            const std::optional<std::string> &id,
            const std::optional<std::u16string> &description,
            const std::string &key,
            bool ignoreError,
            bool allowMissingID,
            const std::function<ASTNode(const NodeWithType &node, TokenReader &tokenReader)> &getNormalIdASTNode)
        : NodeSerializable(id, description, false),
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
            const std::function<ASTNode(const NodeWithType &node, TokenReader &tokenReader)> &getNormalIdASTNode)
        : NodeSerializable(id, description, false),
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
        : NodeSerializable(id, description, false) {}

    NodeRange::NodeRange(const std::optional<std::string> &id,
                         const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {}

    NodeSingleSymbol NodeRelativeFloat::nodeRelativeNotation(u'~', u"相对坐标（~x ~y ~z）", false);
    NodeSingleSymbol NodeRelativeFloat::nodeCaretNotation(u'^', u"局部坐标（^左 ^上 ^前）", false);
    NodeOr NodeRelativeFloat::nodePreSymbol({nodeRelativeNotation, nodeCaretNotation}, false);

    NodeRelativeFloat::NodeRelativeFloat(const std::optional<std::string> &id,
                                         const std::optional<std::u16string> &description,
                                         bool canUseCaretNotation)
        : NodeSerializable(id, description, false),
          canUseCaretNotation(canUseCaretNotation) {}

    NodeString::NodeString(const std::optional<std::string> &id,
                           const std::optional<std::u16string> &description,
                           bool allowMissingString,
                           const bool canContainSpace,
                           const bool ignoreLater)
        : NodeSerializable(id, description, false),
          allowMissingString(allowMissingString),
          canContainSpace(canContainSpace),
          ignoreLater(ignoreLater) {}

    NodeString NodeTargetSelector::nodePlayerName("TARGET_SELECTOR_PLAYER_NAME", u"玩家名字", false, true, false);
    NodeSingleSymbol NodeTargetSelector::nodeWildcard(u'*', u"所有正被记分板跟踪的实体");
    NodeNormalId NodeTargetSelector::nodeTargetSelectorVariable(
            "TARGET_SELECTOR_VARIABLE", u"目标选择器变量",
            std::make_shared<std::vector<std::shared_ptr<NormalId>>>(std::vector<std::shared_ptr<NormalId>>{
                    NormalId::make(u"@e", u"选择所有实体(只选择活着的实体)"),
                    NormalId::make(u"@a", u"选择所有玩家(无论死活)"),
                    NormalId::make(u"@r", u"选择一名随机玩家(可通过type选择非玩家实体)(只选择活着的实体)"),
                    NormalId::make(u"@p", u"选择最近的玩家(若距离相同，会在其中选择最晚进入服务器的玩家)"),
                    NormalId::make(u"@s", u"命令的执行者(只选择唯一一个实体)(包括已死亡玩家)"),
                    NormalId::make(u"@initiator", u"选择当前与该NPC进行交互(在NPC内置的命令界面中使用)")}),
            true, false,
            [](const NodeWithType &node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readStringASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    NodeSingleSymbol NodeTargetSelector::nodeSeparator(u',', u"目标选择器参数分隔符");
    NodeString NodeTargetSelector::nodeString("TARGET_SELECTOR_ARGUMENT_STRING", u"目标选择器参数(字符串)", true, true, false);
    NodeBoolean NodeTargetSelector::nodeBoolean("BOOLEAN", u"布尔值", std::nullopt, std::nullopt);
    NodeRelativeFloat NodeTargetSelector::nodeRelativeFloat("TARGET_SELECTOR_ARGUMENT_RELATIVE_FLOAT", u"目标选择器参数(相对坐标)", false);

    NodeText::NodeText(const std::optional<std::string> &id,
                       const std::optional<std::u16string> &description,
                       const std::shared_ptr<NormalId> &data,
                       const std::function<ASTNode(const NodeWithType &node, TokenReader &tokenReader)> &getTextASTNode)
        : NodeSerializable(id, description, false),
          data(data),
          getTextASTNode(getTextASTNode) {}

    NodeAnd::NodeAnd(std::vector<NodeWithType> childNodes)
        : childNodes(std::move(childNodes)) {
#ifdef CHelperDebug
        if (this->childNodes.empty()) {
            throw std::runtime_error("childNodes is empty");
        }
        for (const auto &item: this->childNodes) {
            if (HEDLEY_UNLIKELY(item.data == nullptr)) {
                throw std::runtime_error("null node in node or");
            }
        }
#endif
    }

    NodeAny::NodeAny() {
        static NodeSingleSymbol nodeLeft1(u'{', u"左括号");
        static NodeSingleSymbol nodeRight1(u'}', u"右括号");
        static NodeSingleSymbol nodeLeft2(u'[', u"左括号");
        static NodeSingleSymbol nodeRight2(u']', u"右括号");
        static NodeRange nodeRange("RANGE", u"范围");
        nodeEntry = NodeEntry(NodeTargetSelector::nodeString, NodeEqualEntry::nodeEqualOrNotEqual, *this);
        nodeObject = NodeList(nodeLeft1, nodeEntry, NodeTargetSelector::nodeSeparator, nodeRight1);
        nodeList = NodeList(nodeLeft2, *this, NodeTargetSelector::nodeSeparator, nodeRight2);
        nodeAny = NodeOr({NodeTargetSelector::nodeRelativeFloat, NodeTargetSelector::nodeBoolean, NodeTargetSelector::nodeString, nodeObject, nodeRange, nodeList}, false);
    }

    NodeWithType NodeAny::getNodeAny() {
        static NodeAny node;
        return node;
    }

    NodeEntry::NodeEntry(NodeWithType nodeKey,
                         NodeWithType nodeSeparator,
                         NodeWithType nodeValue)
        : nodeKey(nodeKey),
          nodeSeparator(nodeSeparator),
          nodeValue(nodeValue) {}

    EqualData::EqualData(std::u16string name,
                         const std::optional<std::u16string> &description,
                         bool canUseNotEqual,
                         NodeWithType nodeValue)
        : name(std::move(name)),
          description(description),
          canUseNotEqual(canUseNotEqual),
          nodeValue(nodeValue) {}

    NodeText NodeEqualEntry::nodeEqual(
            "TARGET_SELECTOR_ARGUMENT_EQUAL", u"等于",
            NormalId::make(u"=", u"等于"),
            [](const NodeWithType &node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readSymbolASTNode(node);
            });
    NodeText NodeEqualEntry::nodeNotEqual(
            "TARGET_SELECTOR_ARGUMENT_NOT_EQUAL", u"不等于",
            NormalId::make(u"=!", u"不等于"),
            [](const NodeWithType &node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readSymbolASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    NodeOr NodeEqualEntry::nodeEqualOrNotEqual({nodeEqual, nodeNotEqual}, false);

    NodeEqualEntry::NodeEqualEntry(std::vector<EqualData> equalDatas)
        : equalDatas(std::move(equalDatas)) {
        nodeKeyContent = std::make_shared<std::vector<std::shared_ptr<NormalId>>>();
        for (const auto &item: this->equalDatas) {
            nodeKeyContent->push_back(NormalId::make(item.name, item.description));
        }
        nodeKey = NodeNormalId("KEY", u"参数名", nodeKeyContent, true);
    }

    NodeList::NodeList(const NodeWithType &nodeLeft,
                       const NodeWithType &nodeElement,
                       const NodeWithType &nodeSeparator,
                       const NodeWithType &nodeRight)
        : nodeLeft(nodeLeft),
          nodeElement(nodeElement),
          nodeSeparator(nodeSeparator),
          nodeRight(nodeRight),
          nodeElementOrRight({nodeElement, nodeRight}, false),
          nodeSeparatorOrRight({nodeSeparator, nodeRight}, false) {
#ifdef CHelperDebug
        if (HEDLEY_UNLIKELY(nodeLeft.data == nullptr || nodeElement.data == nullptr || nodeSeparator.data == nullptr || nodeRight.data == nullptr)) {
            throw std::runtime_error("NodeOr has a null child node");
        }
#endif
    }

    NodeOr::NodeOr(std::vector<NodeWithType> childNodes,
                   bool isAttachToEnd,
                   bool isUseFirst,
                   bool noSuggestion,
                   const char16_t *defaultErrorReason,
                   ASTNodeId::ASTNodeId nodeId)
        : childNodes(std::move(childNodes)),
          isAttachToEnd(isAttachToEnd),
          isUseFirst(isUseFirst),
          noSuggestion(noSuggestion),
          defaultErrorReason(defaultErrorReason),
          nodeId(nodeId) {
#ifdef CHelperDebug
        if (this->childNodes.empty()) {
            throw std::runtime_error("childNodes is empty");
        }
        for (const auto &item: this->childNodes) {
            if (HEDLEY_UNLIKELY(item.data == nullptr)) {
                throw std::runtime_error("null node in node or");
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
        : symbol(symbol),
          normalId(getNormalId(symbol, description)),
          isAddSpace(isAddSpace) {}

    NodeOptional::NodeOptional(NodeWithType optionalNode)
        : optionalNode(optionalNode) {}

#ifdef CHelperDebug
#define CHELPER_IS_BASE_OF_NODE_SERIALIZABLE(v1) \
    case Node::NodeTypeId::v1:                   \
        return std::is_base_of_v<NodeSerializable, NodeTypeDetail<Node::NodeTypeId::v1>::Type>;

    bool isNodeSerializable(NodeWithType innerNode) {
        switch (innerNode.nodeTypeId) {
            CODEC_PASTE(CHELPER_IS_BASE_OF_NODE_SERIALIZABLE, CHELPER_NODE_TYPES)
            default:
                return false;
        }
    }
#endif

    NodeWrapped::NodeWrapped(NodeWithType innerNode)
        : innerNode(innerNode) {
#ifdef CHelperDebug
        if (!isNodeSerializable(innerNode)) {
            throw std::runtime_error("invalid innerNode in NodeWrapped");
        }
#endif
    }

    [[nodiscard]] NodeSerializable &NodeWrapped::getNodeSerializable() const {
        return *reinterpret_cast<NodeSerializable *>(innerNode.data);
    }

    NodeWithType NodeJsonElement::getNodeJsonElement() {
        static NodeJsonString jsonString("JSON_STRING", u"JSON字符串");
        static NodeJsonInteger jsonInteger("JSON_INTEGER", u"JSON整数", std::nullopt, std::nullopt);
        static NodeJsonFloat jsonFloat("JSON_FLOAT", u"JSON小数", std::nullopt, std::nullopt);
        static NodeJsonNull jsonNull("JSON_NULL", u"JSON空值");
        static NodeJsonBoolean jsonBoolean("JSON_BOOLEAN", u"JSON布尔值", std::nullopt, std::nullopt);
        static NodeJsonList jsonList("JSON_LIST", u"JSON列表");
        static NodeJsonObject jsonObject("JSON_OBJECT", u"JSON对象");
        static NodeOr jsonElement(
                {jsonBoolean, jsonFloat, jsonInteger, jsonNull, jsonString, jsonList, jsonObject},
                false, false, true,
                u"类型不匹配，当前内容不是有效的JSON元素");
        return jsonElement;
    }

    NodeSingleSymbol NodeJsonEntry::nodeSeparator(u':', u"冒号");
    static NodeJsonString jsonString("JSON_STRING", u"JSON字符串");

    NodeEntry NodeJsonEntry::nodeAllEntry(jsonString, nodeSeparator, NodeJsonElement::getNodeJsonElement());

    NodeJsonEntry::NodeJsonEntry(const std::optional<std::string> &id,
                                 const std::optional<std::u16string> &description,
                                 std::u16string key,
                                 std::vector<std::string> value)
        : NodeSerializable(id, description, false),
          key(std::move(key)),
          value(std::move(value)) {}

    NodeWithType NodeJsonEntry::getNodeJsonAllEntry() {
        static NodeJsonEntry nodeJsonAllEntry("NODE_JSON_ENTRY", u"JSON对象键值对");
        return nodeJsonAllEntry;
    }

    NodeSingleSymbol NodeJsonList::nodeLeft(u'[', u"JSON列表左括号");
    NodeSingleSymbol NodeJsonList::nodeRight(u']', u"JSON列表右括号");
    NodeSingleSymbol NodeJsonList::nodeSeparator(u',', u"JSON列表分隔符");
    NodeList NodeJsonList::nodeAllList(nodeLeft, NodeJsonElement::getNodeJsonElement(), nodeSeparator, nodeRight);

    NodeJsonList::NodeJsonList(const std::optional<std::string> &id,
                               const std::optional<std::u16string> &description,
                               std::string data)
        : NodeSerializable(id, description, false),
          data(std::move(data)) {}

    NodeJsonNull::NodeJsonNull(const std::optional<std::string> &id,
                               const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {}

    NodeJsonObject::NodeJsonObject(const std::optional<std::string> &id,
                                   const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {
        nodeElement1 = std::nullopt;
        nodeElement2 = NodeOr({NodeJsonEntry::getNodeJsonAllEntry()}, false, true);
        static NodeSingleSymbol nodeListLeft(u'{', u"JSON列表左括号");
        static NodeSingleSymbol nodeListRight(u'}', u"JSON列表右括号");
        static NodeSingleSymbol nodeListSeparator(u',', u"JSON列表分隔符");
        nodeList = NodeList(nodeListLeft, nodeElement2, nodeListSeparator, nodeListRight);
    }

    NodeJsonString::NodeJsonString(const std::optional<std::string> &id,
                                   const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {}

}// namespace CHelper::Node
