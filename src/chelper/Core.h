//
// Created by Yancey on 2024/2/24.
//

#pragma once

#ifndef CHELPER_CORE_H
#define CHELPER_CORE_H

#include "node/NodeType.h"
#include "parser/ASTNode.h"
#include "resources/CPack.h"

namespace CHelper {

    class Core {
    private:
        std::string input;
        size_t index = 0;
        std::unique_ptr<CPack> cpack;
        ASTNode astNode;
        std::shared_ptr<std::vector<Suggestion>> suggestions;

    public:
        Core(std::unique_ptr<CPack> cpack, ASTNode astNode);

        static std::shared_ptr<Core> create(const std::function<std::unique_ptr<CPack>()> &getCPack);

        static std::shared_ptr<Core> createByDirectory(const std::string &cpackPath);

        static std::shared_ptr<Core> createByJson(const std::string &cpackPath);

        static std::shared_ptr<Core> createByBson(const std::string &cpackPath);

        void onTextChanged(const std::string &content, size_t index);

        void onSelectionChanged(size_t index0);

        [[nodiscard]] const CPack *getCPack() const {
            return cpack.get();
        }

        [[nodiscard]] const ASTNode *getAstNode() const {
            return &astNode;
        }

        [[nodiscard]] std::string getDescription() const {
            return astNode.getDescription(index);
        }

        [[nodiscard]] std::vector<std::shared_ptr<ErrorReason>> getErrorReasons() const {
            return astNode.getErrorReasons();
        }

        std::vector<Suggestion> *getSuggestions();

        [[nodiscard]] std::string getStructure() const {
            return astNode.getStructure();
        }

        [[nodiscard]] [[maybe_unused]] std::string getColors() const {
            return astNode.getColors();
        }

        [[nodiscard]] std::optional<std::string> onSuggestionClick(size_t which);
    };

} // CHelper

#endif //CHELPER_CORE_H
