//
// Created by Yancey on 2023/12/20.
//

#include "NodeRelativeFloat.h"
#include "../../util/TokenUtil.h"
#include "../util/NodeOr.h"
#include "../util/NodeSingleSymbol.h"

namespace CHelper::Node {

    static std::unique_ptr<NodeBase> nodeRelativeNotation = std::make_unique<NodeSingleSymbol>(
            "RELATIVE_FLOAT_RELATIVE_NOTATION", "相对坐标（~x ~y ~z）", '~');
    static std::unique_ptr<NodeBase> nodeCaretNotation = std::make_unique<NodeSingleSymbol>(
            "RELATIVE_FLOAT_CARET_NOTATION", "局部坐标（^左 ^上 ^右）", '^');
    static std::unique_ptr<NodeBase> nodePreSymbol = std::make_unique<NodeOr>(
            "RELATIVE_FLOAT_RELATIVE", "相对坐标（~x ~y ~z）",
            std::vector<const NodeBase *>{nodeRelativeNotation.get(), nodeCaretNotation.get()}, false);

    NodeRelativeFloat::NodeRelativeFloat(const std::optional<std::string> &id,
                                         const std::optional<std::string> &description,
                                         bool canUseCaretNotation)
        : NodeBase(id, description, false),
          canUseCaretNotation(canUseCaretNotation) {}

    NodeRelativeFloat::NodeRelativeFloat(const nlohmann::json &j,
                                         [[maybe_unused]] const CPack &cpack)
        : NodeBase(j, true),
          canUseCaretNotation(JsonUtil::read<bool>(j, "canUseCaretNotation")) {}

    NodeRelativeFloat::NodeRelativeFloat(BinaryReader &binaryReader,
                                         [[maybe_unused]] const CPack &cpack)
        : NodeBase(binaryReader),
          canUseCaretNotation(binaryReader.read<bool>()) {}

    NodeType *NodeRelativeFloat::getNodeType() const {
        return NodeType::RELATIVE_FLOAT.get();
    }

    void NodeRelativeFloat::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::encode(j, "canUseCaretNotation", canUseCaretNotation);
    }

    void NodeRelativeFloat::writeBinToFile(BinaryWriter &binaryWriter) const {
        NodeBase::writeBinToFile(binaryWriter);
        binaryWriter.encode(canUseCaretNotation);
    }

    ASTNode NodeRelativeFloat::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        std::pair<uint8_t, ASTNode> result = getASTNode(this, cpack, tokenReader);
        if (HEDLEY_UNLIKELY(result.second.isError())) {
            return std::move(result.second);
        }
        if (HEDLEY_UNLIKELY(!canUseCaretNotation && result.first == 2)) {
            VectorView<Token> tokens = result.second.tokens;
            return ASTNode::andNode(this, {std::move(result.second)}, tokens, ErrorReason::logicError(tokens, "不能使用局部坐标"), "relativeFloat");
        }
        return result.second;
    }

    std::pair<uint8_t, ASTNode> NodeRelativeFloat::getASTNode(const NodeBase *node,
                                                                   const CPack *cpack,
                                                                   TokenReader &tokenReader) {
        tokenReader.push();
        std::vector<ASTNode> childNodes;
        // 0 - 绝对坐标，1 - 相对坐标，2 - 局部坐标
        uint8_t type;
        tokenReader.push();
        ASTNode preSymbol = nodePreSymbol->getASTNode(tokenReader, cpack);
        if (HEDLEY_UNLIKELY(preSymbol.isError())) {
            type = 0;
            tokenReader.restore();
        } else {
            if (HEDLEY_LIKELY(preSymbol.childNodes[1].isError())) {
                type = 1;
            } else {
                type = 2;
            }
            tokenReader.pop();
            //空格检测
            if (HEDLEY_UNLIKELY(tokenReader.ready() && tokenReader.peek()->type == TokenType::WHITE_SPACE)) {
                childNodes.push_back(std::move(preSymbol));
                return {type, ASTNode::andNode(node, std::move(childNodes), tokenReader.collect())};
            }
            childNodes.push_back(preSymbol);
        }
        //数值部分
        tokenReader.push();
        ASTNode number = tokenReader.readFloatASTNode(node);
        std::shared_ptr<ErrorReason> errorReason;
        if (HEDLEY_LIKELY(!number.isError())) {
            tokenReader.pop();
        } else if (HEDLEY_UNLIKELY(childNodes.empty())) {
            tokenReader.pop();
            VectorView<Token> tokens = number.tokens;
            errorReason = ErrorReason::typeError(tokens, FormatUtil::format(
                                                                 "类型不匹配，{0}不是有效的坐标参数",
                                                                 TokenUtil::toString(tokens)));
        } else {
            tokenReader.restore();
        }
        childNodes.push_back(std::move(number));
        ASTNode result = ASTNode::andNode(node, std::move(childNodes), tokenReader.collect(), errorReason);
        // 为了获取补全提示，再嵌套一层or节点
        return {type, ASTNode::orNode(node, {std::move(result), std::move(preSymbol)}, nullptr)};
    }

    void NodeRelativeFloat::collectStructure(const ASTNode *astNode,
                                             StructureBuilder &structure,
                                             bool isMustHave) const {
        structure.append(isMustHave, description.value_or("坐标"));
    }

}// namespace CHelper::Node