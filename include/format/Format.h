#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace FormatUtil {

    class ArgBase {
    public:
        virtual ~ArgBase() = default;

        virtual void Format(std::ostringstream &ss, const std::string &fmt) = 0;
    };

    template<class T>
    class Arg : public ArgBase {
    private:
        T m_arg;

    public:
        explicit Arg(T arg) : m_arg(arg) {}

        void Format(std::ostringstream &ss, const std::string &fmt) override {
            ss << m_arg;
        }
    };

    class ArgArray : public std::vector<ArgBase *> {
    public:
        ~ArgArray() {
            std::for_each(begin(), end(), [](ArgBase *p) { delete p; });
        }
    };

    static void formatItem(std::ostringstream &ss, const std::string &item, const ArgArray &args) {
        char *endPtr = nullptr;
        int index = strtol(&item[0], &endPtr, 10);
        if (index < 0 || index >= args.size()) {
            return;
        }

        if (*endPtr == ',') {
            int alignment = strtol(endPtr + 1, &endPtr, 10);
            if (alignment > 0) {
                ss << std::right << std::setw(alignment);
            } else if (alignment < 0) {
                ss << std::left << std::setw(-alignment);
            }
        }

        std::string fmt;
        if (*endPtr == ':') {
            fmt = endPtr + 1;
        }

        args[index]->Format(ss, fmt);
    }

    template<class T>
    static void transfer(ArgArray &argArray, T t) {
        argArray.push_back(new Arg<T>(t));
    }

    template<class T, typename... Args>
    static void transfer(ArgArray &argArray, T t, Args &&... args) {
        transfer(argArray, t);
        transfer(argArray, args...);
    }

    template<typename... Args>
    std::string format(const std::string &format, Args &&... args) {
        if (sizeof...(args) == 0) {
            return format;
        }

        ArgArray argArray;
        transfer(argArray, args...);
        size_t start = 0;
        size_t pos;
        std::ostringstream ss;
        while (true) {
            pos = format.find('{', start);
            if (pos == std::string::npos) {
                ss << format.substr(start);
                break;
            }

            ss << format.substr(start, pos - start);
            if (format[pos + 1] == '{') {
                ss << '{';
                start = pos + 2;
                continue;
            }

            start = pos + 1;
            pos = format.find('}', start);
            if (pos == std::string::npos) {
                ss << format.substr(start - 1);
                break;
            }

            formatItem(ss, format.substr(start, pos - start), argArray);
            start = pos + 1;
        }

        return ss.str();
    }
} // FormatUtil