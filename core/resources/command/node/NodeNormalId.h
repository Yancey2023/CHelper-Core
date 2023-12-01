//
// Created by Yancey666 on 2023/11/11.
//

#ifndef CHELPER_NODENORMALID_H
#define CHELPER_NODENORMALID_H

#include "NodeBase.h"
#include "../../id/NormalId.h"
#include "../../CPack.h"
#include <vector>

namespace CHelper::Node {

    class NodeNormalId : public NodeBase {
    public:
        const std::optional<std::string> key;
        const std::vector<NormalId> *contents;

        NodeNormalId(const std::optional<std::string> &id,
                     const std::optional<std::string> &description,
                     const std::optional<std::string> &key,
                     const std::vector<NormalId> *contents);

        NodeNormalId(const nlohmann::json &j,
                     const CPack &cpack);

        ~NodeNormalId();

        void toJson(nlohmann::json &j) const override;

    };

}

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<CHelper::Node::NodeNormalId> {

    static void to_json(nlohmann::json &j, const CHelper::Node::NodeNormalId &t) {
        t.toJson(j);
    };

};

#endif //CHELPER_NODENORMALID_H
