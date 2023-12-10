//
// Created by Yancey666 on 2023/11/10.
//

#ifndef CHELPER_NODEBASE_H
#define CHELPER_NODEBASE_H

#include <pch.h>
#include "../type/NodeType.h"

#define NODE_TYPE(nodeName, nodeClass)                                                           \
NodeType nodeClass::TYPE(#nodeName, [](const nlohmann::json &j, const CPack &cpack) { \
return (std::shared_ptr<NodeBase>) std::make_shared<nodeClass>(j, cpack);                        \
});                                                                                              \
                                                                                                 \
NodeType nodeClass::getNodeType() const {                                                        \
    return nodeClass::TYPE;                                                                      \
}

#define NODE_TYPE_H                    \
static NodeType TYPE;                  \
                                       \
NodeType getNodeType() const override;

namespace CHelper {

    class CPack;

    namespace Node {

        class NodeType;

        class NodeBase : public JsonUtil::ToJson {
        public:
            static NodeType TYPE;
            const std::optional<std::string> id;
            const std::optional<std::string> description;
            std::unordered_set<std::shared_ptr<NodeBase>> nextNodes;

        protected:
            NodeBase(const std::optional<std::string> &id,
                     const std::optional<std::string> &description);

            NodeBase(const nlohmann::json &j,
                     const CPack &cpack);

        public:
            virtual NodeType getNodeType() const;

            void addToSet(std::unordered_set<std::shared_ptr<NodeBase>> &nodes);

            void toJson(nlohmann::json &j) const override;
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
