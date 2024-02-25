//
// Created by Yancey on 2024/2/24.
//

#include "Core.h"

#include "parser/Parser.h"

namespace CHelper {

    Core::Core(CPack cpack, ASTNode astNode)
            : cpack(std::move(cpack)),
              astNode(std::move(astNode)) {}

    std::shared_ptr<Core> Core::create(const std::string &cpackPath) {
        Node::NodeType::init();
        try {
            clock_t start, end;
            start = clock();
            CPack cPack = CPack::create(cpackPath);
            end = clock();
            CHELPER_INFO(ColorStringBuilder()
                                 .green("CPack load successfully (")
                                 .purple(std::to_string(end - start) + "ms")
                                 .green(")")
                                 .build());
            return std::make_shared<Core>(cPack, Parser::parse("", cPack));
        } catch (const std::exception &e) {
            CHELPER_ERROR("parse failed");
            CHelper::Exception::printStackTrace(e);
            CHelper::Profile::clear();
            return nullptr;
        }
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
        return astNode.getErrorReasons();
    }

    std::vector<Suggestion> Core::getSuggestions() {
        auto result = astNode.getSuggestions(index);
        suggestions = std::make_shared<std::vector<Suggestion>>(result);
        return result;
    }

    std::string Core::getStructure() const {
        return astNode.getStructure();
    }

    std::string Core::getColors() const {
        return astNode.getColors();
    }

    std::optional<std::string> Core::onSuggestionClick(size_t which) const {
        if(suggestions == nullptr || which >= suggestions->size()){
            return std::nullopt;
        }
        return suggestions->at(which).onClick();
    }

} // CHelper