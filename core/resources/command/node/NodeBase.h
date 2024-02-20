//
// Created by Yancey666 on 2023/11/10.
//

#ifndef CHELPER_NODEBASE_H
#define CHELPER_NODEBASE_H

#include <pch.h>
#include "../type/NodeType.h"
#include "../../../parser/ASTNode.h"
#include "../../../parser/TokenReader.h"

#define NODE_TYPE_H                                                                                                                     \
static NodeType TYPE;                                                                                                                   \
                                                                                                                                        \
NodeType getNodeType() const override;

#define NODE_TYPE(nodeName, nodeClass)                                                                                                  \
NodeType nodeClass::TYPE(nodeName, [](const nlohmann::json &j, const CPack &cpack) -> std::shared_ptr<NodeBase> {                       \
    return std::make_shared<nodeClass>(j, cpack);                                                                                       \
});                                                                                                                                     \
\
NodeType nodeClass::getNodeType() const {                                                                                               \
    return nodeClass::TYPE;                                                                                                             \
}

#define NODE_TYPE_INSTANCE_H(nodeClass)                                                                                                 \
nodeClass(const std::optional<std::string> &id,                                                                                         \
          const std::optional<std::string> &description);                                                                               \
                                                                                                                                        \
static NodeType TYPE;                                                                                                                   \
                                                                                                                                        \
NodeType getNodeType() const override;                                                                                                  \
                                                                                                                                        \
static std::shared_ptr<nodeClass> getInstance();

#define NODE_TYPE_INSTANCE(nodeName, nodeClass, nodeDescription)                                                                        \
                                                                                                                                        \
nodeClass::nodeClass(const std::optional<std::string> &id,                                                                              \
                     const std::optional<std::string> &description)                                                                     \
        : NodeBase(id, description) {}                                                                                                  \
                                                                                                                                        \
NodeType nodeClass::TYPE(nodeName, [](const nlohmann::json &j, const CPack &cpack) {                                                    \
     return (std::shared_ptr<NodeBase>) nodeClass::getInstance();                                                                       \
});                                                                                                                                     \
                                                                                                                                        \
NodeType nodeClass::getNodeType() const {                                                                                               \
    return nodeClass::TYPE;                                                                                                             \
}                                                                                                                                       \
                                                                                                                                        \
std::shared_ptr<nodeClass> nodeClass::getInstance() {                                                                                   \
    static std::shared_ptr<nodeClass> INSTANCE;                                                                                         \
    if (INSTANCE == nullptr) {                                                                                                          \
        INSTANCE = std::make_shared<nodeClass>(std::optional(nodeName), std::optional(nodeDescription));                                \
    }                                                                                                                                   \
    return INSTANCE;                                                                                                                    \
}

namespace CHelper {

    class CPack;

    namespace Node {

        class NodeType;

        class NodeBase : public JsonUtil::ToJson {
        public:
            static NodeType TYPE;
            const std::optional<std::string> id;
            const std::optional<std::string> description;
            //存储下一个节点，需要调用构造函数之后再进行添加
            std::vector<std::shared_ptr<NodeBase>> nextNodes;

        protected:
            NodeBase(const std::optional<std::string> &id,
                     const std::optional<std::string> &description);

            NodeBase(const nlohmann::json &j,
                     const CPack &cpack);

            virtual ~NodeBase() = default;

        public:
            static std::shared_ptr<NodeBase> getNodeFromJson(const nlohmann::json &j,
                                                             const CPack &cpack);

            [[nodiscard]] virtual NodeType getNodeType() const;

            void toJson(nlohmann::json &j) const override;

            [[nodiscard]] virtual ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const = 0;

            [[nodiscard]] ASTNode getASTNodeWithNextNode(TokenReader &tokenReader, const CPack &cpack) const;

        private:
            ASTNode getSimpleASTNode(TokenReader &tokenReader,
                                     TokenType::TokenType type,
                                     const std::string &requireType,
                                     const std::string &astNodeId,
                                     const std::function<std::shared_ptr<ErrorReason>(const std::string &str,
                                                                                const VectorView<Token> &tokens)>& check) const;

        protected:
            ASTNode getStringASTNode(TokenReader &tokenReader,
                                     const std::string &astNodeId = "unknown") const;

            ASTNode getIntegerASTNode(TokenReader &tokenReader,
                                      const std::string &astNodeId = "unknown") const;

            ASTNode getFloatASTNode(TokenReader &tokenReader,
                                    const std::string &astNodeId = "unknown") const;

            ASTNode getSymbolASTNode(TokenReader &tokenReader,
                                     char ch,
                                     const std::string &astNodeId = "unknown") const;

            ASTNode getByChildNode(TokenReader &tokenReader,
                                   const CPack &cpack,
                                   const std::shared_ptr<NodeBase> &childNode,
                                   const std::string &astNodeId = "unknown") const;

            //当childNodes只需要有第一个node并且其他node不一定需要的时侯使用
            ASTNode getOptionalASTNode(TokenReader &tokenReader,
                                       const CPack &cpack,
                                       bool isIgnoreChildNodesError,
                                       const std::vector<std::shared_ptr<NodeBase>> &childNodes,
                                       const std::string &astNodeId = "unknown") const;
        };

    } // Node

} // CHelper

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<std::shared_ptr<CHelper::Node::NodeBase>> {

    static void to_json(nlohmann::json &j, const std::shared_ptr<CHelper::Node::NodeBase> &t) {
        t->toJson(j);
    };
};

#endif //CHELPER_NODEBASE_H
