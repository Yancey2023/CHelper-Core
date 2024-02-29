//
// Created by Yancey on 2024/2/28.
//

#ifndef CHELPER_NODEJSONENTRY_H
#define CHELPER_NODEJSONENTRY_H

#include "../NodeBase.h"

namespace CHelper::Node {

    //在json数据文件并没有这个类型，添加这个节点类型只是为了更好实现json数据类型中的object类型
    class NodeJsonEntry : public NodeBase {
    public:
        const std::string key;
        const std::string value;
        std::shared_ptr<NodeBase> nodeEntry;

        NodeJsonEntry(const std::optional<std::string> &id,
                      const std::optional<std::string> &description,
                      std::string key,
                      std::string value);

        explicit NodeJsonEntry(const nlohmann::json &j);

        void init(const std::vector<std::shared_ptr<NodeBase>> &dataList);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack &cpack) const override;

    };

} // CHelper::Node


#endif //CHELPER_NODEJSONENTRY_H
