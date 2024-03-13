//
// Created by Yancey on 2024/2/24.
//

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

        static std::shared_ptr<Core> create(const std::string &cpackPath);

        void onTextChanged(const std::string &content, size_t index);

        void onSelectionChanged(size_t index0);

        [[nodiscard]] const ASTNode &getAstNode() const;

        [[nodiscard]] std::string getDescription() const;

        [[nodiscard]] std::vector<std::shared_ptr<ErrorReason>> getErrorReasons() const;

        std::vector<Suggestion> *getSuggestions();

        [[nodiscard]] std::string getStructure() const;

        [[nodiscard]] std::string getColors() const;

        [[nodiscard]] std::optional<std::string> onSuggestionClick(size_t which) const;
    };

} // CHelper

#endif //CHELPER_CORE_H
