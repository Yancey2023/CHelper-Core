//
// Created by Yancey on 2024/2/24.
//

#pragma once

#ifndef CHELPER_CORE_H
#define CHELPER_CORE_H

#include "node/NodeType.h"
#include "old2new/Old2New.h"
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

        static Core *create(const std::function<std::unique_ptr<CPack>()> &getCPack);

#if CHelperSupportJson == true
        static Core *createByDirectory(const std::filesystem::path &cpackPath);

        static Core *createByJson(const std::filesystem::path &cpackPath);

        static Core *createByBson(const std::filesystem::path &cpackPath);
#endif

#if CHelperWeb != true
        static Core *createByBinary(const std::filesystem::path &cpackPath);
#endif

        void onTextChanged(const std::string &content, size_t index);

        void onSelectionChanged(size_t index0);

        [[nodiscard]] const CPack *getCPack() const;

        [[nodiscard]] const ASTNode *getAstNode() const;

        [[nodiscard]] std::string getDescription() const;

        [[nodiscard]] std::vector<std::shared_ptr<ErrorReason>> getErrorReasons() const;

        std::vector<Suggestion> *getSuggestions();

        [[nodiscard]] std::string getStructure() const;

        [[nodiscard]] [[maybe_unused]] std::string getColors() const;

        [[nodiscard]] std::optional<std::string> onSuggestionClick(size_t which);

        static std::string old2new(const Old2New::BlockFixData &blockFixData, const std::string &old);

    };

}// namespace CHelper

#endif//CHELPER_CORE_H
