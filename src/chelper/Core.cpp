//
// Created by Yancey on 2024/2/24.
//

#include "Core.h"

#include "parser/Parser.h"

namespace CHelper {

    Core::Core(CPack cpack, ASTNode astNode)
            : cpack(std::move(cpack)),
              astNode(std::move(astNode)) {}

    Core Core::create(const std::string &cpackPath) {
        Node::NodeType::init();
        CPack cPack = CPack::create(cpackPath);
        return {cPack, Parser::parse("", cPack)};
    }

    void Core::onTextChanged(const std::string &content, size_t index0) {
        astNode = Parser::parse(content, cpack);
        index = index0;
    }

    void Core::onSelectionChanged(size_t index0) {
        index = index0;
    }

    std::string Core::getDescription() const {
        return astNode.getDescription(index);
    }

    std::vector<std::shared_ptr<ErrorReason>> Core::getErrorReasons() const {
        return astNode.getErrorReasons(cpack);
    }

    std::vector<Suggestion> Core::getSuggestions() const {
        return astNode.getSuggestions(cpack, index);
    }

    std::string Core::getStructure() const {
        return astNode.getStructure();
    }

    std::string Core::getColors() const {
        return astNode.getColors();
    }

} // CHelper