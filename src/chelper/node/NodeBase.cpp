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
              description(JsonUtil::fromJsonOptionalLikely<std::string>(j, "description")),
              isMustAfterWhiteSpace(j.value("isMustAfterWhiteSpace", isMustAfterWhiteSpace)) {
//#if CHelperDebug
//        if (!description.has_value()) {
//            return;
//        }
//        auto type = fromJsonOptional<>(j, type, std::string);
//        if (!type.has_value() || type.value() == "PER_COMMAND" || type.value() == "TARGET_SELECTOR") {
//            return;
//        }
//        static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
//        if (converter.from_bytes(description.value()).length() > 5) {
//            CHELPER_WARN("节点介绍长度过长 " + description.value());
//        }
//#endif
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
            if (item->nodeName == type) {
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
        JsonUtil::toJsonOptionalLikely(j, "description", description);
        JsonUtil::toJson(j, "type", getNodeType()->nodeName);
        //TODO 需要更好的判断isMustAfterWhiteSpace是否输出的逻辑
        JsonUtil::toJson(j, "isMustAfterWhiteSpace", isMustAfterWhiteSpace);
    }

    ASTNode NodeBase::getASTNodeWithNextNode(TokenReader &tokenReader, const CPack *cpack) const {
        //空格检测
        tokenReader.push();
        if (isMustAfterWhiteSpace && tokenReader.skipWhitespace() == 0) {
            VectorView <Token> tokens = tokenReader.collect();
            return ASTNode::simpleNode(this, tokens, ErrorReason::requireWhiteSpace(tokens));
        }
        tokenReader.pop();
        tokenReader.push();
        //当前节点
        DEBUG_GET_NODE_BEGIN(this)
        ASTNode currentASTNode = getASTNode(tokenReader, cpack);
        DEBUG_GET_NODE_END(this)
        if (currentASTNode.isError() || nextNodes.empty()) {
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
            if (isError && (isIgnoreChildNodesError || tokens.isEmpty())) {
                tokenReader.restore();
                break;
            }
            childASTNodes.push_back(std::move(astNode));
            tokenReader.pop();
            if (isError) {
                break;
            }
        }
        return ASTNode::andNode(this, std::move(childASTNodes), tokenReader.collect(), nullptr, astNodeId);
    }

    std::optional<std::string> NodeBase::collectDescription(const ASTNode *node, size_t index) const {
#if CHelperDebug == true
        if (!description.has_value()) {
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

    void NodeBase::collectStructureNormal(StructureBuilder &structure, bool isMustHave) const {
        collectStructure(nullptr, structure, isMustHave);
        if (nextNodes.empty()) {
            return;
        }
        for (const auto &item: nextNodes) {
            if (item == NodeLF::getInstance()) {
                isMustHave = false;
            }
        }
        nextNodes[0]->collectStructureNormal(structure, isMustHave);
    }

} // CHelper::Node
