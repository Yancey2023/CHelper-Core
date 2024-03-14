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
            ASTNode astNode = Parser::parse("", cPack.get());
            return std::make_shared<Core>(std::move(cPack), std::move(astNode));
        } catch (const std::exception &e) {
            CHELPER_ERROR("CPack load failed");
            CHelper::Exception::printStackTrace(e);
            CHelper::Profile::clear();
            return nullptr;
        }
    }

    void Core::onTextChanged(const std::string &content, size_t index0) {
        if (input != content) {
            input = content;
            astNode = Parser::parse(input, cpack.get());
        }
        onSelectionChanged(index0);
    }

    void Core::onSelectionChanged(size_t index0) {
        if (index != index0) {
            index = index0;
            suggestions = nullptr;
        }
    }

    std::vector<Suggestion> *Core::getSuggestions() {
        if (suggestions == nullptr) {
            suggestions = std::make_shared<std::vector<Suggestion>>(astNode.getSuggestions(index));
        }
        return suggestions.get();
    }

    std::optional<std::string> Core::onSuggestionClick(size_t which) {
        if (suggestions == nullptr || which >= suggestions->size()) {
            return std::nullopt;
        }
        return suggestions->at(which).onClick(this, TokenUtil::toString(astNode.tokens));
    }

} // CHelper