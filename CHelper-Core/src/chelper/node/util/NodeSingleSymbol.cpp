//
// Created by Yancey on 2023/12/19.
//

#include <chelper/node/util/NodeSingleSymbol.h>

namespace CHelper::Node {

    static std::shared_ptr<NormalId> getNormalId(char16_t symbol, const std::optional<std::u16string> &description) {
        std::u16string name;
        name.push_back(symbol);
        return NormalId::make(name, description);
    }

    NodeSingleSymbol::NodeSingleSymbol(char16_t symbol,
                                       const std::optional<std::u16string> &description,
                                       bool isAddSpace)
        : symbol(symbol),
          normalId(getNormalId(symbol, description)),
          isAddSpace(isAddSpace) {}

    NodeTypeId::NodeTypeId NodeSingleSymbol::getNodeType() const {
        return NodeTypeId::SINGLE_SYMBOL;
    }

}// namespace CHelper::Node