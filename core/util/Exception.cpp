//
// Created by Yancey666 on 2023/11/30.
//

#include <iostream>
#include "Exception.h"

namespace CHelper::Exception {

    const char *CHelperException::what() const noexcept {
        return "[CHelper] Unknown Exception";
    }

    std::string CHelperException::reason() const {
        return Color::BLUE + "[CHelper] " +
               Color::RED + "Unknown Exception" +
               Color::NORMAL;
    }

    CantCreateInstance::CantCreateInstance(std::string className)
            : className(std::move(className)) {}

    const char *CantCreateInstance::what() const noexcept {
        return "[CHelper] You create a instance which you shouldn't create";
    }

    std::string CantCreateInstance::reason() const {
        return Color::BLUE + "[CHelper] " +
               Color::RED + "You create a instance which you shouldn't create : " +
               Color::PURPLE + className +
               Color::NORMAL;
    }

    CPackLoadFailed::CPackLoadFailed(std::string state,
                                     std::string details)
            : state(std::move(state)),
              details(std::move(details)) {}

    const char *CPackLoadFailed::what() const noexcept {
        return "[CHelper] CPack load failed";
    }

    std::string CPackLoadFailed::reason() const {
        return Color::BLUE + "[CHelper] " +
               Color::RED + "CPack load failed" +
               Color::BLUE + "\n[CHelper] " +
               Color::RED + "state" +
               Color::NORMAL + " : " +
               Color::PURPLE + state + "\n" +
               Color::RED + details +
               Color::NORMAL;
    }

    UnknownIdType::UnknownIdType(std::string fileName,
                                 std::string idType)
            : fileName(std::move(fileName)),
              idType(std::move(idType)) {}

    const char *UnknownIdType::what() const noexcept {
        return "[CHelper] unknown id type";
    }

    std::string UnknownIdType::reason() const {
        return Color::BLUE + "[CHelper] " +
               Color::RED + "unknown id type" +
               Color::NORMAL + " -> " +
               Color::PURPLE + idType +
               Color::RED + " (in file \"" +
               Color::PURPLE + fileName +
               Color::RED + "\")" +
               Color::NORMAL;
    }

    UnknownNodeType::UnknownNodeType(std::string nodeType)
            : nodeType(std::move(nodeType)) {}

    const char *UnknownNodeType::what() const noexcept {
        return "[CHelper] unknown node type";
    }

    std::string UnknownNodeType::reason() const {
        return Color::BLUE + "[CHelper] " +
               Color::RED + "unknown node type" +
               Color::NORMAL + " -> " +
               Color::PURPLE + nodeType +
               Color::NORMAL;
    }

    UnknownNodeId::UnknownNodeId(std::vector<std::string> commandName,
                                 std::string nodeId)
            : commandName(std::move(commandName)),
              nodeId(std::move(nodeId)) {}

    const char *UnknownNodeId::what() const noexcept {
        return "[CHelper] unknown node id";
    }

    std::string UnknownNodeId::reason() const {
        return Color::BLUE + "[CHelper] " +
               Color::RED + "unknown node id" +
               Color::NORMAL + " -> " +
               Color::PURPLE + nodeId +
               Color::RED + " (in command " +
               Color::PURPLE + getCommandName(commandName) +
               Color::RED + ')' +
               Color::NORMAL;
    }

    RequireParentNodeId::RequireParentNodeId(std::vector<std::string> commandName)
            : commandName(std::move(commandName)) {}

    const char *RequireParentNodeId::what() const noexcept {
        return "[CHelper] dismiss parent node id";
    }

    std::string RequireParentNodeId::reason() const {
        return Color::BLUE + "[CHelper] " +
               Color::RED + "dismiss parent node id" +
               Color::RED + " (in command " +
               Color::PURPLE + getCommandName(commandName) +
               Color::RED + ')' +
               Color::NORMAL;
    }

    RequireChildNodeIds::RequireChildNodeIds(std::vector<std::string> commandName,
                                             std::string parentNodeId)
            : commandName(std::move(commandName)),
              parentNodeId(std::move(parentNodeId)) {}

    const char *RequireChildNodeIds::what() const noexcept {
        return "[CHelper] dismiss child node ids";
    }

    std::string RequireChildNodeIds::reason() const {
        return Color::BLUE + "[CHelper] " +
               Color::RED + "dismiss child node ids, the parent node" +
               Color::NORMAL + " -> " +
               Color::PURPLE + parentNodeId +
               Color::RED + " (in command " +
               Color::PURPLE + getCommandName(commandName) +
               Color::RED + ')' +
               Color::NORMAL;
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

    CommandLoadFailed::CommandLoadFailed(std::string state,
                                         std::string details)
            : state(std::move(state)),
              details(std::move(details)) {}

    const char *CommandLoadFailed::what() const noexcept {
        return "[CHelper] CPack load failed";
    }

    std::string CommandLoadFailed::reason() const {
        return Color::BLUE + "[CHelper] " +
               Color::RED + "command load failed" +
               Color::BLUE + "\n[CHelper] " +
               Color::RED + "state" +
               Color::NORMAL + " : " +
               Color::PURPLE + state + "\n" +
               Color::RED + details +
               Color::NORMAL;
    }

    NodeLoadFailed::NodeLoadFailed(std::string state,
                                   std::string details)
            : state(std::move(state)),
              details(std::move(details)) {}

    const char *NodeLoadFailed::what() const noexcept {
        return "[CHelper] node load failed";
    }

    std::string NodeLoadFailed::reason() const {
        return Color::BLUE + "[CHelper] " +
               Color::RED + "node load failed" +
               Color::BLUE + "\n[CHelper] " +
               Color::RED + "state" +
               Color::NORMAL + " : " +
               Color::PURPLE + state + "\n" +
               Color::RED + details +
               Color::NORMAL;
    }

    WrongRange::WrongRange(size_t start, size_t end)
            : start(start),
              end(end) {}

    const char *WrongRange::what() const noexcept {
        return "[CHelper] wrong range";
    }

    std::string WrongRange::reason() const {
        return Color::BLUE + "[CHelper] " +
               Color::RED + "wrong range: " +
               Color::PURPLE + "(" + std::to_string(start) + ", " + std::to_string(end) + ")" +
               Color::NORMAL;
    }


} // CHelper::Exception