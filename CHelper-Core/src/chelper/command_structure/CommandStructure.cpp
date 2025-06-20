//
// Created by Yancey on 2025-06-20.
//

#include <chelper/command_structure/CommandStructure.h>
#include <chelper/command_structure/StructureBuilder.h>
#include <chelper/node/NodeType.h>

#define CHELPER_CODEC_COLLECT_STRUCTURE(v1) \
    case Node::NodeTypeId::v1:              \
        return CommandStructure<typename Node::NodeTypeDetail<Node::NodeTypeId::v1>::Type>::collectStructure(astNode, reinterpret_cast<const typename Node::NodeTypeDetail<Node::NodeTypeId::v1>::Type &>(node), structure, isMustHave);

namespace CHelper::CommandStructure {

    bool collectNodeStructure(const ASTNode *astNode, const Node::NodeBase &node, StructureBuilder &structure, bool isMustHave);

    void collectNodeStructureOrUnknown(const ASTNode *astNode, const Node::NodeBase &node, StructureBuilder &structure, bool isMustHave) {
        if (!collectNodeStructure(astNode, node, structure, isMustHave)) {
            structure.appendUnknown(isMustHave);
        }
    }

    template<class NodeType, class = void>
    struct CommandStructure {
        static_assert(std::is_base_of_v<Node::NodeBase, NodeType>, "NodeType must be derived from NodeBase");
        static_assert(!std::is_same_v<Node::NodeBase, NodeType>, "NodeType must not be NodeBase");
        static bool collectStructure(const ASTNode *astNode, const NodeType &node, StructureBuilder &structure, bool isMustHave) {
            return false;
        }
    };

    template<bool isJson>
    struct CommandStructure<Node::NodeTemplateBoolean<isJson>> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeTemplateBoolean<isJson> &node, StructureBuilder &structure, bool isMustHave) {
            structure.append(isMustHave, node.brief.value_or(node.description.value_or(u"布尔值")));
            return true;
        }
    };

    template<class T, bool isJson>
    struct CommandStructure<Node::NodeTemplateNumber<T, isJson>> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeTemplateNumber<T, isJson> &node, StructureBuilder &structure, bool isMustHave) {
            constexpr auto defaultStr = std::numeric_limits<T>::is_integer ? u"整数" : u"数字";
            structure.append(isMustHave, node.brief.value_or(node.description.value_or(defaultStr)));
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeBlock> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeBlock &node, StructureBuilder &structure, bool isMustHave) {
            structure.append(isMustHave, u"方块ID");
            if (HEDLEY_LIKELY(node.nodeBlockType == Node::NodeBlockType::BLOCK_WITH_BLOCK_STATE)) {
                structure.append(false, u"方块状态");
            }
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeCommand> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeCommand &node, StructureBuilder &structure, bool isMustHave) {
            if (astNode == nullptr || astNode->childNodes.size() < 2 || astNode->tokens.size() < 2) {
                structure.append(isMustHave, u"命令");
            } else {
                structure.appendSpace().append(std::u16string(astNode->childNodes[0].tokens.toString()));
                const auto &usage = astNode->childNodes[1];
                collectNodeStructure(&usage, *usage.node, structure, isMustHave);
            }
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeCommandName> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeCommandName &node, StructureBuilder &structure, bool isMustHave) {
            structure.append(isMustHave, node.brief.value_or(u"命令名"));
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeIntegerWithUnit> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeIntegerWithUnit &node, StructureBuilder &structure, bool isMustHave) {
            structure.append(isMustHave, node.brief.value_or(node.description.value_or(u"可能带单位的整数")));
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeItem> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeItem &node, StructureBuilder &structure, bool isMustHave) {
            switch (node.nodeItemType) {
                case Node::NodeItemType::ITEM_GIVE:
                    collectNodeStructureOrUnknown(nullptr, *node.nodeItemId, structure, isMustHave);
                    collectNodeStructureOrUnknown(nullptr, *Node::NodeItem::nodeCount, structure, false);
                    collectNodeStructureOrUnknown(nullptr, *Node::NodeItem::nodeAllData, structure, false);
                    collectNodeStructureOrUnknown(nullptr, *node.nodeComponent, structure, false);
                    break;
                case Node::NodeItemType::ITEM_CLEAR:
                    collectNodeStructureOrUnknown(nullptr, *node.nodeItemId, structure, isMustHave);
                    collectNodeStructureOrUnknown(nullptr, *Node::NodeItem::nodeAllData, structure, false);
                    collectNodeStructureOrUnknown(nullptr, *Node::NodeItem::nodeCount, structure, false);
                    break;
                default:
                    collectNodeStructureOrUnknown(nullptr, *node.nodeItemId, structure, isMustHave);
                    collectNodeStructureOrUnknown(nullptr, *Node::NodeItem::nodeCount, structure, false);
                    collectNodeStructureOrUnknown(nullptr, *Node::NodeItem::nodeAllData, structure, false);
                    collectNodeStructureOrUnknown(nullptr, *node.nodeComponent, structure, false);
                    break;
            }
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeJson> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeJson &node, StructureBuilder &structure, bool isMustHave) {
            structure.append(isMustHave, node.brief.value_or(node.description.value_or(u"JSON文本")));
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeLF> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeLF &node, StructureBuilder &structure, bool isMustHave) {
            structure.append(u"\n");
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeNamespaceId> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeNamespaceId &node, StructureBuilder &structure, bool isMustHave) {
            structure.append(isMustHave, node.brief.value_or(node.description.value_or(u"ID")));
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeNormalId> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeNormalId &node, StructureBuilder &structure, bool isMustHave) {
            structure.append(isMustHave, node.brief.value_or(node.description.value_or(u"ID")));
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodePerCommand> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodePerCommand &node, StructureBuilder &structure, bool isMustHave) {
            const auto &bestNode = astNode->getBestNode();
            collectNodeStructureOrUnknown(&bestNode, *bestNode.node, structure, isMustHave);
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodePosition> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodePosition &node, StructureBuilder &structure, bool isMustHave) {
            structure.append(isMustHave, node.brief.value_or(u"位置"));
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeRange> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeRange &node, StructureBuilder &structure, bool isMustHave) {
            structure.append(isMustHave, node.brief.value_or(node.description.value_or(u"范围")));
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeRelativeFloat> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeRelativeFloat &node, StructureBuilder &structure, bool isMustHave) {
            structure.append(isMustHave, node.brief.value_or(node.description.value_or(u"坐标")));
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeRepeat> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeRepeat &node, StructureBuilder &structure, bool isMustHave) {
            bool isAddMoreSymbol = false;
            bool isAddMiddle = false;
            if (astNode != nullptr) {
                for (const auto &item: astNode->childNodes) {
                    //如果内容为空，就跳过
                    if (HEDLEY_UNLIKELY(item.tokens.isEmpty() || item.tokens.isAllSpace())) {
                        continue;
                    }
                    //获取结构
                    const ASTNode &astNode1 = HEDLEY_LIKELY(item.whichBest == 0)
                                                      ? item.getBestNode().getBestNode()
                                                      : item.getBestNode();
                    auto node1 = reinterpret_cast<const Node::NodeAnd *>(astNode1.node);
                    size_t astNodeSize = astNode1.childNodes.size();
                    size_t nodeSize = node1->childNodes.size();
                    if (HEDLEY_LIKELY(astNode1.isError())) {
                        for (size_t i = 0; i < nodeSize; ++i) {
                            if (HEDLEY_LIKELY(i < astNodeSize)) {
                                const auto &item2 = astNode1.childNodes[i];
                                collectNodeStructureOrUnknown(&item2, *item2.node, structure, true);
                            } else {
                                collectNodeStructureOrUnknown(nullptr, *node1->childNodes[i], structure, true);
                            }
                            isAddMiddle = true;
                        }
                    } else {
                        if (!isAddMoreSymbol) {
                            isAddMoreSymbol = true;
                            structure.appendSpace().append(u"...");
                        }
                    }
                    if (HEDLEY_UNLIKELY(item.whichBest == 1)) {
                        //如果是结束语句，就不继续执行了
                        return true;
                    }
                }
            }
            //如果没有遇到结束语句，添加...和结束语句的结构
            if (!isAddMoreSymbol || isAddMiddle) {
                structure.appendSpace().append(u"...");
            }
            for (const auto &item: reinterpret_cast<const Node::NodeAnd *>(reinterpret_cast<const Node::NodeOr *>(node.nodeElement)->childNodes[1])->childNodes) {
                collectNodeStructure(nullptr, *item, structure, true);
            }
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeString> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeString &node, StructureBuilder &structure, bool isMustHave) {
            structure.append(isMustHave, node.brief.value_or(node.description.value_or(u"字符串")));
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeTargetSelector> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeTargetSelector &node, StructureBuilder &structure, bool isMustHave) {
            structure.append(isMustHave, node.brief.value_or(u"目标选择器"));
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeText> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeText &node, StructureBuilder &structure, bool isMustHave) {
            structure.appendSpace().append(node.data->name);
            return true;
        }
    };

    template<>
    struct CommandStructure<Node::NodeWrapped> {
        static bool collectStructure(const ASTNode *astNode, const Node::NodeWrapped &node, StructureBuilder &structure, bool isMustHave) {
            if (astNode == nullptr || (astNode->mode == ASTNodeMode::NONE && astNode->isAllSpaceError())) {
                collectNodeStructureOrUnknown(nullptr, *node.innerNode, structure, isMustHave);
                if (isMustHave) {
                    for (const auto &item: node.nextNodes) {
                        if (HEDLEY_UNLIKELY(item->innerNode->nodeTypeId == Node::NodeTypeId::LF)) {
                            isMustHave = false;
                            break;
                        }
                    }
                }
                if (!node.nextNodes.empty()) {
                    collectNodeStructureOrUnknown(nullptr, *node.nextNodes[0], structure, isMustHave);
                }
                return true;
            } else {
                collectNodeStructureOrUnknown(&astNode->childNodes[0], *node.innerNode, structure, isMustHave);
                if (node.nextNodes.empty()) {
                    return true;
                }
                if (isMustHave) {
                    for (const auto &item: node.nextNodes) {
                        if (HEDLEY_UNLIKELY(item->innerNode->nodeTypeId == Node::NodeTypeId::LF)) {
                            isMustHave = false;
                            break;
                        }
                    }
                }
                if (astNode->childNodes.size() < 2) {
                    return collectStructure(nullptr, *node.nextNodes[0], structure, isMustHave);
                } else {
                    const auto &bestASTNode = astNode->childNodes[1].getBestNode();
                    return collectNodeStructure(&bestASTNode, *bestASTNode.node, structure, isMustHave);
                }
            }
        }
    };

    bool collectNodeStructure(const ASTNode *astNode, const Node::NodeBase &node, StructureBuilder &structure, bool isMustHave) {
        switch (node.nodeTypeId) {
            CODEC_PASTE(CHELPER_CODEC_COLLECT_STRUCTURE, CHELPER_NODE_TYPES)
            default:
                HEDLEY_UNREACHABLE();
        }
    }

    std::u16string getStructure(const ASTNode &astNode) {
        StructureBuilder structureBuilder;
        collectNodeStructure(&astNode, *astNode.node, structureBuilder, true);
        std::u16string result = structureBuilder.build();
        while (HEDLEY_UNLIKELY(!result.empty() && result[result.size() - 1] == '\n')) {
            result.pop_back();
        }
        return result;
    }

}// namespace CHelper::CommandStructure
