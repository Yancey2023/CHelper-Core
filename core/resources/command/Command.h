//
// Created by Yancey666 on 2023/11/10.
//

#ifndef CHELPER_COMMAND_H
#define CHELPER_COMMAND_H

#include "pch.h"
#include "node/NodeBase.h"


namespace CHelper {

    class Command : public JsonUtil::ToJson {
    public:
        std::vector<std::string> name;
        std::optional<std::string> description;
        std::optional<std::vector<std::shared_ptr<Node::NodeBase>>> startNodes;

    public:
        Command(const std::vector<std::string> &name,
                const std::optional<std::string> &description,
                const std::optional<std::vector<std::shared_ptr<Node::NodeBase>>> &startNodes);

        void addToSet(std::unordered_set<std::shared_ptr<Node::NodeBase>> &nodes) const;

        void toJson(nlohmann::json &j) const override;

    }; // Command

} // CHelper

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<CHelper::Command> {

    static void to_json(nlohmann::json &j, const CHelper::Command &t) {
        t.toJson(j);
    };
};

#endif //CHELPER_COMMAND_H
