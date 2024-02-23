//
// Created by Yancey on 2023/11/30.
//

#include "Exception.h"
#include "Profile.h"
#include "ColorStringBuilder.h"
#include "SimpleLogger.h"

namespace CHelper::Exception {

    void printStackTrace(const std::exception& e) {
        CHELPER_ERROR(std::string(e.what()) + "\nstack trace:\n" + Profile::getStackTrace());
        Profile::clear();
    }

    CantCreateInstance::CantCreateInstance(const std::string &className) {
        Profile::push(ColorStringBuilder()
                              .red("You create a instance which you shouldn't create : ")
                              .purple(className)
                              .build());
    }

    const char *CantCreateInstance::what() const noexcept {
        return "[CHelper] You create a instance which you shouldn't create";
    }

    CPackLoadFailed::CPackLoadFailed() {
        Profile::push(ColorStringBuilder()
                              .red("CPack load failed")
                              .build());
    }

    const char *CPackLoadFailed::what() const noexcept {
        return "[CHelper] CPack load failed";
    }

    UnknownIdType::UnknownIdType(const std::string &fileName, const std::string &idType) {
        Profile::push(ColorStringBuilder()
                              .red("unknown id type")
                              .normal(" -> ")
                              .purple(idType)
                              .red(" (in file \"")
                              .purple(fileName)
                              .red("\")")
                              .build());
    }

    const char *UnknownIdType::what() const noexcept {
        return "[CHelper] unknown id type";
    }

    UnknownNodeType::UnknownNodeType(const std::string &nodeType) {
        Profile::push(ColorStringBuilder()
                              .red("unknown node type")
                              .normal(" -> ")
                              .purple(nodeType)
                              .build());
    }

    const char *UnknownNodeType::what() const noexcept {
        return "[CHelper] unknown node type";
    }

    UnknownNodeId::UnknownNodeId(const std::vector<std::string> &commandName, const std::string &nodeId) {
        Profile::push(ColorStringBuilder()
                              .red("unknown node id")
                              .normal(" -> ")
                              .purple(nodeId)
                              .red(" (in node \"")
                              .purple(getCommandName(commandName))
                              .red("\")")
                              .build());
    }

    const char *UnknownNodeId::what() const noexcept {
        return "[CHelper] unknown node id";
    }

    RequireParentNodeId::RequireParentNodeId(const std::vector<std::string> &commandName) {
        Profile::push(ColorStringBuilder()
                              .red("dismiss parent node id")
                              .red(" (in node \"")
                              .purple(getCommandName(commandName))
                              .red("\")")
                              .build());
    }

    const char *RequireParentNodeId::what() const noexcept {
        return "[CHelper] dismiss parent node id";
    }

    RequireChildNodeIds::RequireChildNodeIds(const std::vector<std::string> &commandName,
                                             const std::string &parentNodeId) {
        Profile::push(ColorStringBuilder()
                              .red("dismiss child node ids, the parent node")
                              .normal(" -> ")
                              .purple(parentNodeId)
                              .red(" (in node \"")
                              .purple(getCommandName(commandName))
                              .red("\")")
                              .build());
    }

    const char *RequireChildNodeIds::what() const noexcept {
        return "[CHelper] dismiss child node ids";
    }

    std::string getCommandName(const std::vector<std::string> &commandName) {
        std::string result;
        bool isFirst = true;
        for (const auto &item: commandName) {
            if (isFirst) {
                isFirst = false;
            } else {
                result.push_back(',');
            }
            result.push_back('\"');
            result.append(item);
            result.push_back('\"');
        }
        return result;
    }

    CommandLoadFailed::CommandLoadFailed() {
        Profile::push(ColorStringBuilder()
                              .red("command load failed")
                              .build());
    }

    const char *CommandLoadFailed::what() const noexcept {
        return "[CHelper] CPack load failed";
    }

    NodeLoadFailed::NodeLoadFailed() {
        Profile::push(ColorStringBuilder()
                              .red("node load failed")
                              .build());
    }

    const char *NodeLoadFailed::what() const noexcept {
        return "[CHelper] node load failed";
    }

    WrongRange::WrongRange(size_t start, size_t end) {
        Profile::push(ColorStringBuilder()
                              .red("wrong range: ")
                              .purple("(" + std::to_string(start) + ", " + std::to_string(end) + ")")
                              .build());
    }

    const char *WrongRange::what() const noexcept {
        return "[CHelper] wrong range";
    }


} // CHelper::Exception