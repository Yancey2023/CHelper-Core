//
// Created by Yancey on 2024/2/20.
//

#include "NodeTargetSelectorArgument.h"
#include "../param/NodeString.h"
#include "../param/NodeText.h"
#include "../../util/TokenUtil.h"
#include "../param/NodeNormalId.h"
#include "../param/NodeFloat.h"
#include "../param/NodeRelativeFloat.h"
#include "../util/NodeOr.h"

namespace CHelper::Node {

    static std::shared_ptr<NodeBase> nodeKey = std::make_shared<NodeNormalId>(
            "TARGET_SELECTOR_ARGUMENT_KEY", "目标选择器参数名", std::nullopt,
            std::make_shared<std::vector<std::shared_ptr<NormalId>>>(std::vector<std::shared_ptr<NormalId>>{
                    std::make_shared<NormalId>("x", "x坐标"),
                    std::make_shared<NormalId>("y", "y坐标"),
                    std::make_shared<NormalId>("z", "z坐标"),
                    std::make_shared<NormalId>("r", "半径小于等于r"),
                    std::make_shared<NormalId>("rm", "半径大于等于rm"),
                    std::make_shared<NormalId>("dx", "x坐标差异(检查实体的腿部位置)"),
                    std::make_shared<NormalId>("dy", "y坐标差异(检查实体的腿部位置)"),
                    std::make_shared<NormalId>("dz", "z坐标差异(检查实体的腿部位置)"),
                    std::make_shared<NormalId>("scores", "分数"),
                    std::make_shared<NormalId>("tag", "标签"),
                    std::make_shared<NormalId>("name", "名字"),
                    std::make_shared<NormalId>("type", "实体类型"),
                    std::make_shared<NormalId>("family", "族"),
                    std::make_shared<NormalId>("rx", "垂直旋转小于等于rx"),
                    std::make_shared<NormalId>("rxm", "垂直旋转大于等于rxm"),
                    std::make_shared<NormalId>("ry", "水平旋转大于等于ry"),
                    std::make_shared<NormalId>("rym", "水平旋转大于等于rym"),
                    std::make_shared<NormalId>("l", "经验等级小于等于l"),
                    std::make_shared<NormalId>("lm", "经验等级大于等于lm"),
                    std::make_shared<NormalId>("m", "游戏模式"),
                    std::make_shared<NormalId>("c", "目标数量(按照距离排序)"),
            })
    );
    static std::shared_ptr<NodeBase> nodeEqual = std::make_shared<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_EQUAL", "等于",
            std::make_shared<NormalId>("=", "等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readSymbolASTNode(node);
            });
    static std::shared_ptr<NodeBase> nodeNotEqual = std::make_shared<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_NOT_EQUAL", "不等于",
            std::make_shared<NormalId>("!=", "不等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readSymbolASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    static std::shared_ptr<NodeBase> nodeSeparator = std::make_shared<NodeOr>(
            "TARGET_SELECTOR_ARGUMENT_SEPARATOR", "目标选择器参数分隔符",
            std::make_shared<std::vector<std::shared_ptr<NodeBase>>>(std::vector<std::shared_ptr<NodeBase>>{
                    nodeEqual, nodeNotEqual
            }), false);
    static std::shared_ptr<NodeBase> nodePosition = std::make_shared<NodeFloat>(
            "TARGET_SELECTOR_ARGUMENT_POSITION", "目标选择器参数值(位置)", std::nullopt, std::nullopt);
    static std::shared_ptr<NodeBase> nodeFloat = std::make_shared<NodeRelativeFloat>(
            "TARGET_SELECTOR_ARGUMENT_FLOAT", "目标选择器参数值(小数)", true);
    static std::shared_ptr<NodeBase> nodeString = std::make_shared<NodeString>(
            "TARGET_SELECTOR_ARGUMENT_STRING", "目标选择器参数值(字符串)", true, false);
    static std::shared_ptr<NodeBase> nodeValue = std::make_shared<NodeOr>(
            "TARGET_SELECTOR_ARGUMENT_VALUE", "目标选择器参数值",
            std::make_shared<std::vector<std::shared_ptr<NodeBase>>>(std::vector<std::shared_ptr<NodeBase>>{
                    nodePosition, nodeFloat
            }), false);

    NodeTargetSelectorArgument::NodeTargetSelectorArgument(const std::optional<std::string> &id,
                                                           const std::optional<std::string> &description,
                                                           const std::shared_ptr<NodeBase> &nodeItem,
                                                           const std::shared_ptr<NodeBase> &nodeFamily,
                                                           const std::shared_ptr<NodeBase> &nodeGameMode,
                                                           const std::shared_ptr<NodeBase> &nodeItemLocation)
            : NodeBase(id, description, false),
              nodeItem(nodeItem),
              nodeFamily(nodeFamily),
              nodeGameMode(nodeGameMode),
              nodeItemLocation(nodeItemLocation) {
        checkNullptr(nodeItem, "nodeItem");
        checkNullptr(nodeFamily, "nodeFamily");
        checkNullptr(nodeGameMode, "nodeGameMode");
        checkNullptr(nodeGameMode, "nodeGameMode");
    }

    ASTNode NodeTargetSelectorArgument::getASTNode(TokenReader &tokenReader) const {
        tokenReader.push();
        std::vector<ASTNode> childNodes;
        // key
        ASTNode astNodeKey = getByChildNode(tokenReader, nodeKey, "key");
        childNodes.push_back(astNodeKey);
        if (astNodeKey.isError()) {
            return ASTNode::andNode(this, childNodes, tokenReader.collect());
        }
        // = or !=
        ASTNode astNodeSeparator = getByChildNode(tokenReader, nodeSeparator, "separator");
        childNodes.push_back(astNodeSeparator);
        if (astNodeSeparator.isError()) {
            return ASTNode::andNode(this, childNodes, tokenReader.collect());
        }
        //value
        std::string key = TokenUtil::toString(astNodeKey.tokens);
        if (key == "x" || key == "y" || key == "z") {
            childNodes.push_back(nodePosition->getASTNode(tokenReader));
        } else if (key == "r" || key == "rm" || key == "dx" || key == "dy" || key == "dz" ||
                   key == "rx" || key == "rxm" || key == "ry" || key == "rym" ||
                   key == "l" || key == "lm") {
            childNodes.push_back(nodeFloat->getASTNode(tokenReader));
        } else if (key == "scores") {
            // TODO scores
        } else if (key == "tag" || key == "name" || key == "type" || key == "family") {
            ASTNode string = nodeString->getASTNode(tokenReader);
            if (string.isError() && string.tokens.isEmpty()) {
                childNodes.push_back(ASTNode::simpleNode(this, string.tokens));
            } else {
                childNodes.push_back(string);
            }
        } else if (key == "scores") {
            //TODO scores
            childNodes.push_back(nodeValue->getASTNode(tokenReader));
        } else if (key == "hasitem") {
            //TODO hasitem
            childNodes.push_back(nodeValue->getASTNode(tokenReader));
        } else if (key == "has_property") {
            //TODO has_property
            childNodes.push_back(nodeValue->getASTNode(tokenReader));
        } else if (key == "haspermission") {
            //TODO haspermission
            childNodes.push_back(nodeValue->getASTNode(tokenReader));
        } else {
            childNodes.push_back(nodeValue->getASTNode(tokenReader));
        }
        return ASTNode::andNode(this, childNodes, tokenReader.collect());
    }

} // CHelper::Node