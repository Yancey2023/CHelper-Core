//
// Created by Yancey666 on 2023/11/10.
//

#ifndef CHELPER_NODEBASE_H
#define CHELPER_NODEBASE_H

#include <pch.h>

namespace CHelper::Node {

    namespace NodeType {

        enum NodeType {
//            UNKNOWN,
            EOL,
            BLOCK,
            BOOLEAN,
            COMMAND,
            COMMAND_NAME,
            FLOAT,
            INTEGER,
            NAMESPACE_ID,
            NORMAL_ID,
            TARGET_SELECTOR,
            TEXT,
            ITEM,
            POSITION
        };

//        const std::string STR_UNKNOWN = "UNKNOWN";
//        const std::string STR_EOL = "EOL";
        const std::string STR_BLOCK = "BLOCK";
        const std::string STR_BOOLEAN = "BOOLEAN";
        const std::string STR_COMMAND = "COMMAND";
        const std::string STR_COMMAND_NAME = "COMMAND_NAME";
        const std::string STR_FLOAT = "FLOAT";
        const std::string STR_INTEGER = "INTEGER";
        const std::string STR_NAMESPACE_ID = "NAMESPACE_ID";
        const std::string STR_NORMAL_ID = "NORMAL_ID";
        const std::string STR_TARGET_SELECTOR = "TARGET_SELECTOR";
        const std::string STR_TEXT = "TEXT";
        const std::string STR_ITEM = "ITEM";
        const std::string STR_POSITION = "POSITION";

    } // NodeType

    class NodeBase : public JsonUtil::ToJson {
    public:
        const NodeType::NodeType type;
        const std::optional<std::string> id;
        const std::optional<std::string> description;
        std::unordered_set<std::shared_ptr<NodeBase>> nextNodes;

    protected:
        NodeBase(NodeType::NodeType type,
                 const std::optional<std::string> &id,
                 const std::optional<std::string> &description);

        NodeBase(NodeType::NodeType type,
                 const nlohmann::json &j);

    public:

        void addToSet(std::unordered_set<std::shared_ptr<NodeBase>> &nodes);

        void toJson(nlohmann::json &j) const override;
    };

} // CHelper::Node

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<std::shared_ptr<CHelper::Node::NodeBase>> {

    static void to_json(nlohmann::json &j, const std::shared_ptr<CHelper::Node::NodeBase> &t) {
        t->toJson(j);
    };
};

#endif //CHELPER_NODEBASE_H
