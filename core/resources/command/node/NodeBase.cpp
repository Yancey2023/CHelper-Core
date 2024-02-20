//
// Created by Yancey666 on 2023/11/10.
//

#include "NodeBase.h"

namespace CHelper::Node {

    NodeType NodeBase::TYPE("UNKNOWN", [](const nlohmann::json &j, const CPack &cpack) {
        return std::shared_ptr<NodeBase>();
    });

    NodeBase::NodeBase(const std::optional<std::string> &id,
                       const std::optional<std::string> &description)
            : id(id),
              description(description) {}

    NodeBase::NodeBase(const nlohmann::json &j,
                       const CPack &cpack)
            : id(FROM_JSON_OPTIONAL(j, id, std::string)),
              description(FROM_JSON_OPTIONAL(j, description, std::string)) {}

    std::shared_ptr<NodeBase> NodeBase::getNodeFromJson(const nlohmann::json &j,
                                                        const CPack &cpack) {
        std::string state = Color::RED + "unknown state" +
                            Color::NORMAL;
        try {
            state = Color::RED + "loading type" +
                    Color::NORMAL;
            std::string type = FROM_JSON(j, type, std::string);
            state = Color::RED + "loading node " +
                    Color::PURPLE + type +
                    Color::NORMAL;
            for (const auto &item: NodeType::NODE_TYPES) {
                if (item->nodeName == type) {
                    return item->createNodeByJson(j, cpack);
                }
            }
            throw Exception::UnknownNodeType(type);
        } catch (Exception::CHelperException &e) {
            throw Exception::NodeLoadFailed(state, Color::RED + e.reason());
        } catch (std::exception &e) {
            throw Exception::NodeLoadFailed(state, Color::BLUE + "[CHelper] " +
                                                   Color::RED + "Exception (" +
                                                   Color::PURPLE + "std::exception" +
                                                   Color::RED + ")" +
                                                   Color::NORMAL + " -> " +
                                                   Color::RED + e.what());
        } catch (std::string &s) {
            throw Exception::NodeLoadFailed(state, Color::BLUE + "[CHelper] " +
                                                   Color::RED + "Exception (" +
                                                   Color::PURPLE + "std::string" +
                                                   Color::RED + ")" +
                                                   Color::NORMAL + " -> " +
                                                   Color::RED + s);
        }
    }

    NodeType NodeBase::getNodeType() const {
        return NodeBase::TYPE;
    }

    void NodeBase::toJson(nlohmann::json &j) const {
        TO_JSON_OPTIONAL(j, id)
        TO_JSON_OPTIONAL(j, description)
        j.push_back({"type", getNodeType().nodeName});
    }

    ASTNode NodeBase::getASTNodeWithNextNode(TokenReader &tokenReader, const CPack &cpack) const {
        tokenReader.push();
        //当前节点
        ASTNode currentASTNode = getASTNode(tokenReader, cpack);
        if (currentASTNode.isError() || nextNodes.empty()) {
            tokenReader.pop();
            return currentASTNode;
        }
        //子节点
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(nextNodes.size());
        for (const auto &item: nextNodes) {
            tokenReader.push();
            childASTNodes.push_back(item->getASTNode(tokenReader, cpack));
            tokenReader.restore();
        }
        tokenReader.push();
        while (tokenReader.ready()) {
            if (tokenReader.read()->type == TokenType::LF) {
                break;
            }
        }
        ASTNode nextASTNode = ASTNode::orNode(this, childASTNodes, tokenReader.collect());
        return ASTNode::andNode(this, {currentASTNode, nextASTNode}, tokenReader.collect());
    }

    ASTNode NodeBase::getSimpleASTNode(TokenReader &tokenReader,
                                       const TokenType::TokenType type,
                                       const std::string &requireType,
                                       const std::string &astNodeId,
                                       const std::function<std::shared_ptr<ErrorReason>(const std::string &str,
                                                                                        const VectorView<Token> &tokens)> &check) const {
        tokenReader.push();
        const Token *token = tokenReader.read();
        VectorView<Token> tokens = tokenReader.collect();
        std::shared_ptr<ErrorReason> errorReason;
        if (token == nullptr) {
            errorReason = ErrorReason::incomplete(tokens, util::Format("指令不完整，需要的参数类型为{0}", requireType));
        } else if (token->type != type) {
            errorReason = ErrorReason::errorType(
                    tokens, util::Format("类型不匹配，正确的参数类型为{0}，但当前参数类型为{1}",
                                         requireType, TokenType::getName(token->type)));
        } else {
            errorReason = check(token->content, tokens);
        }
        return ASTNode::simpleNode(this, tokens, errorReason, astNodeId);
    }

    ASTNode NodeBase::getStringASTNode(TokenReader &tokenReader, const std::string &astNodeId) const {
        return getSimpleASTNode(tokenReader, TokenType::STRING, "字符串类型", astNodeId,
                                [](const std::string &str,
                                   const VectorView<Token> &tokens) -> std::shared_ptr<ErrorReason> {
                                    return nullptr;
                                });
    }

    ASTNode NodeBase::getIntegerASTNode(TokenReader &tokenReader, const std::string &astNodeId) const {
        return getSimpleASTNode(tokenReader, TokenType::NUMBER, "整数类型", astNodeId,
                                [](const std::string &str,
                                   const VectorView<Token> &tokens) -> std::shared_ptr<ErrorReason> {
                                    for (const auto &ch: str) {
                                        if (ch == '.') {
                                            return ErrorReason::errorContent(
                                                    tokens, "类型不匹配，正确的参数类型为整数，但当前参数类型为小数");
                                        }
                                    }
                                    return nullptr;
                                });
    }

    ASTNode NodeBase::getFloatASTNode(TokenReader &tokenReader, const std::string &astNodeId) const {
        return getSimpleASTNode(tokenReader, TokenType::NUMBER, "数字类型", astNodeId,
                                [](const std::string &str,
                                   const VectorView<Token> &tokens) -> std::shared_ptr<ErrorReason> {
                                    bool isHavePoint = false;
                                    for (const auto &ch: str) {
                                        if (ch != '.') {
                                            continue;
                                        }
                                        if (isHavePoint) {
                                            return ErrorReason::errorContent(tokens, "数字格式错误");
                                        }
                                        isHavePoint = true;
                                    }
                                    return nullptr;
                                });
    }

    ASTNode NodeBase::getSymbolASTNode(TokenReader &tokenReader, char ch, const std::string &astNodeId) const {
        return getSimpleASTNode(tokenReader, TokenType::SYMBOL, "符号类型", astNodeId,
                                [&ch](const std::string &str,
                                      const VectorView<Token> &tokens) -> std::shared_ptr<ErrorReason> {
                                    if (str.length() != 1 || str[0] != ch) {
                                        return ErrorReason::errorContent(tokens, util::Format(
                                                "内容不匹配，正确的符号为{0}，但当前内容为{1}",
                                                ch, str));
                                    }
                                    return nullptr;
                                });
    }

    ASTNode NodeBase::getByChildNode(TokenReader &tokenReader,
                                     const CPack &cpack,
                                     const std::shared_ptr<NodeBase> &childNode,
                                     const std::string &astNodeId) const {
        ASTNode node = childNode->getASTNode(tokenReader, cpack);
        return ASTNode::andNode(this, {node}, node.tokens, nullptr, astNodeId);
    }

    //isIgnoreChildNodesError
    //true - 第一个错误节点到后面都不算做子节点
    //false - 第一个内容为空的错误节点到后面都不算做子节点
    ASTNode NodeBase::getOptionalASTNode(TokenReader &tokenReader,
                                         const CPack &cpack,
                                         bool isIgnoreChildNodesError,
                                         const std::vector<std::shared_ptr<NodeBase>> &childNodes,
                                         const std::string &astNodeId) const {
#if CHelperDebug == true
        if (childNodes.size() <= 1) {
            Logger::warn("NodeBase", "getOptionalASTNode()传入的子节点数量应该大于1");
        }
#endif
        tokenReader.push();
        std::vector<ASTNode> childASTNodes;
        for (const auto &item: childNodes) {
            tokenReader.push();
            ASTNode astNode = item->getASTNode(tokenReader, cpack);
            if (astNode.isError() && (!isIgnoreChildNodesError || astNode.tokens.start != astNode.tokens.end)) {
                if (childASTNodes.empty()) {
                    tokenReader.pop();
                    return ASTNode::andNode(this, {astNode}, tokenReader.collect(), nullptr, astNodeId);
                }
                tokenReader.restore();
                break;
            }
            childASTNodes.push_back(astNode);
            tokenReader.pop();
        }
        return ASTNode::andNode(this, childASTNodes, tokenReader.collect(), nullptr, astNodeId);
    }

} // CHelper::Node
