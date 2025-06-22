//
// Created by Yancey on 2023/11/6.
//

#include <chelper/lexer/Lexer.h>
#include <chelper/node/NodeInitialization.h>
#include <chelper/node/NodeType.h>
#include <chelper/parser/Parser.h>
#include <chelper/resources/CPack.h>
#include <chelper/serialization/Serialization.h>

#ifdef CHelperDebug
#define DEBUG_GET_NODE_BEGIN(node, index) size_t index = tokenReader.indexStack.size()
#else
#define DEBUG_GET_NODE_BEGIN(node, index) ;
#endif

#ifdef CHelperDebug
#define DEBUG_GET_NODE_END(node, index)                                                                         \
    do {                                                                                                        \
        if (HEDLEY_UNLIKELY((index) != tokenReader.indexStack.size())) {                                        \
            Profile::push("TokenReaderIndexError: {}", FORMAT_ARG(NodeTypeHelper::getName((node).nodeTypeId))); \
            throw std::runtime_error("TokenReaderIndexError");                                                  \
        }                                                                                                       \
    } while (0)
#else
#define DEBUG_GET_NODE_END(node, index) ;
#endif

#define CHELPER_GET_AST_NODE(v1) \
    case Node::NodeTypeId::v1:   \
        return Parser<typename Node::NodeTypeDetail<Node::NodeTypeId::v1>::Type>::getASTNode(*reinterpret_cast<const typename Node::NodeTypeDetail<Node::NodeTypeId::v1>::Type *>(node.data), tokenReader, cpack);

namespace CHelper::Parser {

    ASTNode parse(const Node::NodeWithType &node, TokenReader &tokenReader, const CPack *cpack);

    ASTNode parseByChildNode(const Node::NodeWithType &node,
                             TokenReader &tokenReader,
                             const CPack *cpack,
                             const Node::NodeWithType &childNode,
                             const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE) {
        ASTNode childAstNode = parse(childNode, tokenReader, cpack);
        TokensView tokens = childAstNode.tokens;
        return ASTNode::andNode(node, {std::move(childAstNode)}, tokens, nullptr, astNodeId);
    }

    template<class NodeType>
    struct Parser {
    };

    template<>
    struct Parser<Node::NodeWrapped> {
        static ASTNode getASTNodeWithIsMustAfterSpace(const Node::NodeWrapped &node, TokenReader &tokenReader, const CPack *cpack, bool isMustAfterSpace) {
            //空格检测
            bool isMustAfterSpace0 = reinterpret_cast<const Node::NodeSerializable *>(node.innerNode.data)->getIsMustAfterSpace();
            if (node.innerNode.nodeTypeId != Node::NodeTypeId::REPEAT) {
                tokenReader.push();
                if (HEDLEY_UNLIKELY((isMustAfterSpace0 || isMustAfterSpace) && node.innerNode.nodeTypeId != Node::NodeTypeId::LF && tokenReader.skipSpace() == 0)) {
                    TokensView tokens = tokenReader.collect();
                    return ASTNode::simpleNode(node, tokens, ErrorReason::requireSpace(tokens));
                }
                tokenReader.pop();
            }
            //当前节点
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(node.innerNode, index);
            ASTNode currentASTNode = parse(node.innerNode, tokenReader, cpack);
            DEBUG_GET_NODE_END(node.innerNode, index);
            if (HEDLEY_UNLIKELY(currentASTNode.isError() || node.nextNodes.empty())) {
                return ASTNode::andNode(node, {std::move(currentASTNode)}, tokenReader.collect());
            }
            //子节点
            std::vector<ASTNode> childASTNodes;
            childASTNodes.reserve(node.nextNodes.size());
            for (const auto &item: node.nextNodes) {
                tokenReader.push();
                childASTNodes.push_back(getASTNodeWithIsMustAfterSpace(*item, tokenReader, cpack, isMustAfterSpace0));
                tokenReader.restore();
            }
            tokenReader.push();
            tokenReader.skipToLF();
            ASTNode nextASTNode = ASTNode::orNode(node, std::move(childASTNodes), tokenReader.collect());
            return ASTNode::andNode(node, {std::move(currentASTNode), std::move(nextASTNode)}, tokenReader.collect());
        }

        static ASTNode getASTNode(const Node::NodeWrapped &node, TokenReader &tokenReader, const CPack *cpack) {
            return getASTNodeWithIsMustAfterSpace(node, tokenReader, cpack, false);
        }
    };

    template<>
    struct Parser<Node::NodeJsonElement> {
        static ASTNode getASTNode(const Node::NodeJsonElement &node, TokenReader &tokenReader, const CPack *cpack) {
            return parseByChildNode(node, tokenReader, cpack, node.start);
        }
    };

    template<>
    struct Parser<Node::NodeJsonEntry> {
        static ASTNode getASTNode(const Node::NodeJsonEntry &node, TokenReader &tokenReader, const CPack *cpack) {
            return parseByChildNode(node, tokenReader, cpack, HEDLEY_LIKELY(node.nodeEntry.has_value()) ? node.nodeEntry.value() : Node::NodeJsonEntry::nodeAllEntry);
        }
    };

    template<>
    struct Parser<Node::NodeJsonList> {
        static ASTNode getASTNode(const Node::NodeJsonList &node, TokenReader &tokenReader, const CPack *cpack) {
            if (HEDLEY_UNLIKELY(!node.nodeList.has_value())) {
                return parseByChildNode(node, tokenReader, cpack, CHelper::Node::NodeJsonList::nodeAllList, ASTNodeId::NODE_JSON_ALL_LIST);
            }
            tokenReader.push();
            ASTNode result1 = parse(node.nodeList.value(), tokenReader, cpack);
            if (HEDLEY_LIKELY(!result1.isError())) {
                return ASTNode::andNode(node, {std::move(result1)}, tokenReader.collect());
            }
            size_t index1 = tokenReader.index;
            tokenReader.restore();
            tokenReader.push();
            ASTNode result2 = parseByChildNode(node, tokenReader, cpack, CHelper::Node::NodeJsonList::nodeAllList, ASTNodeId::NODE_JSON_ALL_LIST);
            size_t index2 = tokenReader.index;
            tokenReader.restore();
            tokenReader.push();
            tokenReader.index = result1.isError() ? index2 : index1;
            return ASTNode::orNode(node, {std::move(result1), std::move(result2)}, tokenReader.collect());
        }
    };

    template<>
    struct Parser<Node::NodeJsonNull> {
        static ASTNode getASTNode(const Node::NodeJsonNull &node, TokenReader &tokenReader, const CPack *cpack) {
            tokenReader.push();
            auto result = tokenReader.readStringASTNode(node);
            tokenReader.pop();
            std::u16string_view str = result.tokens.toString();
            if (HEDLEY_LIKELY(str.empty())) {
                TokensView tokens = result.tokens;
                return ASTNode::andNode(node, {std::move(result)}, tokens, ErrorReason::contentError(tokens, u"null参数为空"));
            } else if (HEDLEY_LIKELY(str != u"null")) {
                TokensView tokens = result.tokens;
                return ASTNode::andNode(node, {std::move(result)}, tokens, ErrorReason::contentError(tokens, fmt::format(u"内容不是null -> {}", str)));
            }
            return result;
        }
    };

    template<>
    struct Parser<Node::NodeJsonObject> {
        static ASTNode getASTNode(const Node::NodeJsonObject &node, TokenReader &tokenReader, const CPack *cpack) {
            return parseByChildNode(node, tokenReader, cpack, node.nodeList);
        }
    };

    std::pair<ASTNode, JsonUtil::ConvertResult>
    getInnerASTNode(const Node::NodeWithType &node,
                    const TokensView &tokens,
                    const std::u16string &content,
                    const CPack *cpack,
                    const Node::NodeWithType &mainNode) {
        auto convertResult = JsonUtil::jsonString2String(content);
        if (HEDLEY_UNLIKELY(convertResult.errorReason != nullptr)) {
            convertResult.errorReason->start--;
            convertResult.errorReason->end--;
            return {ASTNode::simpleNode(node, tokens, convertResult.errorReason), std::move(convertResult)};
        }
        auto tokenReader = TokenReader(std::make_shared<LexerResult>(Lexer::lex(convertResult.result)));
#ifdef CHelperTest
        Profile::push("start parsing: {}", FORMAT_ARG(utf8::utf16to8(content)));
#endif
        DEBUG_GET_NODE_BEGIN(mainNode, index);
        ASTNode result = parse(convertResult.result, cpack, mainNode);
        DEBUG_GET_NODE_END(mainNode, index);
#ifdef CHelperTest
        Profile::pop();
#endif
        return {result, convertResult};
    }

    template<>
    struct Parser<Node::NodeJsonString> {
        static ASTNode getASTNode(const Node::NodeJsonString &node, TokenReader &tokenReader, const CPack *cpack) {
            tokenReader.push();
            ASTNode result = tokenReader.readStringASTNode(node);
            tokenReader.pop();
            TokensView tokens = result.tokens;
            std::u16string_view str = tokens.toString();
            if (HEDLEY_UNLIKELY(str.empty())) {
                return ASTNode::simpleNode(node, tokens, ErrorReason::incomplete(tokens, u"字符串参数内容为空"));
            } else if (HEDLEY_UNLIKELY(str[0] != '"')) {
                return ASTNode::simpleNode(node, tokens, ErrorReason::contentError(tokens, fmt::format(u"字符串参数内容应该在双引号内 -> {}", str)));
            }
            std::shared_ptr<ErrorReason> errorReason;
            if (HEDLEY_LIKELY(str.size() <= 1 || str[str.size() - 1] != '"')) {
                errorReason = ErrorReason::contentError(tokens, fmt::format(u"字符串参数内容应该在双引号内 -> {}", str));
            }
            if (HEDLEY_LIKELY(!node.data.has_value() || node.data->nodes.empty())) {
                return ASTNode::simpleNode(node, tokens, errorReason);
            }
            size_t offset = tokens.getStartIndex() + 1;
            auto innerNode = getInnerASTNode(node, tokens, std::u16string(str), cpack, node.nodeData);
            ASTNode newResult = ASTNode::andNode(node, {std::move(innerNode.first)}, tokens, errorReason, ASTNodeId::NODE_STRING_INNER);
            if (HEDLEY_UNLIKELY(errorReason == nullptr)) {
                for (auto &item: newResult.errorReasons) {
                    item = std::make_shared<ErrorReason>(
                            item->level,
                            innerNode.second.convert(item->start) + offset,
                            innerNode.second.convert(item->end) + offset,
                            item->errorReason);
                }
            }
            return newResult;
        }
    };

    template<>
    struct Parser<Node::NodeBlock> {
        static ASTNode getASTNode(const Node::NodeBlock &node, TokenReader &tokenReader, const CPack *cpack) {
            tokenReader.push();
            ASTNode blockId = parseByChildNode(node, tokenReader, cpack, node.nodeBlockId, ASTNodeId::NODE_BLOCK_BLOCK_ID);
            if (HEDLEY_UNLIKELY(node.nodeBlockType == Node::NodeBlockType::BLOCK || blockId.isError())) {
                tokenReader.pop();
                return blockId;
            }
            tokenReader.push();
            ASTNode blockStateLeftBracket = parse(CHelper::Node::NodeBlock::nodeBlockStateLeftBracket, tokenReader, cpack);
            tokenReader.restore();
            if (HEDLEY_LIKELY(blockStateLeftBracket.isError())) {
                return ASTNode::andNode(node, {blockId}, tokenReader.collect(),
                                        nullptr, ASTNodeId::NODE_BLOCK_BLOCK_AND_BLOCK_STATE);
            }
            std::u16string_view str = blockId.tokens.toString();
            XXH64_hash_t strHash = XXH3_64bits(str.data(), str.size() * sizeof(decltype(str)::value_type));
            std::shared_ptr<NamespaceId> currentBlock = nullptr;
            for (const auto &item: *node.blockIds->blockStateValues) {
                if (HEDLEY_UNLIKELY(item->fastMatch(strHash) || item->getIdWithNamespace()->fastMatch(strHash))) {
                    currentBlock = item;
                    break;
                }
            }
            auto nodeBlockState = currentBlock == nullptr
                                          ? BlockId::getNodeAllBlockState()
                                          : std::static_pointer_cast<BlockId>(currentBlock)->getNode(node.blockIds->blockPropertyDescriptions);
            auto astNodeBlockState = parseByChildNode(node, tokenReader, cpack, nodeBlockState, ASTNodeId::NODE_BLOCK_BLOCK_STATE);
            return ASTNode::andNode(node, {blockId, astNodeBlockState}, tokenReader.collect(),
                                    nullptr, ASTNodeId::NODE_BLOCK_BLOCK_AND_BLOCK_STATE);
        }
    };

    template<>
    struct Parser<Node::NodeCommand> {
        static ASTNode getASTNode(const Node::NodeCommand &node, TokenReader &tokenReader, const CPack *cpack) {
            tokenReader.push();
            ASTNode commandStart = parse(Node::NodeCommand::nodeCommandStart, tokenReader, cpack);
            if (HEDLEY_UNLIKELY(commandStart.isError())) {
                tokenReader.restore();
                tokenReader.push();
            }
            ASTNode commandName = tokenReader.readStringASTNode(node, ASTNodeId::NODE_COMMAND_COMMAND_NAME);
            if (HEDLEY_UNLIKELY(commandName.tokens.size() == 0)) {
                TokensView tokens = tokenReader.collect();
                return ASTNode::andNode(node, {std::move(commandName)}, tokens, ErrorReason::contentError(tokens, u"命令名字为空"), ASTNodeId::NODE_COMMAND_COMMAND);
            }
            std::u16string_view str = commandName.tokens.toString();
            const Node::NodePerCommand *currentCommand = nullptr;
            if (HEDLEY_LIKELY(!commandName.isError())) {
                bool isBreak = false;
                for (const auto &item: *node.commands) {
                    for (const auto &item2: item.name) {
                        if (HEDLEY_UNLIKELY(str == item2)) {
                            isBreak = true;
                            break;
                        }
                    }
                    if (HEDLEY_UNLIKELY(isBreak)) {
                        currentCommand = &item;
                        break;
                    }
                }
            }
            if (HEDLEY_UNLIKELY(currentCommand == nullptr)) {
                TokensView tokens = tokenReader.collect();
                return ASTNode::andNode(node, {std::move(commandName)}, tokens, ErrorReason::contentError(tokens, fmt::format(u"命令名字不匹配，找不到名为{}的命令", str)), ASTNodeId::NODE_COMMAND_COMMAND);
            }
            ASTNode usage = parse(*currentCommand, tokenReader, cpack);
            return ASTNode::andNode(node, {std::move(commandName), std::move(usage)},
                                    tokenReader.collect(), nullptr, ASTNodeId::NODE_COMMAND_COMMAND);
        }
    };

    template<>
    struct Parser<Node::NodeCommandName> {
        static ASTNode getASTNode(const Node::NodeCommandName &node, TokenReader &tokenReader, const CPack *cpack) {
            return tokenReader.readStringASTNode(node);
        }
    };

    template<>
    struct Parser<Node::NodeIntegerWithUnit> {
        static ASTNode getASTNode(const Node::NodeIntegerWithUnit &node, TokenReader &tokenReader, const CPack *cpack) {
            return parseByChildNode(node, tokenReader, cpack, node.nodeIntegerMaybeHaveUnit);
        }
    };

    ASTNode getOptionalASTNode(const Node::NodeItem &node,
                               TokenReader &tokenReader,
                               const CPack *cpack,
                               bool isIgnoreChildNodesError,
                               const std::vector<Node::NodeWithType> &childNodes,
                               const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE) {
        tokenReader.push();
        std::vector<ASTNode> childASTNodes;
        for (const auto &item: childNodes) {
            tokenReader.push();
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(item, index);
            ASTNode astNode = parse(item, tokenReader, cpack);
            DEBUG_GET_NODE_END(item, index);
            bool isError = astNode.isError();
            const TokensView tokens = tokenReader.collect();
            if (HEDLEY_UNLIKELY(isError && (isIgnoreChildNodesError || tokens.isEmpty()))) {
                tokenReader.restore();
                break;
            }
            childASTNodes.push_back(std::move(astNode));
            tokenReader.pop();
            if (HEDLEY_UNLIKELY(isError)) {
                break;
            }
        }
        return ASTNode::andNode(node, std::move(childASTNodes), tokenReader.collect(), nullptr, astNodeId);
    }

    template<>
    struct Parser<Node::NodeItem> {
        static ASTNode getASTNode(const Node::NodeItem &node, TokenReader &tokenReader, const CPack *cpack) {
            tokenReader.push();
            ASTNode itemId = parse(node.nodeItemId, tokenReader, cpack);
            std::u16string_view str = itemId.tokens.toString();
            XXH64_hash_t strHash = XXH3_64bits(str.data(), str.size() * sizeof(decltype(str)::value_type));
            std::shared_ptr<NamespaceId> currentItem = nullptr;
            for (const auto &item: *node.itemIds) {
                if (HEDLEY_UNLIKELY(item->fastMatch(strHash) || item->getIdWithNamespace()->fastMatch(strHash))) {
                    currentItem = item;
                    break;
                }
            }
            std::vector<ASTNode> childNodes = {std::move(itemId)};
            Node::NodeWithType nodeData = currentItem == nullptr ? CHelper::Node::NodeItem::nodeAllData : std::static_pointer_cast<ItemId>(currentItem)->getNode();
            switch (node.nodeItemType) {
                case Node::NodeItemType::ITEM_GIVE:
                    childNodes.push_back(getOptionalASTNode(
                            node, tokenReader, cpack, false,
                            {Node::NodeItem::nodeCount, nodeData, node.nodeComponent}));
                    break;
                case Node::NodeItemType::ITEM_CLEAR:
                    childNodes.push_back(getOptionalASTNode(
                            node, tokenReader, cpack, false,
                            {nodeData, Node::NodeItem::nodeCount}));
                    break;
                default:
                    childNodes.push_back(getOptionalASTNode(
                            node, tokenReader, cpack, false,
                            {Node::NodeItem::nodeCount, nodeData, node.nodeComponent}));
                    break;
            }
            return ASTNode::andNode(node, std::move(childNodes), tokenReader.collect());
        }
    };

    template<>
    struct Parser<Node::NodeJson> {
        static ASTNode getASTNode(const Node::NodeJson &node, TokenReader &tokenReader, const CPack *cpack) {
            //TODO 原始JSON文本的目标选择器要支持*
            return parseByChildNode(node, tokenReader, cpack, node.nodeJson);
        }
    };

    template<>
    struct Parser<Node::NodeLF> {
        static ASTNode getASTNode(const Node::NodeLF &node, TokenReader &tokenReader, const CPack *cpack) {
            tokenReader.push();
            tokenReader.skipToLF();
            TokensView tokens = tokenReader.collect();
            std::shared_ptr<ErrorReason> errorReason;
            if (HEDLEY_UNLIKELY(tokens.hasValue())) {
                errorReason = ErrorReason::excess(tokens, u"命令后面有多余部分 -> " + std::u16string(tokens.toString()));
            }
            return ASTNode::simpleNode(node, tokens, errorReason);
        }
    };

    template<>
    struct Parser<Node::NodeNamespaceId> {
        static ASTNode getASTNode(const Node::NodeNamespaceId &node, TokenReader &tokenReader, const CPack *cpack) {
            // namespace:id
            // 字符串中已经包含冒号，因为冒号不是结束字符
            DEBUG_GET_NODE_BEGIN(node, index);
            auto result = tokenReader.readStringASTNode(node);
            DEBUG_GET_NODE_END(node, index);
            if (HEDLEY_UNLIKELY(result.tokens.isEmpty())) {
                TokensView tokens = result.tokens;
                return ASTNode::andNode(node, {std::move(result)}, tokens, ErrorReason::incomplete(tokens, u"命令不完整"));
            }
            if (HEDLEY_UNLIKELY(!node.ignoreError.value_or(false))) {
                TokensView tokens = result.tokens;
                std::u16string_view str = tokens.toString();
                XXH64_hash_t strHash = XXH3_64bits(str.data(), str.size() * sizeof(decltype(str)::value_type));
                if (HEDLEY_UNLIKELY(std::all_of(node.customContents->begin(), node.customContents->end(), [&strHash](const auto &item) {
                        return !item->fastMatch(strHash) && !item->getIdWithNamespace()->fastMatch(strHash);
                    }))) {
                    return ASTNode::andNode(node, {std::move(result)}, tokens, ErrorReason::incomplete(tokens, fmt::format(u"找不到含义 -> {}", str)));
                }
            }
            return result;
        }
    };

    template<>
    struct Parser<Node::NodeNormalId> {
        static ASTNode getASTNode(const Node::NodeNormalId &node, TokenReader &tokenReader, const CPack *cpack) {
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(node, index);
            ASTNode result = node.getNormalIdASTNode(node, tokenReader);
            DEBUG_GET_NODE_END(node, index);
            if (HEDLEY_UNLIKELY(node.allowMissingID)) {
                if (HEDLEY_UNLIKELY(result.isError())) {
                    tokenReader.restore();
                    tokenReader.push();
                    return ASTNode::simpleNode(node, tokenReader.collect());
                }
                tokenReader.pop();
                return result;
            }
            tokenReader.pop();
            if (HEDLEY_UNLIKELY(result.tokens.isEmpty())) {
                TokensView tokens = result.tokens;
                return ASTNode::andNode(node, {std::move(result)}, tokens, ErrorReason::incomplete(tokens, u"命令不完整"));
            }
            if (HEDLEY_UNLIKELY(!node.ignoreError.value_or(true))) {
                TokensView tokens = result.tokens;
                std::u16string_view str = tokens.toString();
                XXH64_hash_t strHash = XXH3_64bits(str.data(), str.size() * sizeof(decltype(str)::value_type));
                if (HEDLEY_UNLIKELY(std::all_of(node.customContents->begin(), node.customContents->end(), [&strHash](const auto &item) {
                        return !item->fastMatch(strHash);
                    }))) {
                    return ASTNode::andNode(node, {std::move(result)}, tokens, ErrorReason::incomplete(tokens, fmt::format(u"找不到含义 -> {}", str)));
                }
            }
            return result;
        }
    };

    template<>
    struct Parser<Node::NodePerCommand> {
        static ASTNode getASTNode(const Node::NodePerCommand &node, TokenReader &tokenReader, const CPack *cpack) {
            std::vector<ASTNode> childASTNodes;
            childASTNodes.reserve(node.startNodes.size());
            for (const auto &item: node.startNodes) {
                tokenReader.push();
                DEBUG_GET_NODE_BEGIN(*item, index);
                childASTNodes.push_back(Parser<Node::NodeWrapped>::getASTNodeWithIsMustAfterSpace(*item, tokenReader, cpack, true));
                DEBUG_GET_NODE_END(*item, index);
                tokenReader.restore();
            }
            tokenReader.push();
            tokenReader.skipToLF();
            return ASTNode::orNode(node, std::move(childASTNodes), tokenReader.collect());
        }
    };

    namespace NodeRelativeFloatType {
        enum NodeRelativeFloatType : uint8_t {
            ABSOLUTE_COORDINATE,
            RELATIVE_WORLD_COORDINATE,
            LOCAL_COORDINATE,
        };
    }// namespace NodeRelativeFloatType

    std::pair<NodeRelativeFloatType::NodeRelativeFloatType, ASTNode>
    getRelativeFloatASTNode(const Node::NodeWithType &node,
                            const CPack *cpack,
                            TokenReader &tokenReader) {
        tokenReader.push();
        std::vector<ASTNode> childNodes;
        // 0 - 绝对坐标，1 - 相对坐标，2 - 局部坐标
        NodeRelativeFloatType::NodeRelativeFloatType type;
        tokenReader.push();
        ASTNode preSymbol = parse(Node::NodeRelativeFloat::nodePreSymbol, tokenReader, cpack);
        if (HEDLEY_UNLIKELY(preSymbol.isError())) {
            type = NodeRelativeFloatType::ABSOLUTE_COORDINATE;
            tokenReader.restore();
        } else {
            if (HEDLEY_LIKELY(preSymbol.childNodes[1].isError())) {
                type = NodeRelativeFloatType::RELATIVE_WORLD_COORDINATE;
            } else {
                type = NodeRelativeFloatType::LOCAL_COORDINATE;
            }
            tokenReader.pop();
            //空格检测
            if (HEDLEY_UNLIKELY(tokenReader.ready() && tokenReader.peek()->type == TokenType::SPACE)) {
                childNodes.push_back(std::move(preSymbol));
                return {type, ASTNode::andNode(node, std::move(childNodes), tokenReader.collect())};
            }
            childNodes.push_back(preSymbol);
        }
        //数值部分
        tokenReader.push();
        ASTNode number = tokenReader.readFloatASTNode(node, ASTNodeId::NODE_RELATIVE_FLOAT_NUMBER);
        std::shared_ptr<ErrorReason> errorReason;
        if (HEDLEY_LIKELY(!number.isError())) {
            tokenReader.pop();
        } else if (HEDLEY_UNLIKELY(childNodes.empty())) {
            tokenReader.pop();
            TokensView tokens = number.tokens;
            errorReason = ErrorReason::typeError(tokens, fmt::format(u"类型不匹配，{}不是有效的坐标参数", tokens.toString()));
        } else {
            tokenReader.restore();
        }
        childNodes.push_back(std::move(number));
        ASTNode result = ASTNode::andNode(node, std::move(childNodes), tokenReader.collect(), errorReason);
        // 为了获取补全提示，再嵌套一层or节点
        return {type, ASTNode::orNode(node, {std::move(result), std::move(preSymbol)}, nullptr)};
    }

    template<>
    struct Parser<Node::NodePosition> {
        static ASTNode getASTNode(const Node::NodePosition &node, TokenReader &tokenReader, const CPack *cpack) {
            tokenReader.push();
            // 0 - 绝对坐标，1 - 相对坐标，2 - 局部坐标
            std::vector<ASTNode> threeChildNodes;
            threeChildNodes.reserve(3);
            NodeRelativeFloatType::NodeRelativeFloatType types[3];
            for (NodeRelativeFloatType::NodeRelativeFloatType &type: types) {
                std::pair<NodeRelativeFloatType::NodeRelativeFloatType, ASTNode> childNode = getRelativeFloatASTNode(node, cpack, tokenReader);
                if (threeChildNodes.empty() && childNode.second.isError() && !childNode.second.tokens.isEmpty()) {
                    tokenReader.pop();
                    TokensView tokens = childNode.second.tokens;
                    return ASTNode::andNode(node, {std::move(childNode.second)}, tokens, nullptr, ASTNodeId::NODE_POSITION_POSITIONS);
                }
                type = childNode.first;
                threeChildNodes.push_back(std::move(childNode.second));
            }
            //判断有没有错误
            TokensView tokens = tokenReader.collect();
            ASTNode result = ASTNode::andNode(node, std::move(threeChildNodes), tokens, nullptr, ASTNodeId::NODE_POSITION_POSITIONS);
            if (HEDLEY_UNLIKELY(!result.isError())) {
                uint8_t count = 0;
                for (NodeRelativeFloatType::NodeRelativeFloatType item: types) {
                    if (HEDLEY_UNLIKELY(item == NodeRelativeFloatType::LOCAL_COORDINATE)) {
                        count++;
                    }
                }
                if (count == 1 || count == 2) {
                    return ASTNode::andNode(node, {std::move(result)}, tokens, nullptr, ASTNodeId::NODE_POSITION_POSITIONS_WITH_ERROR);
                }
            }
            return result;
        }
    };

    std::shared_ptr<ErrorReason> checkNumber(const TokensView &tokens, std::u16string_view str) {
        if (HEDLEY_UNLIKELY(str.empty())) {
            return ErrorReason::contentError(tokens, u"范围的数值为空");
        }
        for (size_t i = 0; i < str.length(); ++i) {
            size_t ch = str[i];
            if (HEDLEY_UNLIKELY(ch < '0' || ch > '9') && (i != 0 || (ch != '-' && ch != '+'))) {
                return ErrorReason::contentError(tokens, u"范围的数值格式不正确，检测非法字符");
            }
        }
        return nullptr;
    }

    template<>
    struct Parser<Node::NodeRange> {
        static ASTNode getASTNode(const Node::NodeRange &node, TokenReader &tokenReader, const CPack *cpack) {
            ASTNode result = tokenReader.readStringASTNode(node);
            std::u16string_view str = result.tokens.toString();
            std::shared_ptr<ErrorReason> errorReason;
            size_t index = str.find(u"..");
            if (HEDLEY_LIKELY(index == std::u16string::npos)) {
                errorReason = checkNumber(result.tokens, str);
            } else if (HEDLEY_UNLIKELY(index == 0)) {
                errorReason = checkNumber(result.tokens, std::u16string_view(str).substr(2));
            } else {
                errorReason = checkNumber(result.tokens, std::u16string_view(str).substr(0, index));
                if (HEDLEY_UNLIKELY(errorReason == nullptr && index + 2 < str.length())) {
                    errorReason = checkNumber(result.tokens, std::u16string_view(str).substr(index + 2));
                }
            }
            return ASTNode::simpleNode(node, result.tokens, errorReason);
        }
    };

    template<>
    struct Parser<Node::NodeRelativeFloat> {
        static ASTNode getASTNode(const Node::NodeRelativeFloat &node, TokenReader &tokenReader, const CPack *cpack) {
            std::pair<NodeRelativeFloatType::NodeRelativeFloatType, ASTNode> result = getRelativeFloatASTNode(node, cpack, tokenReader);
            if (HEDLEY_UNLIKELY(result.second.isError())) {
                return std::move(result.second);
            }
            if (HEDLEY_UNLIKELY(!node.canUseCaretNotation && result.first == NodeRelativeFloatType::LOCAL_COORDINATE)) {
                TokensView tokens = result.second.tokens;
                return ASTNode::andNode(node, {std::move(result.second)}, tokens, nullptr, ASTNodeId::NODE_RELATIVE_FLOAT_WITH_ERROR);
            }
            return result.second;
        }
    };

    template<>
    struct Parser<Node::NodeRepeat> {
        static ASTNode getASTNode(const Node::NodeRepeat &node, TokenReader &tokenReader, const CPack *cpack) {
            tokenReader.push();
            std::vector<ASTNode> childNodes;
            while (true) {
                ASTNode orNode = parse(node.nodeElement, tokenReader, cpack);
                bool isAstNodeError = orNode.childNodes[0].isError();
                bool isBreakAstNodeError = orNode.childNodes[1].isError();
                if (HEDLEY_UNLIKELY(!isBreakAstNodeError || isAstNodeError ||
                                    (!tokenReader.ready() && node.repeatData->isEnd[orNode.childNodes[0].whichBest]))) {
                    childNodes.push_back(std::move(orNode));
                    return ASTNode::andNode(node, std::move(childNodes), tokenReader.collect(), nullptr);
                }
                childNodes.push_back(std::move(orNode));
            }
        }
    };

    template<>
    struct Parser<Node::NodeString> {
        static ASTNode getASTNode(const Node::NodeString &node, TokenReader &tokenReader, const CPack *cpack) {
            if (HEDLEY_UNLIKELY(node.ignoreLater)) {
                //后面的所有内容都算作这个字符串
                tokenReader.push();
                tokenReader.skipToLF();
                TokensView tokens = tokenReader.collect();
                if (HEDLEY_UNLIKELY(!node.allowMissingString && tokens.isEmpty())) {
                    return ASTNode::simpleNode(node, tokens, ErrorReason::incomplete(tokens, u"字符串参数内容为空"));
                } else {
                    return ASTNode::simpleNode(node, tokens);
                }
            }
            tokenReader.push();
            ASTNode result = tokenReader.readStringASTNode(node);
            if (HEDLEY_UNLIKELY(node.allowMissingString && result.isError())) {
                tokenReader.restore();
                tokenReader.push();
                return ASTNode::simpleNode(node, tokenReader.collect());
            }
            tokenReader.pop();
            if (HEDLEY_UNLIKELY(!node.allowMissingString && result.tokens.isEmpty())) {
                return ASTNode::simpleNode(node, result.tokens, ErrorReason::incomplete(result.tokens, u"字符串参数内容为空"));
            }
            if (HEDLEY_UNLIKELY(!node.canContainSpace)) {
                if (HEDLEY_UNLIKELY(result.tokens.toString().find(' ') != std::u16string::npos)) {
                    return ASTNode::simpleNode(node, result.tokens, ErrorReason::contentError(result.tokens, u"字符串参数内容不可以包含空格"));
                }
                return result;
            }
            std::u16string_view str = result.tokens.toString();
            if (HEDLEY_LIKELY(str.empty() || str[0] != '"')) {
                return result;
            }
            auto convertResult = JsonUtil::jsonString2String(std::u16string(str));
            size_t offset = result.tokens.getStartIndex();
            if (HEDLEY_UNLIKELY(convertResult.errorReason != nullptr)) {
                convertResult.errorReason->start += offset;
                convertResult.errorReason->end += offset;
                return ASTNode::simpleNode(node, result.tokens, convertResult.errorReason);
            }
            if (HEDLEY_UNLIKELY(!convertResult.isComplete)) {
                return ASTNode::simpleNode(node, result.tokens, ErrorReason::contentError(result.tokens, fmt::format(u"字符串参数内容双引号不封闭 -> {}", str)));
            }
            return result;
        }
    };

    template<>
    struct Parser<Node::NodeTargetSelector> {
        static ASTNode getASTNode(const Node::NodeTargetSelector &node, TokenReader &tokenReader, const CPack *cpack) {
            tokenReader.skipSpace();
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(Node::NodeTargetSelector::nodeAt, nodeAtIndex);
            ASTNode at = parse(Node::NodeTargetSelector::nodeAt, tokenReader, cpack);
            DEBUG_GET_NODE_END(Node::NodeTargetSelector::nodeAt, nodeAtIndex);
            tokenReader.restore();
            if (HEDLEY_UNLIKELY(at.isError())) {
                //不是@符号开头
                if (node.isWildcard) {
                    // 尝试匹配通配符
                    tokenReader.push();
                    DEBUG_GET_NODE_BEGIN(Node::NodeTargetSelector::nodeWildcard, nodeWildcardIndex);
                    ASTNode wildcard = parse(Node::NodeTargetSelector::nodeWildcard, tokenReader, cpack);
                    DEBUG_GET_NODE_END(Node::NodeTargetSelector::nodeWildcard, nodeWildcardIndex);
                    if (wildcard.isError()) {
                        tokenReader.restore();
                    } else {
                        tokenReader.pop();
                        return wildcard;
                    }
                }
                // 当作玩家名处理
                DEBUG_GET_NODE_BEGIN(Node::NodeTargetSelector::nodePlayerName, nodePlayerNameIndex);
                ASTNode result = parseByChildNode(node, tokenReader, cpack, Node::NodeTargetSelector::nodePlayerName, ASTNodeId::NODE_TARGET_SELECTOR_PLAYER_NAME);
                DEBUG_GET_NODE_END(Node::NodeTargetSelector::nodePlayerName, nodePlayerNameIndex);
                return result;
            }
            //@符号开头，进入目标选择器检测
            //目标选择器变量
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(Node::NodeTargetSelector::nodeTargetSelectorVariable, nodeTargetSelectorVariableIndex);
            ASTNode targetSelectorVariable = parse(Node::NodeTargetSelector::nodeTargetSelectorVariable, tokenReader, cpack);
            DEBUG_GET_NODE_END(Node::NodeTargetSelector::nodeTargetSelectorVariable, nodeTargetSelectorVariableIndex);
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(Node::NodeTargetSelector::nodeLeft, nodeLeftIndex);
            ASTNode leftBracket = parse(Node::NodeTargetSelector::nodeLeft, tokenReader, cpack);
            DEBUG_GET_NODE_END(Node::NodeTargetSelector::nodeLeft, nodeLeftIndex);
            tokenReader.restore();
            if (HEDLEY_LIKELY(leftBracket.isError())) {
                //没有后面的[...]
                return ASTNode::andNode(node, {targetSelectorVariable}, tokenReader.collect(),
                                        nullptr, ASTNodeId::NODE_TARGET_SELECTOR_NO_ARGUMENTS);
            }
            ASTNode arguments = parse(node.nodeArguments, tokenReader, cpack);
            return ASTNode::andNode(node, {targetSelectorVariable, arguments}, tokenReader.collect(),
                                    nullptr, ASTNodeId::NODE_TARGET_SELECTOR_WITH_ARGUMENTS);
        }
    };

    template<>
    struct Parser<Node::NodeText> {
        static ASTNode getASTNode(const Node::NodeText &node, TokenReader &tokenReader, const CPack *cpack) {
            tokenReader.skipSpace();
            DEBUG_GET_NODE_BEGIN(node, index);
            auto result = node.getTextASTNode(node, tokenReader);
            DEBUG_GET_NODE_END(node, index);
            std::u16string_view str = result.tokens.toString();
            if (HEDLEY_UNLIKELY(str != node.data->name)) {
                TokensView tokens = result.tokens;
                if (HEDLEY_UNLIKELY(str.empty())) {
                    return ASTNode::andNode(node, {std::move(result)}, tokens, ErrorReason::contentError(tokens, u"命令不完整"));
                } else {
                    return ASTNode::andNode(node, {std::move(result)}, tokens, ErrorReason::contentError(tokens, fmt::format(u"找不到含义 -> {}", str)));
                }
            }
            return result;
        }
    };

    template<>
    struct Parser<Node::NodeAnd> {
        static ASTNode getASTNode(const Node::NodeAnd &node, TokenReader &tokenReader, const CPack *cpack) {
            tokenReader.push();
            std::vector<ASTNode> childASTNodes;
            bool isMustAfterSpace = false;
            for (size_t i = 0; i < node.childNodes.size(); ++i) {
                const auto &item = node.childNodes[i];
                if (item.nodeTypeId == Node::NodeTypeId::WRAPPED) {
                    const auto *nodeWrapped = reinterpret_cast<const Node::NodeWrapped *>(item.data);
                    ASTNode childNode = Parser<Node::NodeWrapped>::getASTNodeWithIsMustAfterSpace(*nodeWrapped, tokenReader, cpack, isMustAfterSpace);
                    bool isError = childNode.isError();
                    childASTNodes.push_back(std::move(childNode));
                    if (HEDLEY_UNLIKELY(isError)) {
                        break;
                    }
                    isMustAfterSpace = reinterpret_cast<const Node::NodeSerializable *>(nodeWrapped->innerNode.data)->getIsMustAfterSpace();
                } else {
                    ASTNode childNode = parse(item, tokenReader, cpack);
                    bool isError = childNode.isError();
                    childASTNodes.push_back(std::move(childNode));
                    if (HEDLEY_UNLIKELY(isError)) {
                        break;
                    }
                    if (HEDLEY_UNLIKELY(i < node.childNodes.size() - 1 &&
                                        tokenReader.ready() &&
                                        tokenReader.peek()->type == TokenType::SPACE)) {
                        tokenReader.push();
                        tokenReader.skip();
                        TokensView tokens = tokenReader.collect();
                        return ASTNode::andNode(node, std::move(childASTNodes), tokenReader.collect(),
                                                ErrorReason::contentError(tokens, u"意外的空格"));
                    }
                    isMustAfterSpace = false;
                }
            }
            return ASTNode::andNode(node, std::move(childASTNodes), tokenReader.collect());
        }
    };

    template<>
    struct Parser<Node::NodeAny> {
        static ASTNode getASTNode(const Node::NodeAny &node, TokenReader &tokenReader, const CPack *cpack) {
#ifdef CHelperDebug
            if (HEDLEY_UNLIKELY(!node.node.has_value())) {
                throw std::runtime_error("not init");
            }
#endif
            return parseByChildNode(node, tokenReader, cpack, *node.node);
        }
    };

    template<>
    struct Parser<Node::NodeEntry> {
        static ASTNode getASTNode(const Node::NodeEntry &node, TokenReader &tokenReader, const CPack *cpack) {
            tokenReader.push();
            std::vector<ASTNode> childNodes;
            auto key = parse(node.nodeKey, tokenReader, cpack);
            if (HEDLEY_UNLIKELY(key.isError())) {
                childNodes.push_back(std::move(key));
                return ASTNode::andNode(node, std::move(childNodes), tokenReader.collect());
            }
            childNodes.push_back(std::move(key));
            auto separator = parse(node.nodeSeparator, tokenReader, cpack);
            if (HEDLEY_UNLIKELY(separator.isError())) {
                childNodes.push_back(std::move(separator));
                return ASTNode::andNode(node, std::move(childNodes), tokenReader.collect());
            }
            childNodes.push_back(std::move(separator));
            childNodes.push_back(parse(node.nodeValue, tokenReader, cpack));
            return ASTNode::andNode(node, std::move(childNodes), tokenReader.collect());
        }
    };

    template<>
    struct Parser<Node::NodeEqualEntry> {
        static ASTNode getASTNode(const Node::NodeEqualEntry &node, TokenReader &tokenReader, const CPack *cpack) {
            tokenReader.push();
            std::vector<ASTNode> childNodes;
            // key
            ASTNode astNodeKey = parseByChildNode(node, tokenReader, cpack, node.nodeKey);
            childNodes.push_back(astNodeKey);
            if (HEDLEY_UNLIKELY(astNodeKey.isError())) {
                return ASTNode::andNode(node, std::move(childNodes), tokenReader.collect());
            }
            std::u16string_view key = astNodeKey.tokens.toString();
            auto it = std::find_if(node.equalDatas.begin(), node.equalDatas.end(), [&key](const auto &t) {
                return t.name == key;
            });
            // = or =!
            ASTNode astNodeSeparator = parseByChildNode(
                    node, tokenReader, cpack,
                    it == node.equalDatas.end() || it->canUseNotEqual
                            ? Node::NodeWithType(Node::NodeEqualEntry::nodeEqualOrNotEqual)
                            : Node::NodeWithType(Node::NodeEqualEntry::nodeEqual));
            childNodes.push_back(astNodeSeparator);
            if (HEDLEY_UNLIKELY(astNodeSeparator.isError())) {
                return ASTNode::andNode(node, std::move(childNodes), tokenReader.collect());
            }
            //value
            if (HEDLEY_UNLIKELY(it == node.equalDatas.end())) {
                Node::NodeInitialization<Node::NodeAny>::init(*Node::NodeAny::getNodeAny(), *cpack);
                childNodes.push_back(parse(*Node::NodeAny::getNodeAny(), tokenReader, cpack));
            } else {
                childNodes.push_back(parse(it->nodeValue, tokenReader, cpack));
            }
            return ASTNode::andNode(node, std::move(childNodes), tokenReader.collect());
        }
    };

    template<>
    struct Parser<Node::NodeList> {
        static ASTNode getASTNode(const Node::NodeList &node, TokenReader &tokenReader, const CPack *cpack) {
            //标记整个[...]，在最后进行收集
            tokenReader.push();
            ASTNode left = parse(node.nodeLeft, tokenReader, cpack);
            if (HEDLEY_UNLIKELY(left.isError())) {
                return ASTNode::andNode(node, {std::move(left)}, tokenReader.collect());
            }
            std::vector<ASTNode> childNodes = {std::move(left)};
            {
#ifdef CHelperDebug
                size_t startIndex = tokenReader.index;
#endif
                //检测[]中间有没有内容
                tokenReader.push();
                DEBUG_GET_NODE_BEGIN(node.nodeRight, nodeRightIndex);
                auto rightBracket1 = parse(node.nodeRight, tokenReader, cpack);
                DEBUG_GET_NODE_END(node.nodeRight, nodeRightIndex);
                tokenReader.restore();
                auto elementOrRight = parse(node.nodeElementOrRight, tokenReader, cpack);
                bool flag = !rightBracket1.isError() || elementOrRight.isError();
                childNodes.push_back(std::move(elementOrRight));
                if (HEDLEY_UNLIKELY(flag)) {
                    return ASTNode::andNode(node, std::move(childNodes), tokenReader.collect());
                }
#ifdef CHelperDebug
                if (HEDLEY_UNLIKELY(startIndex == tokenReader.index)) {
                    SPDLOG_WARN("NodeList has some error");
                    return ASTNode::andNode(node, std::move(childNodes), tokenReader.collect());
                }
#endif
            }
            while (true) {
#ifdef CHelperDebug
                size_t startIndex = tokenReader.index;
#endif
                //检测是分隔符还是右括号
                tokenReader.push();
                DEBUG_GET_NODE_BEGIN(node.nodeRight, nodeRightIndex);
                auto rightBracket = parse(node.nodeRight, tokenReader, cpack);
                DEBUG_GET_NODE_END(node.nodeRight, nodeRightIndex);
                tokenReader.restore();
                DEBUG_GET_NODE_BEGIN(node.nodeSeparatorOrRight, nodeSeparatorOrRightIndex);
                auto separatorOrRight = parse(node.nodeSeparatorOrRight, tokenReader, cpack);
                DEBUG_GET_NODE_END(node.nodeSeparatorOrRight, nodeSeparatorOrRightIndex);
                bool flag = !rightBracket.isError() || separatorOrRight.isError();
                childNodes.push_back(std::move(separatorOrRight));
                if (HEDLEY_UNLIKELY(flag)) {
                    return ASTNode::andNode(node, std::move(childNodes), tokenReader.collect());
                }
                //检测是不是元素
                DEBUG_GET_NODE_BEGIN(node.nodeElement, nodeElementIndex);
                ASTNode element = parse(node.nodeElement, tokenReader, cpack);
                DEBUG_GET_NODE_END(node.nodeElement, nodeElementIndex);
                flag = element.isError();
                childNodes.push_back(std::move(element));
                if (HEDLEY_UNLIKELY(flag)) {
                    return ASTNode::andNode(node, std::move(childNodes), tokenReader.collect());
                }
#ifdef CHelperDebug
                if (HEDLEY_UNLIKELY(startIndex == tokenReader.index)) {
                    SPDLOG_WARN("NodeList has some error");
                    return ASTNode::andNode(node, std::move(childNodes), tokenReader.collect());
                }
#endif
            }
        }
    };

    template<>
    struct Parser<Node::NodeOr> {
        static ASTNode getASTNode(const Node::NodeOr &node, TokenReader &tokenReader, const CPack *cpack) {
            std::vector<ASTNode> childASTNodes;
            std::vector<size_t> indexes;
            if (HEDLEY_LIKELY(!node.isUseFirst)) {
                childASTNodes.reserve(node.childNodes.size());
                indexes.reserve(node.childNodes.size());
            }
            for (const auto &item: node.childNodes) {
                tokenReader.push();
                ASTNode childNode = parse(item, tokenReader, cpack);
                bool isNodeError = childNode.isError();
                childASTNodes.push_back(std::move(childNode));
                indexes.push_back(tokenReader.index);
                tokenReader.restore();
                if (HEDLEY_UNLIKELY(node.isUseFirst && !isNodeError)) {
                    break;
                }
            }
            if (HEDLEY_UNLIKELY(node.isAttachToEnd)) {
                tokenReader.push();
                tokenReader.skipToLF();
                const TokensView tokens = tokenReader.collect();
                return ASTNode::orNode(node, std::move(childASTNodes), tokens, node.defaultErrorReason, node.nodeId);
            } else {
                ASTNode result = ASTNode::orNode(node, std::move(childASTNodes), nullptr, node.defaultErrorReason, node.nodeId);
                tokenReader.index = indexes[result.whichBest];
                return result;
            }
        }
    };

    template<>
    struct Parser<Node::NodeSingleSymbol> {
        static ASTNode getASTNode(const Node::NodeSingleSymbol &node, TokenReader &tokenReader, const CPack *cpack) {
            ASTNode symbolNode = tokenReader.readSymbolASTNode(node);
            std::shared_ptr<ErrorReason> errorReason;
            if (HEDLEY_UNLIKELY(symbolNode.isError())) {
                if (HEDLEY_LIKELY(symbolNode.tokens.isEmpty())) {
                    return ASTNode::simpleNode(node, symbolNode.tokens, ErrorReason::incomplete(symbolNode.tokens, fmt::format(u"命令不完整，需要符号{:c}", node.symbol)));
                } else {
                    return ASTNode::simpleNode(node, symbolNode.tokens, ErrorReason::typeError(symbolNode.tokens, fmt::format(u"类型不匹配，需要符号{:c}，但当前内容为{}", node.symbol, symbolNode.tokens.toString())));
                }
            }
            std::u16string_view str = symbolNode.tokens.toString();
            if (HEDLEY_LIKELY(str.length() == 1 && str[0] == node.symbol)) {
                return symbolNode;
            }
            return ASTNode::simpleNode(node, symbolNode.tokens, ErrorReason::contentError(symbolNode.tokens, fmt::format(u"内容不匹配，正确的符号为{:c}，但当前内容为{}", node.symbol, str)));
        }
    };

    template<bool isJson>
    struct Parser<Node::NodeTemplateBoolean<isJson>> {
        static ASTNode getASTNode(const Node::NodeTemplateBoolean<isJson> &node, TokenReader &tokenReader, const CPack *cpack) {
            ASTNode astNode = tokenReader.readStringASTNode(node);
            std::u16string_view str = astNode.tokens.toString();
            if (HEDLEY_LIKELY(str == u"true" || str == u"false")) {
                return astNode;
            }
            TokensView tokens = astNode.tokens;
            return ASTNode::andNode(node, {std::move(astNode)}, tokens,
                                    ErrorReason::contentError(tokens, fmt::format(u"内容不匹配，应该为布尔值，但当前内容为{}", str)));
        }
    };

    template<class T, bool isJson>
    struct Parser<Node::NodeTemplateNumber<T, isJson>> {
        static ASTNode getASTNode(const Node::NodeTemplateNumber<T, isJson> &node, TokenReader &tokenReader, const CPack *cpack) {
            if constexpr (std::numeric_limits<T>::is_integer) {
                return tokenReader.readIntegerASTNode(node);
            } else {
                return tokenReader.readFloatASTNode(node);
            }
        }
    };

    ASTNode parse(const Node::NodeWithType &node, TokenReader &tokenReader, const CPack *cpack) {
        switch (node.nodeTypeId) {
            CODEC_PASTE(CHELPER_GET_AST_NODE, CHELPER_NODE_TYPES)
            default:
                HEDLEY_UNREACHABLE();
        }
    }

    ASTNode parse(const std::u16string &content, const CPack *cpack, const Node::NodeWithType &mainNode) {
        TokenReader tokenReader = TokenReader(std::make_shared<LexerResult>(Lexer::lex(content)));
#ifdef CHelperTest
        Profile::push("start parsing: {}", FORMAT_ARG(utf8::utf16to8(tokenReader.lexerResult->content)));
#endif
        DEBUG_GET_NODE_BEGIN(mainNode, index);
        auto result = parse(mainNode, tokenReader, cpack);
        DEBUG_GET_NODE_END(mainNode, index);
#ifdef CHelperTest
        Profile::pop();
#endif
        return result;
    }

    ASTNode parse(const std::u16string &content, const CPack *cpack) {
        return parse(content, cpack, cpack->mainNode);
    }

}// namespace CHelper::Parser