//
// Created by Yancey on 2023/11/11.
//

#include "NodeCommandName.h"
#include "../../resources/CPack.h"
#include "../../util/TokenUtil.h"

namespace CHelper::Node {

    NodeCommandName::NodeCommandName(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description,
                                     const std::vector<std::unique_ptr<Node::NodeBase>> *commands)
            : NodeBase(id, description, false),
              commands(commands) {}

    NodeCommandName::NodeCommandName(const nlohmann::json &j,
                                     const CPack &cpack)
            : NodeBase(j, true),
              commands(cpack.commands.get()) {}

    NodeType *NodeCommandName::getNodeType() const {
        return NodeType::COMMAND_NAME.get();
    }

    ASTNode NodeCommandName::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return tokenReader.readStringASTNode(this);
    }

    bool NodeCommandName::collectIdError(const ASTNode *astNode,
                                         std::vector<std::shared_ptr<ErrorReason>> &idErrorReasons) const {
        if (astNode->isError()) {
            return true;
        }
        std::string str = TokenUtil::toString(astNode->tokens);
        for (const auto &command: *commands) {
            for (const auto &name: ((NodePerCommand *) command.get())->name) {
                if (str == name) {
                    return true;
                }
            }
        }
        idErrorReasons.push_back(ErrorReason::idError(astNode->tokens, std::string("找不到命令名 -> ").append(str)));
        return true;
    }

    bool NodeCommandName::collectSuggestions(const ASTNode *astNode,
                                             size_t index,
                                             std::vector<Suggestions> &suggestions) const {
        std::string str = TokenUtil::toString(astNode->tokens)
                .substr(0, index - TokenUtil::getStartIndex(astNode->tokens));

        std::vector<std::shared_ptr<NormalId>> nameStartOf, nameContain, descriptionContain;
        for (const auto &item: *commands) {
            bool flag = false;
            for (const auto &item2: ((NodePerCommand *) item.get())->name) {
                //通过名字进行搜索
                size_t index1 = item2.find(str);
                if (index1 != std::string::npos) {
                    if (index1 == 0) {
                        nameStartOf.push_back(std::make_shared<NormalId>(item2, item->description));
                    } else {
                        nameContain.push_back(std::make_shared<NormalId>(item2, item->description));
                    }
                }
                flag = true;
            }
            if (flag) {
                continue;
            }
            //通过介绍进行搜索
            if (item->description.has_value()) {
                size_t index2 = item->description.value().find(str);
                if (index2 != std::string::npos) {
                    for (const auto &item2: ((NodePerCommand *) item.get())->name) {
                        descriptionContain.push_back(std::make_shared<NormalId>(item2, item->description));
                    }
                }
            }
        }
        std::vector<std::shared_ptr<NormalId>> suggestions1;
        suggestions1.reserve(nameStartOf.size() + nameContain.size() + descriptionContain.size());
        suggestions1.insert(suggestions1.end(), nameStartOf.begin(), nameStartOf.end());
        suggestions1.insert(suggestions1.end(), nameContain.begin(), nameContain.end());
        suggestions1.insert(suggestions1.end(), descriptionContain.begin(), descriptionContain.end());
        std::sort(suggestions1.begin(), suggestions1.end(),
                  [](const auto &item1, const auto &item2) {
                      return item1->name < item2->name;
                  });
        Suggestions suggestions2;
        size_t start = TokenUtil::getStartIndex(astNode->tokens);
        size_t end = TokenUtil::getEndIndex(astNode->tokens);
        std::transform(suggestions1.begin(), suggestions1.end(),
                       std::back_inserter(suggestions2.suggestions),
                       [&start, &end](const auto &item) {
                           return Suggestion(start, end, item);
                       });
        suggestions2.markFiltered();
        suggestions.push_back(std::move(suggestions2));
        return true;
    }

    void NodeCommandName::collectStructure(const ASTNode *astNode,
                                           StructureBuilder &structure,
                                           bool isMustHave) const {
        structure.append(isMustHave, "命令名");
    }

} // CHelper::Node