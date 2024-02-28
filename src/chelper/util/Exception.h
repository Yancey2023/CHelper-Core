//
// Created by Yancey on 2023/11/30.
//

#ifndef CHELPER_EXCEPTION_H
#define CHELPER_EXCEPTION_H

#include <vector>
#include <string>

namespace CHelper::Exception {

    void printStackTrace(const std::exception &e);

    class UnknownIdType : public std::exception {
    public:
        UnknownIdType(const std::string &fileName, const std::string &idType);

        [[nodiscard]] const char *what() const noexcept override;
    };

    class UnknownNodeType : public std::exception {
    public:
        explicit UnknownNodeType(const std::string &nodeType);

        [[nodiscard]] const char *what() const noexcept override;
    };

    class UnknownNodeId : public std::exception {
    public:
        UnknownNodeId(const std::string& currentNodeId, const std::string &requiredNodeId);

        UnknownNodeId(const std::vector<std::string> &commandName, const std::string &requiredNodeId);

        [[nodiscard]] const char *what() const noexcept override;
    };

    class RequireParentNodeId : public std::exception {
    public:
        explicit RequireParentNodeId(const std::vector<std::string> &commandName);

        [[nodiscard]] const char *what() const noexcept override;

    };

    class RequireChildNodeIds : public std::exception {
    public:
        RequireChildNodeIds(const std::vector<std::string> &commandName, const std::string &parentNodeId);

        [[nodiscard]] const char *what() const noexcept override;
    };

    std::string getCommandName(const std::vector<std::string> &commandName);

    class NodeLoadFailed : public std::exception {
    public:
        NodeLoadFailed();

        [[nodiscard]] const char *what() const noexcept override;
    };

    // end < start || start < 0
    class WrongRange : public std::exception {
    public:
        WrongRange(size_t start, size_t end);

        [[nodiscard]] const char *what() const noexcept override;
    };

    // tokenReader指针栈的位置不正确
    class TokenReaderIndexError : public std::exception {
    public:
        TokenReaderIndexError();

        [[nodiscard]] const char *what() const noexcept override;
    };

} // CHelper::Exception

#endif //CHELPER_EXCEPTION_H
