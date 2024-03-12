//
// Created by Yancey on 2024/2/24.
//

#include "Core.h"

#include "parser/Parser.h"
#include "util/TokenUtil.h"

namespace CHelper {

    Core::Core(std::unique_ptr<CPack> cpack, ASTNode astNode)
            : cpack(std::move(cpack)),
              astNode(std::move(astNode)) {}

    std::shared_ptr<Core> Core::create(const std::string &cpackPath) {
        Node::NodeType::init();
        try {
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            std::unique_ptr<CPack> cPack = CPack::create(cpackPath);
            end = std::chrono::high_resolution_clock::now();
            CHELPER_INFO(ColorStringBuilder()
                                 .green("CPack load successfully (")
                                 .purple(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                         end - start).count()) + "ms")
                                 .green(")")
                                 .build());
            return std::make_shared<Core>(std::move(cPack), Parser::parse("", cPack.get()));
        } catch (const std::exception &e) {
            CHELPER_ERROR("parse failed");
            CHelper::Exception::printStackTrace(e);
            CHelper::Profile::clear();
            return nullptr;
        }
    }

    void Core::onTextChanged(const std::string &content, size_t index0) {
        astNode = Parser::parse(content, cpack.get());
        index = index0;
    }

    void Core::onSelectionChanged(size_t index0) {
        index = index0;
    }

    const ASTNode &Core::getAstNode() const {
        return astNode;
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
        if (suggestions == nullptr || which >= suggestions->size()) {
            return std::nullopt;
        }
        return suggestions->at(which).onClick(TokenUtil::toString(astNode.tokens));
    }

} // CHelper