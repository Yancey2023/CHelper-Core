//
// Created by Yancey on2024-5-10.
//

#pragma once

#ifndef CHELPER_BINARYUTIL_H
#define CHELPER_BINARYUTIL_H

#include "hedley.h"
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace CHelper {

#if CHelperOnlyReadBinary != true
    class BinaryWriter {
    public:
        bool isNeedConvert;
        std::ostream &ostream;

        BinaryWriter(bool isTargetSmallEndian,
                     std::ostream &ostream);

        void encode(bool t);

        void encode(int8_t t);

        void encode(uint8_t t);

        void encode(int16_t t);

        void encode(uint16_t t);

        void encode(int32_t t);

        void encode(uint32_t t);

        void encode(int64_t t);

        void encode(uint64_t t);

        void encode(float t);

        void encode(double t);

        void encodeSize(size_t t);

        void encode(const std::string &t);

        void encode(const std::u16string &t);

        template<class T>
        void encode(const std::shared_ptr<T> &t) {
            if (HEDLEY_UNLIKELY(t == nullptr)) {
                throw std::runtime_error("fail to encode std::shared_ptr because it is nullptr");
            }
            encode(*t);
        }

        template<class T>
        void encode(const std::vector<T> &t) {
            encodeSize(t.size());
            for (const T &item: t) {
                encode(item);
            }
        }

        template<class T>
        void encode(const std::optional<T> &t) {
            if (HEDLEY_LIKELY(t.has_value())) {
                encode(true);
                encode(t.value());
            } else {
                encode(false);
            }
        }

        template<class T, class S>
        void encode(const std::unordered_map<T, S> &t) {
            encodeSize(t.size());
            for (const auto &item: t) {
                encode(item.first);
                encode(item.second);
            }
        }

        template<class T, class S>
        void encode(const std::pair<T, S> &t) {
            encode(t.first);
            encode(t.second);
        }

        template<class T>
        void encode(const T &t) {
            to_binary(*this, t);
        }
    };
#endif

    class BinaryReader {
    public:
        bool isNeedConvert;
        std::istream &istream;

        BinaryReader(bool isTargetSmallEndian,
                     std::istream &istream);

        size_t readSize();

        void decode(bool &t);

        void decode(int8_t &t);

        void decode(uint8_t &t);

        void decode(int16_t &t);

        void decode(uint16_t &t);

        void decode(int32_t &t);

        void decode(uint32_t &t);

        void decode(int64_t &t);

        void decode(uint64_t &t);

        void decode(float &t);

        void decode(double &t);

        void decode(std::string &t);

        void decode(std::u16string &t);

        template<class T>
        void decode(std::vector<T> &t) {
            size_t size = readSize();
            t.reserve(size);
            for (int i = 0; i < size; ++i) {
                t.push_back(read<T>());
            }
        }

        template<class T>
        void decode(std::optional<T> &t) {
            if (read<bool>()) {
                t = std::make_optional<T>();
                decode(t.value());
            } else {
                t = std::nullopt;
            }
        }

        template<class T>
        void decode(std::shared_ptr<T> &t) {
            t = std::make_shared<T>();
            decode(*t);
        }

        template<class T, class S>
        void decode(std::unordered_map<T, S> &t) {
            size_t size = readSize();
            t.reserve(size);
            for (int i = 0; i < size; ++i) {
                auto key = read<T>();
                auto value = read<S>();
                t.emplace(std::move(key), std::move(value));
            }
        }

        template<class T, class S>
        void decode(std::pair<T, S> &t) {
            t = {read<T>(), read<S>()};
        }

        template<class T>
        void decode(T &value) {
            from_binary(*this, value);
        }

        template<class T>
        T read() {
            T t;
            decode(t);
            return t;
        }
    };

}// namespace CHelper

#endif//CHELPER_BINARYUTIL_H
