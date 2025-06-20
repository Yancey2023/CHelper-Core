//
// Created by Yancey on 2025-06-20.
//

#pragma once

#ifndef AUTOSUGGESTION_H
#define AUTOSUGGESTION_H

#include <chelper/auto_suggestion/Suggestions.h>
#include <chelper/parser/ASTNode.h>

namespace CHelper::AutoSuggestion {

    Suggestions getSuggestions(const ASTNode &astNode, size_t index);

}// namespace CHelper::AutoSuggestion

#endif//AUTOSUGGESTION_H
