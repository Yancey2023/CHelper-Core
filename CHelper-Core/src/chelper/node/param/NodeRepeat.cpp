//
// Created by Yancey on 2024/3/1.
//

#include <chelper/node/param/NodeRepeat.h>
#include <chelper/node/util/NodeAnd.h>
#include <chelper/node/util/NodeOr.h>

namespace CHelper::Node {

    void NodeRepeat::init(const CPack &cpack) {
        const auto &it = cpack.repeatNodes.find(key);
        if (HEDLEY_LIKELY(it != cpack.repeatNodes.end())) {
            repeatData = it->second.first;
            nodeElement = it->second.second;
            return;
        }
        Profile::push("link repeat data {} to content", FORMAT_ARG(key));
        Profile::push("fail to find repeat data by id {}", FORMAT_ARG(key));
        throw std::runtime_error("fail to find repeat data");
    }

    NodeTypeId::NodeTypeId NodeRepeat::getNodeType() const {
        return NodeTypeId::REPEAT;
    }

}// namespace CHelper::Node