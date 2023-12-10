//
// Created by Yancey666 on 2023/11/13.
//

#ifndef CHELPER_NODELF_H
#define CHELPER_NODELF_H

#include "NodeBase.h"

namespace CHelper::Node {

    class NodeLF : public NodeBase {
    public:
        NodeLF(const std::optional<std::string> &id,
               const std::optional<std::string> &description);

        static std::shared_ptr<NodeLF> getInstance();

        NODE_TYPE_H;

    };

} // CHelper::Node

#endif //CHELPER_NODELF_H
