//
// Created by Yancey on 2023/11/10.
//

#include "NodeBase.h"
#include "param/NodeLF.h"

namespace CHelper::Node {

    NodeBase::NodeBase(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       bool isMustAfterWhiteSpace)
            : id(id),
              description(description),
              isMustAfterWhiteSpace(isMustAfterWhiteSpace) {}

    NodeBase::NodeBase(const nlohmann::json &j,
                       bool isMustAfterWhiteSpace)
            : id(JsonUtil::fromJsonOptionalLikely<std::string>(j, "id")),
              brief(JsonUtil::fromJsonOptionalLikely<std::string>(j, "brief")),
              description(JsonUtil::fromJsonOptionalLikely<std::string>(j, "description")),
              isMustAfterWhiteSpace(j.value("isMustAfterWhiteSpace", isMustAfterWhiteSpace)) {
#ifdef HEDLEY_GCC_VERSION
#if CHelperDebug
        if (HEDLEY_UNLIKELY(!description.has_value() || brief.has_value())) {
            return;
        }
        auto type = JsonUtil::fromJsonOptionalLikely<std::string>(j, "type");
        if (HEDLEY_UNLIKELY(!type.has_value() || type.value() == "PER_COMMAND" || type.value() == "TARGET_SELECTOR" ||
                            type.value() == "JSON_OBJECT" || type.value() == "JSON_INTEGER" ||
                            type.value() == "JSON_FLOAT" ||
                            type.value() == "JSON_NULL" || type.value() == "JSON_LIST" ||
                            type.value() == "JSON_STRING" ||
                            type.value() == "JSON_BOOLEAN" || type.value() == "TEXT" || type.value() == "BLOCK" ||
                            type.value() == "ITEM")) {
            return;
        }

        static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        if (HEDLEY_UNLIKELY(converter.from_bytes(description.value()).length() > 5)) {
            CHELPER_WARN("节点介绍长度过长 " + type.value_or("") + " " + description.value());
        }
#endif
#endif
    }

    std::unique_ptr<NodeBase> NodeBase::getNodeFromJson(const nlohmann::json &j,
                                                        const CPack &cpack) {
        Profile::push(ColorStringBuilder().red("loading type").build());
        auto type = JsonUtil::fromJson<std::string>(j, "type");
        auto id = JsonUtil::fromJsonOptionalLikely<std::string>(j, "id");
        Profile::next(ColorStringBuilder().red("loading node ").purple(type).build());
        if (HEDLEY_LIKELY(id.has_value())) {
            Profile::next(ColorStringBuilder()
                                  .red("loading node ")
                                  .purple(type)
                                  .red(" with id \"")
                                  .purple(id.value())
                                  .red("\"")
                                  .build());
        } else {
            Profile::next(ColorStringBuilder()
                                  .red("loading node ")
                                  .purple(type)
                                  .red(" without id")
                                  .build());
        }
        for (const auto &item: NodeType::NODE_TYPES) {
            if (HEDLEY_UNLIKELY(item->nodeName == type)) {
                Profile::pop();
                return item->createNodeByJson(j, cpack);
            }
        }
        throw Exception::UnknownNodeType(type);
    }

    NodeType *NodeBase::getNodeType() const {
        return NodeType::UNKNOWN.get();
    }

    void NodeBase::toJson(nlohmann::json &j) const {
        JsonUtil::toJsonOptionalLikely(j, "id", id);
        JsonUtil::toJsonOptionalUnlikely(j, "brief", brief);
        JsonUtil::toJsonOptionalLikely(j, "description", description);
        JsonUtil::toJson(j, "type", getNodeType()->nodeName);
        //TODO 需要更好的判断isMustAfterWhiteSpace是否输出的逻辑
        JsonUtil::toJson(j, "isMustAfterWhiteSpace", isMustAfterWhiteSpace);
    }

    ASTNode NodeBase::getASTNodeWithNextNode(TokenReader &tokenReader, const CPack *cpack) const {
        //空格检测
        tokenReader.push();
        if (HEDLEY_UNLIKELY(isMustAfterWhiteSpace && tokenReader.skipWhitespace() == 0)) {
            VectorView <Token> tokens = tokenReader.collect();
            return ASTNode::simpleNode(this, tokens, ErrorReason::requireWhiteSpace(tokens), "compound");
        }
        tokenReader.pop();
        tokenReader.push();
        //当前节点
        DEBUG_GET_NODE_BEGIN(this)
        ASTNode currentASTNode = getASTNode(tokenReader, cpack);
        DEBUG_GET_NODE_END(this)
        if (HEDLEY_UNLIKELY(currentASTNode.isError() || nextNodes.empty())) {
            return ASTNode::andNode(this, {std::move(currentASTNode)}, tokenReader.collect(), nullptr, "compound");
        }
        //子节点
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(nextNodes.size());
        for (const auto &item: nextNodes) {
            tokenReader.push();
            childASTNodes.push_back(item->getASTNodeWithNextNode(tokenReader, cpack));
            tokenReader.restore();
        }
        tokenReader.push();
        tokenReader.skipToLF();
        ASTNode nextASTNode = ASTNode::orNode(this, std::move(childASTNodes), tokenReader.collect(), nullptr,
                                              "nextNode");
        return ASTNode::andNode(this, {std::move(currentASTNode), std::move(nextASTNode)}, tokenReader.collect(),
                                nullptr, "compound");
    }

    ASTNode NodeBase::getByChildNode(TokenReader &tokenReader,
                                     const CPack *cpack,
                                     const NodeBase *childNode,
                                     const std::string &astNodeId) const {
        ASTNode node = childNode->getASTNode(tokenReader, cpack);
        VectorView <Token> tokens = node.tokens;
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
                                         const std::string &astNodeId) const {
        tokenReader.push();
        std::vector<ASTNode> childASTNodes;
        for (const auto &item: childNodes) {
            tokenReader.push();
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(item)
            ASTNode astNode = item->getASTNodeWithNextNode(tokenReader, cpack);
            DEBUG_GET_NODE_END(item)
            bool isError = astNode.isError();
            const VectorView <Token> tokens = tokenReader.collect();
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

    std::optional<std::string> NodeBase::collectDescription(const ASTNode *node, size_t index) const {
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

} // CHelper::Node
