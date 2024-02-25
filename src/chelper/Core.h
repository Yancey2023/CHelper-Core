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
        CPack cpack;
        ASTNode astNode;

        Core(CPack cpack, ASTNode astNode);

    public:
        static Core create(const std::string &cpackPath);

        void onTextChanged(const std::string &content, size_t index);

        void onSelectionChanged(size_t index0);

        std::string getDescription() const;

        std::vector<std::shared_ptr<ErrorReason>> getErrorReasons() const;

        std::vector<Suggestion> getSuggestions() const;

        std::string getStructure() const;

        std::string getColors() const;

    };

} // CHelper

#endif //CHELPER_CORE_H
