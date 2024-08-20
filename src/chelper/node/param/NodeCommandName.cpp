//
// Created by Yancey on 2023/11/11.
//

#include "NodeCommandName.h"
#include "../../resources/CPack.h"

namespace CHelper::Node {

    void NodeCommandName::init(const CPack &cpack) {
        commands = cpack.commands.get();
    }

    NodeType *NodeCommandName::getNodeType() const {
        return NodeType::COMMAND_NAME.get();
    }

    ASTNode NodeCommandName::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return tokenReader.readStringASTNode(this);
    }

    bool NodeCommandName::collectIdError(const ASTNode *astNode,
                                         std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (HEDLEY_UNLIKELY(astNode->isError())) {
            return true;
        }
        std::u16string_view str = astNode->tokens.toString();
        for (const auto &command: *commands) {
            for (const auto &name: ((NodePerCommand *) command.get())->name) {
                if (HEDLEY_UNLIKELY(str == name)) {
                    return true;
                }
            }
        }
        idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::u16string(u"找不到命令名 -> ").append(str)));
        return true;
    }

    bool NodeCommandName::collectSuggestions(const ASTNode *astNode,
                                             size_t index,
                                             std::vector<Suggestions> &suggestions) const {
        std::u16string_view str = astNode->tokens.toString()
                                       .substr(0, index - astNode->tokens.getStartIndex());
        std::vector<std::shared_ptr<NormalId>> nameStartOf, nameContain, descriptionContain;
        for (const auto &item: *commands) {
            bool flag = false;
            for (const auto &item2: ((NodePerCommand *) item.get())->name) {
                //通过名字进行搜索
                size_t index1 = item2.find(str);
                if (HEDLEY_UNLIKELY(index1 != std::u16string::npos)) {
                    if (HEDLEY_UNLIKELY(index1 == 0)) {
                        nameStartOf.push_back(NormalId::make(item2, item->description));
                    } else {
                        nameContain.push_back(NormalId::make(item2, item->description));
                    }
                    flag = true;
                }
            }
            if (HEDLEY_UNLIKELY(flag)) {
                continue;
            }
            //通过介绍进行搜索
            if (HEDLEY_UNLIKELY(item->description.has_value() &&
                                item->description.value().find(str) != std::u16string::npos)) {
                for (const auto &item2: ((NodePerCommand *) item.get())->name) {
                    descriptionContain.push_back(NormalId::make(item2, item->description));
                }
            }
        }
        auto compare = [](const auto &item1, const auto &item2) {
            return item1->name < item2->name;
        };
        std::sort(nameStartOf.begin(), nameStartOf.end(), compare);
        std::sort(nameContain.begin(), nameContain.end(), compare);
        std::sort(descriptionContain.begin(), descriptionContain.end(), compare);
        std::vector<std::shared_ptr<NormalId>> suggestions1;
        suggestions1.reserve(nameStartOf.size() + nameContain.size() + descriptionContain.size());
        suggestions1.insert(suggestions1.end(), nameStartOf.begin(), nameStartOf.end());
        suggestions1.insert(suggestions1.end(), nameContain.begin(), nameContain.end());
        suggestions1.insert(suggestions1.end(), descriptionContain.begin(), descriptionContain.end());
        Suggestions suggestions2(SuggestionsType::ID);
        size_t start = astNode->tokens.getStartIndex();
        size_t end = astNode->tokens.getEndIndex();
        std::transform(suggestions1.begin(), suggestions1.end(),
                       std::back_inserter(suggestions2.suggestions),
                       [&start, &end](const auto &item) {
                           return Suggestion(start, end, true, item);
                       });
        suggestions2.markFiltered();
        suggestions.push_back(std::move(suggestions2));
        return true;
    }

    void NodeCommandName::collectStructure(const ASTNode *astNode,
                                           StructureBuilder &structure,
                                           bool isMustHave) const {
        structure.append(isMustHave, u"命令名");
    }

    bool NodeCommandName::collectColor(const ASTNode *astNode,
                                       ColoredString &coloredString,
                                       const Theme &theme) const {
        coloredString.setColor(astNode->tokens, theme.colorId);
        return true;
    }

    CODEC_UNIQUE_PTR(NodeCommandName)

}// namespace CHelper::Node