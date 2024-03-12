//
// Created by Yancey on 2023/11/10.
//

#ifndef CHELPER_NODEBASE_H
#define CHELPER_NODEBASE_H

#include "pch.h"
#include "NodeType.h"
#include "../parser/ASTNode.h"
#include "../parser/TokenReader.h"

namespace CHelper {

    class CPack;

    namespace Node {

        class NodeType;

        class NodeBase {
        public:
            const std::optional<std::string> id;
            const std::optional<std::string> description;
            //true-一定要在空格后 false-不一定在空格后
            const bool isMustAfterWhiteSpace;
            //存储下一个节点，需要调用构造函数之后再进行添加
            std::vector<NodeBase *> nextNodes;

        protected:
            NodeBase(const std::optional<std::string> &id,
                     const std::optional<std::string> &description,
                     bool isMustAfterWhiteSpace);

            NodeBase(const nlohmann::json &j,
                     bool isMustAfterWhiteSpace);

        public:
            virtual ~NodeBase() = default;

            static std::unique_ptr<NodeBase> getNodeFromJson(const nlohmann::json &j,
                                                             const CPack &cpack);

            [[nodiscard]] virtual NodeType *getNodeType() const;

            virtual void toJson(nlohmann::json &j) const;

            [[nodiscard]] virtual ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const = 0;

            [[nodiscard]] ASTNode getASTNodeWithNextNode(TokenReader &tokenReader, const CPack *cpack) const;

        protected:
            ASTNode getByChildNode(TokenReader &tokenReader,
                                   const CPack *cpack,
                                   const NodeBase *childNode,
                                   const std::string &astNodeId = "") const;

            //当childNodes只需要有第一个node并且其他node不一定需要的时侯使用
            ASTNode getOptionalASTNode(TokenReader &tokenReader,
                                       const CPack *cpack,
                                       bool isIgnoreChildNodesError,
                                       const std::vector<const NodeBase *> &childNodes,
                                       const std::string &astNodeId = "") const;

        public:
            virtual std::optional<std::string> collectDescription(const ASTNode *node, size_t index) const;

            virtual bool collectIdError(const ASTNode *astNode,
                                        std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const;

            virtual bool collectSuggestions(const ASTNode *astNode,
                                            size_t index,
                                            std::vector<Suggestion> &suggestions) const;

            virtual void collectStructure(const ASTNode *astNode,
                                          StructureBuilder &structure,
                                          bool isMustHave) const;

            void collectStructureNormal(StructureBuilder &structure,
                                        bool isMustHave) const;

        };

    } // Node

} // CHelper

template<>
struct nlohmann::adl_serializer<std::unique_ptr<CHelper::Node::NodeBase>> {
    static void to_json(nlohmann::json &j, const std::unique_ptr<CHelper::Node::NodeBase> &t) {
        t->toJson(j);
    };
};

#endif //CHELPER_NODEBASE_H
