//
// Created by Yancey on 2025-6-19.
//

#ifndef CHELPER_PARAMETERHINT_H
#define CHELPER_PARAMETERHINT_H

#include "chelper/node/param/NodeRepeat.h"
#include "pch.h"
#include <chelper/node/NodeType.h>
#include <chelper/parser/ASTNode.h>
#include <optional>

namespace CHelper::ParameterHint {

    std::optional<std::u16string> getParameterHint(const ASTNode &astNode, size_t index);

}// namespace CHelper::ParameterHint

#endif//CHELPER_PARAMETERHINT_H
