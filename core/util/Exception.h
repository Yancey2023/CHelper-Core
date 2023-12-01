//
// Created by Yancey666 on 2023/11/30.
//

#ifndef CHELPER_EXCEPTION_H
#define CHELPER_EXCEPTION_H

#include "pch.h"

namespace CHelper::Exception {

    class CHelperException : public std::exception {
    public:
        CHelperException() = default;

        [[nodiscard]] const char *what() const noexcept override;

        [[nodiscard]] virtual std::string reason() const;
    };

    class CPackLoadFailed : public CHelperException {
    public:
        std::string state;
        std::string details;

        CPackLoadFailed(std::string state, std::string details);

        [[nodiscard]] const char *what() const noexcept override;

        [[nodiscard]] std::string reason() const override;
    };

    class UnknownIdType : public CHelperException {

    public:
        std::string fileName;
        std::string idType;

        UnknownIdType(std::string fileName, std::string idType);

        [[nodiscard]] const char *what() const noexcept override;

        [[nodiscard]] std::string reason() const override;
    };

    class UnknownNodeType : public CHelperException {
    public:
        std::string nodeType;

        explicit UnknownNodeType(std::string nodeType);

        [[nodiscard]] const char *what() const noexcept override;

        [[nodiscard]] std::string reason() const override;
    };

    class UnknownNodeId : public CHelperException {

    public:
        std::vector<std::string> commandName;
        std::string nodeId;

        UnknownNodeId(std::vector<std::string> commandName, std::string nodeId);

        [[nodiscard]] const char *what() const noexcept override;

        [[nodiscard]] std::string reason() const override;
    };

    class RequireParentNodeId : public CHelperException {
    public:
        std::vector<std::string> commandName;

        explicit RequireParentNodeId(std::vector<std::string> commandName);

        [[nodiscard]] const char *what() const noexcept override;

        [[nodiscard]] std::string reason() const override;

    };

    class RequireChildNodeIds : public CHelperException {
    public:
        std::vector<std::string> commandName;
        std::string parentNodeId;

        RequireChildNodeIds(std::vector<std::string> commandName, std::string parentNodeId);

        [[nodiscard]] const char *what() const noexcept override;

        [[nodiscard]] std::string reason() const override;
    };

    std::string getCommandName(const std::vector<std::string> &commandName);

    class CommandLoadFailed : public CHelperException {
    public:
        std::string state;
        std::string details;

        CommandLoadFailed(std::string state, std::string details);

        [[nodiscard]] const char *what() const noexcept override;

        [[nodiscard]] std::string reason() const override;
    };

    class NodeLoadFailed : public CHelperException {
    public:
        std::string state;
        std::string details;

        NodeLoadFailed(std::string state, std::string details);

        [[nodiscard]] const char *what() const noexcept override;

        [[nodiscard]] std::string reason() const override;
    };

} // CHelper::Exception

#endif //CHELPER_EXCEPTION_H
