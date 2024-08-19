//
// Created by Yancey on 2023/11/10.
//

#include "NodeBase.h"
#include "param/NodeLF.h"

namespace CHelper::Node {

    NodeBase::NodeBase(const std::optional<std::wstring> &id,
                       const std::optional<std::wstring> &description,
                       bool isMustAfterWhiteSpace)
        : id(id),
          description(description),
          isMustAfterWhiteSpace(isMustAfterWhiteSpace) {}

    void NodeBase::init(const CPack &cpack) {
    }

    ASTNode NodeBase::getASTNodeWithNextNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getASTNodeWithNextNode(tokenReader, cpack, isAfterWhitespace());
    }

    ASTNode NodeBase::getASTNodeWithNextNode(TokenReader &tokenReader, const CPack *cpack, bool isRequireWhitespace) const {
        //空格检测
        tokenReader.push();
        if (HEDLEY_UNLIKELY(isRequireWhitespace && getNodeType() != NodeType::LF.get() && tokenReader.skipWhitespace() == 0)) {
            TokensView tokens = tokenReader.collect();
            return ASTNode::simpleNode(this, tokens, ErrorReason::requireWhiteSpace(tokens), ASTNodeId::COMPOUND);
        }
        tokenReader.pop();
        tokenReader.push();
        //当前节点
        DEBUG_GET_NODE_BEGIN(this)
        ASTNode currentASTNode = getASTNode(tokenReader, cpack);
        DEBUG_GET_NODE_END(this)
        if (HEDLEY_UNLIKELY(currentASTNode.isError() || nextNodes.empty())) {
            return ASTNode::andNode(this, {std::move(currentASTNode)}, tokenReader.collect(), nullptr, ASTNodeId::COMPOUND);
        }
        //子节点
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(nextNodes.size());
        for (const auto &item: nextNodes) {
            tokenReader.push();
            childASTNodes.push_back(item->getASTNodeWithNextNode(tokenReader, cpack, isAfterWhitespace() || item->isAfterWhitespace()));
            tokenReader.restore();
        }
        tokenReader.push();
        tokenReader.skipToLF();
        ASTNode nextASTNode = ASTNode::orNode(this, std::move(childASTNodes), tokenReader.collect(), nullptr, ASTNodeId::NEXT_NODE);
        return ASTNode::andNode(this, {std::move(currentASTNode), std::move(nextASTNode)}, tokenReader.collect(), nullptr, ASTNodeId::COMPOUND);
    }

    ASTNode NodeBase::getByChildNode(TokenReader &tokenReader,
                                     const CPack *cpack,
                                     const NodeBase *childNode,
                                     const ASTNodeId::ASTNodeId &astNodeId) const {
        ASTNode node = childNode->getASTNode(tokenReader, cpack);
        TokensView tokens = node.tokens;
        return ASTNode::andNode(this, {std::move(node)}, tokens, nullptr, astNodeId);
    }

    /**
     * 节点不一定有
     *
     * @param isIgnoreChildNodesError true - 第一个错误节点到后面都不算做子节点
     *                                false - 第一个内容为空的错误节点到后面都不算做子节点
     */
    ASTNode NodeBase::getOptionalASTNode(TokenReader &tokenReader,
                                         const CPack *cpack,
                                         bool isIgnoreChildNodesError,
                                         const std::vector<const NodeBase *> &childNodes,
                                         const ASTNodeId::ASTNodeId &astNodeId) const {
        tokenReader.push();
        std::vector<ASTNode> childASTNodes;
        for (const auto &item: childNodes) {
            tokenReader.push();
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(item)
            ASTNode astNode = item->getASTNodeWithNextNode(tokenReader, cpack);
            DEBUG_GET_NODE_END(item)
            bool isError = astNode.isError();
            const TokensView tokens = tokenReader.collect();
            if (HEDLEY_UNLIKELY(isError && (isIgnoreChildNodesError || tokens.isEmpty()))) {
                tokenReader.restore();
                break;
            }
            childASTNodes.push_back(std::move(astNode));
            tokenReader.pop();
            if (HEDLEY_UNLIKELY(isError)) {
                break;
            }
        }
        return ASTNode::andNode(this, std::move(childASTNodes), tokenReader.collect(), nullptr, astNodeId);
    }

    std::optional<std::wstring> NodeBase::collectDescription(const ASTNode *node, size_t index) const {
#if CHelperDebug == true
        if (HEDLEY_UNLIKELY(!description.has_value())) {
            CHELPER_WARN("description is null");
        }
#endif
        return description;
    }

    //创建AST节点的时候只得到了结构的错误，ID的错误需要调用这个方法得到
    bool NodeBase::collectIdError(const ASTNode *astNode,
                                  std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        return false;
    }

    bool NodeBase::collectSuggestions(const ASTNode *astNode,
                                      size_t index,
                                      std::vector<Suggestions> &suggestions) const {
        return false;
    }

    void NodeBase::collectStructure(const ASTNode *astNode,
                                    StructureBuilder &structure,
                                    bool isMustHave) const {
    }

    bool NodeBase::collectColor(const ASTNode *astNode,
                                ColoredString &coloredString,
                                const Theme &theme) const {
        return false;
    }

    void NodeBase::collectStructureWithNextNodes(StructureBuilder &structure, bool isMustHave) const {
        if (HEDLEY_UNLIKELY(brief.has_value())) {
            structure.append(isMustHave, brief.value());
        } else {
            structure.isDirty = false;
            collectStructure(nullptr, structure, isMustHave);
            if (HEDLEY_UNLIKELY(!structure.isDirty)) {
                structure.appendUnknown(isMustHave);
                structure.isDirty = false;
                return;
            }
        }
        if (HEDLEY_UNLIKELY(nextNodes.empty())) {
            return;
        }
        if (HEDLEY_LIKELY(isMustHave)) {
            for (const auto &item: nextNodes) {
                if (HEDLEY_UNLIKELY(item == NodeLF::getInstance())) {
                    isMustHave = false;
                    break;
                }
            }
        }
        nextNodes[0]->collectStructureWithNextNodes(structure, isMustHave);
    }

    bool NodeBase::isAfterWhitespace() const {
        return isMustAfterWhiteSpace.value_or(false);
    }

    CODEC(NodeBase, id, brief, description, isMustAfterWhiteSpace)

#if CHelperOnlyReadBinary != true
    void from_json(const nlohmann::json &j, std::unique_ptr<NodeBase> &t) {
        Profile::push("loading type");
        auto type = JsonUtil::read<std::wstring>(j, "type");
        auto id = JsonUtil::read<std::optional<std::wstring>>(j, "id");
        Profile::next("loading node {}", type);
        if (HEDLEY_LIKELY(id.has_value())) {
            Profile::next("loading node {} with id \"{}\"", type, id.value());
        } else {
            Profile::next("loading node {} without id", type);
        }
        for (const auto &item: NodeType::NODE_TYPES) {
            if (HEDLEY_UNLIKELY(item->nodeName == type)) {
                item->decodeByJson(j, t);
                Profile::pop();
                return;
            }
        }
        Profile::next("unknown node type -> {}", type);
        throw std::runtime_error("unknown node type");
    }

    void to_json(nlohmann::json &j, const std::unique_ptr<NodeBase> &t) {
        t->getNodeType()->encodeByJson(j, t);
        JsonUtil::encode(j, "type", t->getNodeType()->nodeName);
    }
#endif

    void from_binary(BinaryReader &binaryReader, std::unique_ptr<NodeBase> &t) {
        auto typeId = binaryReader.read<uint8_t>();
        if (HEDLEY_UNLIKELY(typeId > NodeType::NODE_TYPES.size())) {
            throw std::runtime_error("unknown typeId");
        }
        const NodeType *type = NodeType::NODE_TYPES[typeId];
        type->decodeByBinary(binaryReader, t);
    }

#if CHelperOnlyReadBinary != true
    void to_binary(BinaryWriter &binaryWriter, const std::unique_ptr<NodeBase> &t) {
        binaryWriter.encode(t->getNodeType()->id);
        t->getNodeType()->encodeByBinary(binaryWriter, t);
    }
#endif

}// namespace CHelper::Node
