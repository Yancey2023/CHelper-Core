//
// Created by Yancey on 2024-10-24.
//

#pragma once

#ifndef SERIALIZATION_H
#define SERIALIZATION_H

// clang-format off

#ifdef CXX_VERSION
    #undef CXX_VERSION
#endif
#ifdef _MSC_VER
    #define CXX_VERSION _MSVC_LANG
#else
    #define CXX_VERSION __cplusplus
#endif

#ifdef HAS_CXX14
    #undef HAS_CXX14
#endif
#if CXX_VERSION >= 201412L
    #define HAS_CXX14
#endif

#ifdef HAS_CXX17
    #undef HAS_CXX17
#endif
#if CXX_VERSION >= 201703L
    #define HAS_CXX17
#endif

#ifdef HAS_CXX20
    #undef HAS_CXX20
#endif
#if CXX_VERSION >= 202002L
    #define HAS_CXX20
#endif

// for: likely, unlikely
#ifdef likely
    #undef likely
#endif
#ifdef unlikely
    #undef unlikely
#endif
#if __cplusplus >= 202002L
    #define likely(expr    (expr) [[likely]]
    #define unlikely(expr) (expr) [[unlikely]]
#elif defined(__GNUC__) || defined(__clang__)
    #define likely(expr)   __builtin_expect(!!(expr), 1)
    #define unlikely(expr) __builtin_expect(!!(expr), 0)
#else
    #define likely(expr)   (!!(expr))
    #define unlikely(expr) (!!(expr))
#endif

// for: if constexpr (expr) {}
#ifdef CONSTEXPR17
    #undef CONSTEXPR17
#endif
#ifdef HAS_CXX17
    #define CONSTEXPR17 constexpr
#else
    #define CONSTEXPR17
#endif

// for: constexpr return_type method() {}
#ifdef CONSTEXPR20
    #undef CONSTEXPR20
#endif
#if HAS_CXX20
    #define CONSTEXPR20 constexpr
#else
    #define CONSTEXPR20 inline
#endif

// clang-format on

// STL
#include <cstdint>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#ifdef HAS_CXX17
#include <filesystem>
#include <optional>
#endif

// rapid json
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#ifndef SERIALIZATION_NO_FILESYSTEM
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#endif

namespace serialization {
    namespace details {

        template<class T, T Val>
        struct integral_constant {
            static constexpr T value = Val;

            // ReSharper disable once CppRedundantInlineSpecifier
            CONSTEXPR20 explicit operator T() const noexcept {
                return value;
            }

            // ReSharper disable once CppRedundantInlineSpecifier
            CONSTEXPR20 T operator()() const noexcept {
                return value;
            }
        };

        template<bool Val>
        using bool_constant = integral_constant<bool, Val>;

        using true_type = bool_constant<true>;
        using false_type = bool_constant<false>;

        template<bool Test, class T = void>
        struct enable_if {};

        template<class T>
        struct enable_if<true, T> {
            using type = T;
        };

        template<bool Test, class T = void>
        using enable_if_t = typename enable_if<Test, T>::type;

        template<class T, class S>
        struct is_same : false_type {};

        template<class T>
        struct is_same<T, T> : true_type {};

        template<class T>
        struct is_integer : false_type {};

        template<>
        struct is_integer<uint8_t> : true_type {
            using RapidJsonTypeHelperType = unsigned;
        };

        template<>
        struct is_integer<int8_t> : true_type {
            using RapidJsonTypeHelperType = int;
        };

        template<>
        struct is_integer<uint16_t> : true_type {
            using RapidJsonTypeHelperType = unsigned;
        };

        template<>
        struct is_integer<int16_t> : true_type {
            using RapidJsonTypeHelperType = int;
        };

        template<>
        struct is_integer<uint32_t> : true_type {
            using RapidJsonTypeHelperType = unsigned;
        };

        template<>
        struct is_integer<int32_t> : true_type {
            using RapidJsonTypeHelperType = int;
        };

        template<>
        struct is_integer<uint64_t> : true_type {
            using RapidJsonTypeHelperType = unsigned;
        };

        template<>
        struct is_integer<int64_t> : true_type {
            using RapidJsonTypeHelperType = int;
        };

        template<class T>
        struct is_number : is_integer<T> {};

        template<>
        struct is_number<float> : true_type {
            using RapidJsonTypeHelperType = float;
        };

        template<>
        struct is_number<double> : true_type {
            using RapidJsonTypeHelperType = double;
        };

        template<>
        struct is_number<long double> : true_type {
            using RapidJsonTypeHelperType = double;
        };

        template<class T>
        struct is_bool_or_number : is_number<T> {};

        template<>
        struct is_bool_or_number<bool> : true_type {
            using RapidJsonTypeHelperType = bool;
        };

        template<class T, class Ch>
        struct JsonKey {
            constexpr static bool enable = false;
        };
    }// namespace details

    template<class>
    struct Ch2EncodingType {
    };

    template<>
    struct Ch2EncodingType<char> {
        // ReSharper disable once CppRedundantTemplateArguments
        using EncodingType = rapidjson::UTF8<char>;
    };

#ifdef HAS_CXX17
    template<>
    struct Ch2EncodingType<char16_t> {
        using EncodingType = rapidjson::UTF16<char16_t>;
    };

    template<>
    struct Ch2EncodingType<char32_t> {
        // ReSharper disable once CppRedundantTemplateArguments
        using EncodingType = rapidjson::UTF32<char32_t>;
    };
#endif

    template<class SourceEncoding, class TargetEncoding>
    std::basic_string<typename TargetEncoding::Ch> xstring2xstring(const std::basic_string<typename SourceEncoding::Ch> &xstring) {
        if CONSTEXPR17 (details::is_same<typename SourceEncoding::Ch, typename TargetEncoding::Ch>::value) {
            return xstring;
        }
        rapidjson::GenericStringStream<SourceEncoding> source(xstring.data());
        rapidjson::GenericStringBuffer<TargetEncoding> target;
        while (source.Peek() != '\0') {
            if (!rapidjson::Transcoder<SourceEncoding, TargetEncoding>::Transcode(source, target)) {
#ifdef CHelperDebug
                throw std::runtime_error("fail to transform string");
#endif
                // ReSharper disable once CppDFAUnreachableCode
                return std::basic_string<typename TargetEncoding::Ch>();
            }
        }
        return std::basic_string<typename TargetEncoding::Ch>(target.GetString(), target.GetLength());
    }

    template<class SourceEncoding, class TargetEncoding>
    std::basic_string<typename TargetEncoding::Ch> xstring2xstring(const typename SourceEncoding::Ch *xstring) {
        if CONSTEXPR17 (details::is_same<typename SourceEncoding::Ch, typename TargetEncoding::Ch>::value) {
            return std::basic_string<typename SourceEncoding::Ch>(xstring);
        }
        rapidjson::GenericStringStream<SourceEncoding> source(xstring);
        rapidjson::GenericStringBuffer<TargetEncoding> target;
        while (source.Peek() != '\0') {
            if (!rapidjson::Transcoder<SourceEncoding, TargetEncoding>::Transcode(source, target)) {
#ifdef CHelperDebug
                throw std::runtime_error("fail to transform string");
#endif
                // ReSharper disable once CppDFAUnreachableCode
                return std::basic_string<typename TargetEncoding::Ch>();
            }
        }
        return std::basic_string<typename TargetEncoding::Ch>(target.GetString(), target.GetLength());
    }

    template<class SourceEncoding, class TargetEncoding>
    std::basic_string<typename TargetEncoding::Ch> jsonString2xstring(const rapidjson::GenericValue<SourceEncoding> &jsonString) {
        if CONSTEXPR17 (details::is_same<typename SourceEncoding::Ch, typename TargetEncoding::Ch>::value) {
            return std::basic_string<typename TargetEncoding::Ch>(
                    // ReSharper disable once CppCStyleCast
                    (typename TargetEncoding::Ch *) jsonString.GetString(),
                    jsonString.GetStringLength());
        }
        rapidjson::GenericStringStream<SourceEncoding> source(jsonString.GetString());
        rapidjson::GenericStringBuffer<TargetEncoding> target;
        while (source.Peek() != '\0') {
            if (!rapidjson::Transcoder<SourceEncoding, TargetEncoding>::Transcode(source, target)) {
#ifdef CHelperDebug
                throw std::runtime_error("fail to transform string");
#endif
                // ReSharper disable once CppDFAUnreachableCode
                return std::basic_string<typename TargetEncoding::Ch>();
            }
        }
        return std::basic_string<typename TargetEncoding::Ch>(target.GetString(), target.GetLength());
    }

    template<class SourceEncoding, class JsonValueType>
    rapidjson::GenericValue<typename JsonValueType::EncodingType> xstring2jsonString(typename JsonValueType::AllocatorType &allocator,
                                                                                     const std::basic_string<typename SourceEncoding::Ch> &xstring) {
        if CONSTEXPR17 (details::is_same<typename SourceEncoding::Ch, typename JsonValueType::Ch>::value) {
            return rapidjson::GenericValue<typename JsonValueType::EncodingType>(xstring.data(), xstring.length(), allocator);
        }
        rapidjson::GenericStringStream<SourceEncoding> source(xstring.c_str());
        rapidjson::GenericStringBuffer<typename JsonValueType::EncodingType> target;
        while (source.Peek() != '\0') {
            if (!rapidjson::Transcoder<SourceEncoding, typename JsonValueType::EncodingType>::Transcode(source, target)) {
                assert(false);
                // ReSharper disable once CppDFAUnreachableCode
                return rapidjson::GenericValue<typename JsonValueType::EncodingType>();
            }
        }
        return typename JsonValueType::ValueType(target.GetString(), target.GetLength(), allocator);
    }

    inline const char *getJsonTypeStr(const rapidjson::Type type) {
        switch (type) {
            case rapidjson::Type::kNullType:
                return "null";
            case rapidjson::Type::kFalseType:
            case rapidjson::Type::kTrueType:
                return "boolean";
            case rapidjson::Type::kObjectType:
                return "object";
            case rapidjson::Type::kArrayType:
                return "array";
            case rapidjson::Type::kStringType:
                return "string";
            case rapidjson::Type::kNumberType:
                return "number";
            default:
                return "unknown";
        }
    }

    namespace exceptions {

        class JsonSerializationKeyException final : public std::runtime_error {
        public:
            template<class Ch>
            explicit JsonSerializationKeyException(const std::basic_string<Ch> &key)
                : std::runtime_error("fail to find json value by key: " +
                                     xstring2xstring<typename Ch2EncodingType<Ch>::EncodingType, Ch2EncodingType<char>::EncodingType>(key)) {}

            template<class Ch>
            explicit JsonSerializationKeyException(const Ch *key)
                : std::runtime_error("fail to find json value by key: " +
                                     xstring2xstring<typename Ch2EncodingType<Ch>::EncodingType, Ch2EncodingType<char>::EncodingType>(key)) {}
        };

        class JsonSerializationTypeException final : public std::runtime_error {
        public:
            explicit JsonSerializationTypeException(const char *expect_type, const char *current_type)
                : std::runtime_error(std::string("json type error, expect type \"")
                                             .append(expect_type)
                                             .append("\", but find \"")
                                             .append(current_type)
                                             .append("\"")) {}
        };

#ifndef SERIALIZATION_NO_FILESYSTEM
        class OpenFileException final : public std::runtime_error {
        public:
            explicit OpenFileException(const char *path, const char *mode, const errno_t err)
                : std::runtime_error(std::string("fail to open file with mode \"")
                                             .append(mode)
                                             .append("\", error code is ")
                                             .append(std::to_string(err))
                                             .append(": ")
                                             .append(path)) {}
        };
#endif

    }// namespace exceptions

    template<class JsonValueType>
    const typename JsonValueType::ValueType &find_member_or_throw(
            const JsonValueType &jsonValue,
            const typename JsonValueType::Ch *key) {
        assert(jsonValue.IsObject());
        const typename JsonValueType::ConstMemberIterator it = jsonValue.FindMember(key);
        if (unlikely(it == jsonValue.MemberEnd())) {
            throw exceptions::JsonSerializationKeyException(key);
        }
        return it->value;
    }

    template<class JsonValueType>
    const typename JsonValueType::ValueType &check_array_or_throw(const JsonValueType &jsonValue) {
        if (unlikely(!jsonValue.IsArray())) {
            throw exceptions::JsonSerializationTypeException("array", getJsonTypeStr(jsonValue.GetType()));
        }
        return jsonValue;
    }

    template<class JsonValueType>
    typename JsonValueType::ConstArray find_array_member_or_throw(
            const JsonValueType &jsonValue,
            const typename JsonValueType::Ch *key) {
        return check_array_or_throw(find_member_or_throw(jsonValue, key)).GetArray();
    }

    template<class T, class = void>
    struct Codec {

        using Type = T;

        constexpr static bool enable = false;
    };

    template<typename T>
    struct Codec<const T> : Codec<T> {
    };

    template<typename T>
    struct Codec<T &> : Codec<T> {
    };

    template<typename T>
    struct Codec<const T &> : Codec<T> {
    };

    template<class T>
    struct BaseCodec {

        using Type = T;

        template<class JsonValueType>
        static void to_json_member(typename JsonValueType::AllocatorType &allocator,
                                   JsonValueType &jsonValue,
                                   const typename JsonValueType::Ch *key,
                                   const Type &t) {
            assert(jsonValue.IsObject());
            typename JsonValueType::ValueType value;
            Codec<Type>::template to_json(allocator, value, t);
            assert(!value.IsNull());
            jsonValue.AddMember(typename JsonValueType::ValueType(key, allocator), std::move(value), allocator);
        }

        template<class JsonValueType>
        static void from_json_member(const JsonValueType &jsonValue,
                                     const typename JsonValueType::Ch *key,
                                     Type &t) {
            static_assert(Codec<Type>::enable, "fail to find impl of Codec");
            assert(jsonValue.IsObject());
            Codec<Type>::template from_json(find_member_or_throw(jsonValue, key), t);
        }
    };

    template<typename ChildType, typename ParentType>
    struct WrappedCodec {

        using Type = ChildType;

        constexpr static bool enable = false;

        static_assert(Codec<ParentType>::enable, "fail to find impl of Codec");

        template<class JsonValueType>
        static void to_json(typename JsonValueType::AllocatorType &allocator,
                            JsonValueType &jsonValue,
                            const Type &t) {
            Codec<ParentType>::template to_json(allocator, jsonValue, static_cast<const ParentType &>(t));
        }

        template<class JsonValueType>
        static void from_json(const JsonValueType &jsonValue,
                              Type &t) {
            Codec<ParentType>::template from_json(jsonValue, static_cast<ParentType &>(t));
        }

        template<bool isNeedConvert>
        static void to_binary(std::ostream &ostream,
                              const Type &t) {
            Codec<ParentType>::template to_binary<isNeedConvert>(ostream, static_cast<const ParentType &>(t));
        }

        template<bool isNeedConvert>
        static void from_binary(std::istream &istream,
                                Type &t) {
            Codec<ParentType>::template from_binary<isNeedConvert>(istream, static_cast<ParentType &>(t));
        }
    };

    template<typename ChildType>
    struct WrappedCodec<ChildType, void> {

        using Type = ChildType;

        constexpr static bool enable = false;

        template<class JsonValueType>
        static void to_json(typename JsonValueType::AllocatorType &allocator,
                            JsonValueType &jsonValue,
                            const Type &t) {
            jsonValue.SetObject();
        }

        template<class JsonValueType>
        static void from_json(const JsonValueType &jsonValue,
                              Type &t) {
            if (unlikely(!jsonValue.IsObject())) {
                throw exceptions::JsonSerializationTypeException("object", getJsonTypeStr(jsonValue.GetType()));
            }
        }

        template<bool isNeedConvert>
        static void to_binary(std::ostream &ostream,
                              const Type &t) {
        }

        template<bool isNeedConvert>
        static void from_binary(std::istream &istream,
                                Type &t) {
        }
    };

    template<class T>
    struct BaseUniquePtrCodec : BaseCodec<std::unique_ptr<T>> {

        using Type = std::unique_ptr<T>;

        static_assert(Codec<T>::enable, "fail to find impl of Codec");

        template<class JsonValueType>
        static void to_json(typename JsonValueType::AllocatorType &allocator,
                            JsonValueType &jsonValue,
                            const Type &t) {
            Codec<T>::template to_json<JsonValueType>(allocator, jsonValue, *t);
        }

        template<class JsonValueType>
        static void from_json(const JsonValueType &jsonValue,
                              Type &t) {
            t = std::make_unique<T>();
            Codec<T>::template from_json<JsonValueType>(jsonValue, *t);
        }

        template<bool isNeedConvert>
        static void to_binary(std::ostream &ostream,
                              const Type &t) {
            Codec<T>::template to_binary<isNeedConvert>(ostream, *t);
        }

        template<bool isNeedConvert>
        static void from_binary(std::istream &istream,
                                Type &t) {
            t = std::make_unique<T>();
            Codec<T>::template from_binary<isNeedConvert>(istream, *t);
        }
    };

    template<class T, class S>
    struct BaseEnumCodec : BaseCodec<T> {

        using Type = T;

        static_assert(std::is_enum_v<T>, "must be an enum!");
        static_assert(details::is_integer<S>::value, "must be an integer!");

        template<class JsonValueType>
        static void to_json(typename JsonValueType::AllocatorType &allocator,
                            JsonValueType &jsonValue,
                            const Type &t) {
            Codec<S>::template to_json(allocator, jsonValue, reinterpret_cast<const S &>(t));
        }

        template<class JsonValueType>
        static void from_json(const JsonValueType &jsonValue,
                              Type &t) {
            Codec<S>::template from_json(jsonValue, reinterpret_cast<S &>(t));
        }

        template<bool isNeedConvert>
        static void to_binary(std::ostream &ostream,
                              const Type &t) {
            Codec<S>::template to_binary<isNeedConvert>(ostream, reinterpret_cast<const S &>(t));
        }

        template<bool isNeedConvert>
        static void from_binary(std::istream &istream,
                                Type &t) {
            Codec<S>::template from_binary<isNeedConvert>(istream, reinterpret_cast<S &>(t));
        }
    };

    template<class T>
    struct Codec<T, details::enable_if_t<details::is_bool_or_number<T>::value>> : BaseCodec<T> {

        using Type = T;
        using RapidJsonTypeHelperType = typename details::is_bool_or_number<T>::RapidJsonTypeHelperType;

        constexpr static bool enable = true;

        template<class JsonValueType>
        static void to_json(typename JsonValueType::AllocatorType &allocator,
                            JsonValueType &jsonValue,
                            const Type &t) {
            jsonValue.template Set<RapidJsonTypeHelperType>(static_cast<RapidJsonTypeHelperType>(t), allocator);
        }

        template<class JsonValueType>
        static void from_json(const JsonValueType &jsonValue,
                              Type &t) {
            RapidJsonTypeHelperType a = 1;
            if (unlikely(!(static_cast<bool>(details::is_same<Type, float>::value)
                                   ? jsonValue.IsLosslessFloat()
                           : static_cast<bool>(details::is_same<Type, double>::value)
                                   ? jsonValue.IsLosslessDouble()
                                   : jsonValue.template Is<RapidJsonTypeHelperType>()))) {
                if CONSTEXPR17 (details::is_same<Type, bool>::value) {
                    throw exceptions::JsonSerializationTypeException("boolean", getJsonTypeStr(jsonValue.GetType()));
                } else {
                    throw exceptions::JsonSerializationTypeException("number", getJsonTypeStr(jsonValue.GetType()));
                }
            }
            t = static_cast<Type>(jsonValue.template Get<RapidJsonTypeHelperType>());
        }

        template<bool isNeedConvert>
        static void to_binary(std::ostream &ostream,
                              const Type &t) {
            if CONSTEXPR17 (isNeedConvert && sizeof(t) > 1) {
                union {
                    Type value;
                    uint8_t data[sizeof(t)];
                    // ReSharper disable once CppTooWideScope
                } source = {t};
                uint8_t target[sizeof(t)];
                for (int i = 0; i < sizeof(t); ++i) {
                    target[i] = source.data[sizeof(t) - 1 - i];
                }
                ostream.write(reinterpret_cast<const char *>(target), sizeof(t));
            } else {
                ostream.write(reinterpret_cast<const char *>(&t), sizeof(t));
            }
        }

        template<bool isNeedConvert>
        static void from_binary(std::istream &istream,
                                Type &t) {
            if CONSTEXPR17 (isNeedConvert && sizeof(t) > 1) {
                uint8_t source[sizeof(t)];
                istream.read(reinterpret_cast<char *>(source), sizeof(t));
                union {// NOLINT(*-pro-type-member-init)
                    Type value;
                    uint8_t data[sizeof(t)];
                } target;
                for (int i = 0; i < sizeof(t); ++i) {
                    target.data[i] = source[sizeof(t) - 1 - i];
                }
                t = target.value;
            } else {
                istream.read(reinterpret_cast<char *>(&t), sizeof(t));
            }
        }
    };

    template<class Ch>
    struct Codec<std::basic_string<Ch>> : BaseCodec<std::basic_string<Ch>> {
        using Type = std::basic_string<Ch>;

        constexpr static bool enable = true;

        template<class JsonValueType>
        static void to_json(typename JsonValueType::AllocatorType &allocator,
                            JsonValueType &jsonValue,
                            const Type &t) {
            jsonValue = xstring2jsonString<typename Ch2EncodingType<Ch>::EncodingType, JsonValueType>(allocator, t);
        }

        template<class JsonValueType>
        static void from_json(const JsonValueType &jsonValue,
                              Type &t) {
            if (unlikely(!jsonValue.IsString())) {
                throw exceptions::JsonSerializationTypeException("string", getJsonTypeStr(jsonValue.GetType()));
            }
            t = jsonString2xstring<typename JsonValueType::EncodingType, typename Ch2EncodingType<Ch>::EncodingType>(jsonValue);
        }

        template<bool isNeedConvert>
        static void to_binary(std::ostream &ostream,
                              const Type &t) {
            if CONSTEXPR17 (details::is_same<Ch, char>::value) {
                Codec<uint32_t>::template to_binary<isNeedConvert>(ostream, t.length());
                ostream.write(reinterpret_cast<const char *>(t.data()), t.length());
            } else {
                std::string str = xstring2xstring<typename Ch2EncodingType<Ch>::EncodingType, Ch2EncodingType<char>::EncodingType>(t);
                Codec<uint32_t>::template to_binary<isNeedConvert>(ostream, str.length());
                ostream.write(str.data(), static_cast<std::streamsize>(str.length()));
            }
        }

        template<bool isNeedConvert>
        static void from_binary(std::istream &istream,
                                Type &t) {
            uint32_t length;
            Codec<uint32_t>::template from_binary<isNeedConvert>(istream, length);
            if CONSTEXPR17 (details::is_same<Ch, char>::value) {
                t.resize(length);
                istream.read(reinterpret_cast<char *>(t.data()), length);
            } else {
                std::string str;
                str.resize(length);
                istream.read(str.data(), length);
                t = xstring2xstring<Ch2EncodingType<char>::EncodingType, typename Ch2EncodingType<Ch>::EncodingType>(str);
            }
        }
    };

    template<class T>
    struct Codec<std::vector<T>> : BaseCodec<std::vector<T>> {

        static_assert(Codec<T>::enable, "fail to find impl of Codec");

        using Type = std::vector<T>;

        constexpr static bool enable = true;

        template<class JsonValueType>
        static void to_json(typename JsonValueType::AllocatorType &allocator,
                            JsonValueType &jsonValue,
                            const Type &t) {
            jsonValue.SetArray();
            jsonValue.GetArray().Reserve(t.size(), allocator);
            for (const T &item: t) {
                JsonValueType itemJsonValue;
                Codec<T>::template to_json(allocator, itemJsonValue, item);
                jsonValue.PushBack(std::move(itemJsonValue), allocator);
            }
        }

        template<class JsonValueType>
        static void from_json(const JsonValueType &jsonValue,
                              Type &t) {
            assert(t.empty());
            if (unlikely(!jsonValue.IsArray())) {
                throw exceptions::JsonSerializationTypeException("array", getJsonTypeStr(jsonValue.GetType()));
            }
            t.reserve(jsonValue.Size());
            for (const auto &item: jsonValue.GetArray()) {
                T item1;
                Codec<T>::template from_json(item, item1);
                t.push_back(std::move(item1));
            }
        }

        template<bool isNeedConvert>
        static void to_binary(std::ostream &ostream,
                              const Type &t) {
            Codec<uint32_t>::template to_binary<isNeedConvert>(ostream, static_cast<uint32_t>(t.size()));
            if CONSTEXPR17 ((!isNeedConvert || sizeof(T) <= 1) && details::is_number<T>::value) {
                ostream.write(reinterpret_cast<const char *>(t.data()), t.size() * sizeof(T));
            } else {
                for (const T &item: t) {
                    Codec<T>::template to_binary<isNeedConvert>(ostream, item);
                }
            }
        }

        template<bool isNeedConvert>
        static void from_binary(std::istream &istream,
                                Type &t) {
            assert(t.empty());
            uint32_t length;
            Codec<uint32_t>::template from_binary<isNeedConvert>(istream, length);
            if CONSTEXPR17 ((!isNeedConvert || sizeof(T) <= 1) && details::is_number<T>::value) {
                t.resize(length);
                istream.read(reinterpret_cast<char *>(t.data()), static_cast<std::streamsize>(length * sizeof(T)));
            } else {
                t.reserve(length);
                for (uint32_t i = 0; i < length; ++i) {
                    T item;
                    Codec<T>::template from_binary<isNeedConvert>(istream, item);
                    t.push_back(std::move(item));
                }
            }
        }
    };

#ifdef HAS_CXX17
    template<class T>
    struct Codec<std::optional<T>> {

        static_assert(Codec<T>::enable, "fail to find impl of Codec");

        using Type = std::optional<T>;

        constexpr static bool enable = true;

        template<class JsonValueType>
        static void to_json(typename JsonValueType::AllocatorType &allocator,
                            JsonValueType &jsonValue,
                            const Type &t) {
            assert(t.has_value());
            Codec<T>::template to_json(allocator, jsonValue, t.value());
        }

        template<class JsonValueType>
        static void from_json(const JsonValueType &jsonValue,
                              Type &t) {
            t = std::make_optional<T>();
            Codec<T>::template from_json(jsonValue, t.value());
        }

        template<class JsonValueType>
        static void to_json_member(typename JsonValueType::AllocatorType &allocator,
                                   JsonValueType &jsonValue,
                                   const typename JsonValueType::Ch *key,
                                   const Type &t) {
            assert(jsonValue.IsObject());
            if (likely(t.has_value())) {
                typename JsonValueType::ValueType value;
                Codec<Type>::template to_json(allocator, value, t);
                assert(!value.IsNull());
                jsonValue.AddMember(typename JsonValueType::ValueType(key, allocator), std::move(value), allocator);
            }
        }

        template<class JsonValueType>
        static void from_json_member(const JsonValueType &jsonValue,
                                     const std::basic_string<typename JsonValueType::Ch> &key,
                                     Type &t) {
            assert(jsonValue.IsObject());
            const typename JsonValueType::ConstMemberIterator &it = jsonValue.FindMember(key.c_str());
            if (unlikely(it == jsonValue.MemberEnd())) {
                t = std::nullopt;
                return;
            }
            Codec<Type>::template from_json(it->value, t);
        }

        template<bool isNeedConvert>
        static void to_binary(std::ostream &ostream,
                              const Type &t) {
            Codec<bool>::template to_binary<isNeedConvert>(ostream, t.has_value());
            if (likely(t.has_value())) {
                Codec<T>::template to_binary<isNeedConvert>(ostream, t.value());
            }
        }

        template<bool isNeedConvert>
        static void from_binary(std::istream &istream,
                                Type &t) {
            bool has_value;
            Codec<bool>::template from_binary<isNeedConvert>(istream, has_value);
            if (likely(has_value)) {
                t = std::make_optional<T>();
                Codec<T>::template from_binary<isNeedConvert>(istream, t.value());
            } else {
                t = std::nullopt;
            }
        }
    };
#endif

    template<class T>
    struct Codec<std::shared_ptr<T>> : BaseCodec<std::shared_ptr<T>> {

        static_assert(Codec<T>::enable, "fail to find impl of Codec");

        using Type = std::shared_ptr<T>;

        constexpr static bool enable = true;

        template<class JsonValueType>
        static void to_json(typename JsonValueType::AllocatorType &allocator,
                            JsonValueType &jsonValue,
                            const Type &t) {
            assert(t != nullptr);
            Codec<T>::template to_json(allocator, jsonValue, *t);
        }

        template<class JsonValueType>
        static void from_json(const JsonValueType &jsonValue,
                              Type &t) {
            t = std::make_shared<T>();
            Codec<T>::template from_json(jsonValue, *t);
        }

        template<bool isNeedConvert>
        static void to_binary(std::ostream &ostream,
                              const Type &t) {
            assert(t != nullptr);
            Codec<T>::template to_binary<isNeedConvert>(ostream, *t);
        }

        template<bool isNeedConvert>
        static void from_binary(std::istream &istream,
                                Type &t) {
            t = std::make_shared<T>();
            Codec<T>::template from_binary<isNeedConvert>(istream, *t);
        }
    };

    template<class Ch, class T>
    struct Codec<std::unordered_map<std::basic_string<Ch>, T>> : BaseCodec<std::unordered_map<std::basic_string<Ch>, T>> {

        static_assert(Codec<T>::enable, "fail to find impl of Codec");

        using Type = std::unordered_map<std::basic_string<Ch>, T>;

        constexpr static bool enable = true;

        template<class JsonValueType>
        static void to_json(typename JsonValueType::AllocatorType &allocator,
                            JsonValueType &jsonValue,
                            const Type &t) {
            jsonValue.SetObject();
            jsonValue.MemberReserve(t.size());
            for (const auto &item: t) {
                typename JsonValueType::ValueType key;
                Codec<std::basic_string<Ch>>::template to_json(allocator, key, item.first);
                typename JsonValueType::ValueType value;
                Codec<T>::template to_json(allocator, value, item.second);
                jsonValue.AddMember(std::move(key), std::move(value), allocator);
            }
        }

        template<class JsonValueType>
        static void from_json(const JsonValueType &jsonValue,
                              Type &t) {
            assert(t.empty());
            if (unlikely(!jsonValue.IsObject())) {
                throw exceptions::JsonSerializationTypeException("object", getJsonTypeStr(jsonValue.GetType()));
            }
            t.reserve(jsonValue.MemberCount());
            auto begin = jsonValue.MemberBegin();
            auto end = jsonValue.MemberEnd();
            for (typename JsonValueType::MemberIterator it = begin; it < end; ++it) {
                std::basic_string<Ch> key;
                Codec<std::basic_string<Ch>>::template from_json(it->name, key);
                T value;
                Codec<T>::template from_json(it->value, value);
                t.emplace(std::move(key), std::move(value));
            }
        }

        template<bool isNeedConvert>
        static void to_binary(std::ostream &ostream,
                              const Type &t) {
            Codec<uint32_t>::template to_binary<isNeedConvert>(ostream, t.size());
            for (const auto &item: t) {
                Codec<std::basic_string<Ch>>::template to_binary<isNeedConvert>(ostream, item.first);
                Codec<T>::template to_binary<isNeedConvert>(ostream, item.second);
            }
        }

        template<bool isNeedConvert>
        static void from_binary(std::istream &istream,
                                Type &t) {
            assert(t.empty());
            uint32_t length;
            Codec<uint32_t>::template from_binary<isNeedConvert>(istream, length);
            t.reserve(length);
            for (uint32_t i = 0; i < length; ++i) {
                std::basic_string<Ch> key;
                Codec<std::basic_string<Ch>>::template from_binary<isNeedConvert>(istream, key);
                T value;
                Codec<T>::template from_binary<isNeedConvert>(istream, value);
                t.emplace(std::move(key), std::move(value));
            }
        }
    };

    template<class T, class S>
    struct Codec<std::unordered_map<T, S>> : BaseCodec<std::unordered_map<T, S>> {

        static_assert(Codec<T>::enable, "fail to find impl of Codec");
        static_assert(Codec<S>::enable, "fail to find impl of Codec");

        using Type = std::unordered_map<T, S>;

        constexpr static bool enable = true;

        template<bool isNeedConvert>
        static void to_binary(std::ostream &ostream,
                              const Type &t) {
            Codec<uint32_t>::template to_binary<isNeedConvert>(ostream, t.size());
            for (const auto &item: t) {
                Codec<T>::template to_binary<isNeedConvert>(ostream, item.first);
                Codec<S>::template to_binary<isNeedConvert>(ostream, item.second);
            }
        }

        template<bool isNeedConvert>
        static void from_binary(std::istream &istream,
                                Type &t) {
            assert(t.empty());
            uint32_t length;
            Codec<uint32_t>::template from_binary<isNeedConvert>(istream, length);
            t.reserve(length);
            for (uint32_t i = 0; i < length; ++i) {
                T key;
                Codec<T>::template from_binary<isNeedConvert>(istream, key);
                S value;
                Codec<S>::template from_binary<isNeedConvert>(istream, value);
                t.emplace(std::move(key), std::move(value));
            }
        }
    };

    template<class T, class S>
    struct Codec<std::pair<T, S>> : BaseCodec<std::pair<T, S>> {

        static_assert(Codec<T>::enable, "fail to find impl of Codec");
        static_assert(Codec<S>::enable, "fail to find impl of Codec");

        using Type = std::pair<T, S>;

        constexpr static bool enable = true;

        template<bool isNeedConvert>
        static void to_binary(std::ostream &ostream,
                              const Type &t) {
            Codec<T>::template to_binary<isNeedConvert>(ostream, t.first);
            Codec<S>::template to_binary<isNeedConvert>(ostream, t.second);
        }

        template<bool isNeedConvert>
        static void from_binary(std::istream &istream,
                                Type &t) {
            Codec<T>::template from_binary<isNeedConvert>(istream, t.first);
            Codec<S>::template from_binary<isNeedConvert>(istream, t.second);
        }
    };

    template<bool isTargetSmallEndian>
    bool getIsNeedConvert() {
        union {
            int16_t int16 = 0x0102;
            int8_t int8;
        } data;
        const bool isUsingSmallEndian = data.int8 == 0x02;
        return isUsingSmallEndian != isTargetSmallEndian;
    }

    template<bool isTargetSmallEndian, class T>
    void from_binary(std::istream &istream, T &t) {
        if (getIsNeedConvert<isTargetSmallEndian>()) {
            Codec<T>::template from_binary<true>(istream, t);
        } else {
            Codec<T>::template from_binary<false>(istream, t);
        }
    }

    template<bool isTargetSmallEndian, class T>
    void to_binary(std::ostream &ostream, const T &t) {
        if (getIsNeedConvert<isTargetSmallEndian>()) {
            Codec<T>::template to_binary<true>(ostream, t);
        } else {
            Codec<T>::template to_binary<false>(ostream, t);
        }
    }

#ifndef SERIALIZATION_NO_FILESYSTEM
    template<class EncodingType = rapidjson::UTF8<>>
    rapidjson::GenericDocument<EncodingType> get_json_from_file(const char *path) {
        FILE *fp;
#ifdef _WIN32
        auto mode = "rb";
#else
        auto mode = "r";
#endif
        const errno_t err = fopen_s(&fp, path, mode);
        if (unlikely(err != 0)) {
            fclose(fp);
            throw exceptions::OpenFileException(path, mode, err);
        }
        char readBuffer[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
        rapidjson::GenericDocument<EncodingType> document;
        document.ParseStream(is);
        fclose(fp);
        return std::move(document);
    }

    template<class EncodingType = rapidjson::UTF8<>>
    void write_json_to_file(const char *path, const rapidjson::GenericValue<EncodingType> &j) {
        FILE *fp;
#ifdef _WIN32
        auto mode = "wb";
#else
        auto mode = "w";
#endif
        errno_t err = fopen_s(&fp, path, mode);
        if (unlikely(err != 0)) {
            fclose(fp);
            throw exceptions::OpenFileException(path, mode, err);
        }
        char writeBuffer[65536];
        rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
        rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
        j.Accept(writer);
        fclose(fp);
    }

#ifdef HAS_CXX17
    template<class EncodingType = rapidjson::UTF8<>>
    rapidjson::GenericDocument<EncodingType> get_json_from_file(const std::filesystem::path &path) {
        return get_json_from_file(path.string().c_str());
    }

    template<class EncodingType = rapidjson::UTF8<>>
    void write_json_to_file(const std::filesystem::path &path, const rapidjson::GenericValue<EncodingType> &j) {
        write_json_to_file(path.string().c_str(), j);
    }
#endif
#endif

}// namespace serialization

#if false
#undef CXX_VERSION
#undef HAS_CXX14
#undef HAS_CXX17
#undef HAS_CXX20
#undef likely
#undef unlikely
#undef CONSTEXPR17
#undef CONSTEXPR20
#endif

#define CODEC_EXPAND(x) x
#define CODEC_GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, NAME, ...) NAME
#define CODEC_PASTE(...) CODEC_EXPAND(CODEC_GET_MACRO(__VA_ARGS__,   \
                                                      CODEC_PASTE64, \
                                                      CODEC_PASTE63, \
                                                      CODEC_PASTE62, \
                                                      CODEC_PASTE61, \
                                                      CODEC_PASTE60, \
                                                      CODEC_PASTE59, \
                                                      CODEC_PASTE58, \
                                                      CODEC_PASTE57, \
                                                      CODEC_PASTE56, \
                                                      CODEC_PASTE55, \
                                                      CODEC_PASTE54, \
                                                      CODEC_PASTE53, \
                                                      CODEC_PASTE52, \
                                                      CODEC_PASTE51, \
                                                      CODEC_PASTE50, \
                                                      CODEC_PASTE49, \
                                                      CODEC_PASTE48, \
                                                      CODEC_PASTE47, \
                                                      CODEC_PASTE46, \
                                                      CODEC_PASTE45, \
                                                      CODEC_PASTE44, \
                                                      CODEC_PASTE43, \
                                                      CODEC_PASTE42, \
                                                      CODEC_PASTE41, \
                                                      CODEC_PASTE40, \
                                                      CODEC_PASTE39, \
                                                      CODEC_PASTE38, \
                                                      CODEC_PASTE37, \
                                                      CODEC_PASTE36, \
                                                      CODEC_PASTE35, \
                                                      CODEC_PASTE34, \
                                                      CODEC_PASTE33, \
                                                      CODEC_PASTE32, \
                                                      CODEC_PASTE31, \
                                                      CODEC_PASTE30, \
                                                      CODEC_PASTE29, \
                                                      CODEC_PASTE28, \
                                                      CODEC_PASTE27, \
                                                      CODEC_PASTE26, \
                                                      CODEC_PASTE25, \
                                                      CODEC_PASTE24, \
                                                      CODEC_PASTE23, \
                                                      CODEC_PASTE22, \
                                                      CODEC_PASTE21, \
                                                      CODEC_PASTE20, \
                                                      CODEC_PASTE19, \
                                                      CODEC_PASTE18, \
                                                      CODEC_PASTE17, \
                                                      CODEC_PASTE16, \
                                                      CODEC_PASTE15, \
                                                      CODEC_PASTE14, \
                                                      CODEC_PASTE13, \
                                                      CODEC_PASTE12, \
                                                      CODEC_PASTE11, \
                                                      CODEC_PASTE10, \
                                                      CODEC_PASTE9,  \
                                                      CODEC_PASTE8,  \
                                                      CODEC_PASTE7,  \
                                                      CODEC_PASTE6,  \
                                                      CODEC_PASTE5,  \
                                                      CODEC_PASTE4,  \
                                                      CODEC_PASTE3,  \
                                                      CODEC_PASTE2,  \
                                                      CODEC_PASTE1)(__VA_ARGS__))
#define CODEC_PASTE2(func, v1) func(v1)
#define CODEC_PASTE3(func, v1, v2) CODEC_PASTE2(func, v1) CODEC_PASTE2(func, v2)
#define CODEC_PASTE4(func, v1, v2, v3) CODEC_PASTE2(func, v1) CODEC_PASTE3(func, v2, v3)
#define CODEC_PASTE5(func, v1, v2, v3, v4) CODEC_PASTE2(func, v1) CODEC_PASTE4(func, v2, v3, v4)
#define CODEC_PASTE6(func, v1, v2, v3, v4, v5) CODEC_PASTE2(func, v1) CODEC_PASTE5(func, v2, v3, v4, v5)
#define CODEC_PASTE7(func, v1, v2, v3, v4, v5, v6) CODEC_PASTE2(func, v1) CODEC_PASTE6(func, v2, v3, v4, v5, v6)
#define CODEC_PASTE8(func, v1, v2, v3, v4, v5, v6, v7) CODEC_PASTE2(func, v1) CODEC_PASTE7(func, v2, v3, v4, v5, v6, v7)
#define CODEC_PASTE9(func, v1, v2, v3, v4, v5, v6, v7, v8) CODEC_PASTE2(func, v1) CODEC_PASTE8(func, v2, v3, v4, v5, v6, v7, v8)
#define CODEC_PASTE10(func, v1, v2, v3, v4, v5, v6, v7, v8, v9) CODEC_PASTE2(func, v1) CODEC_PASTE9(func, v2, v3, v4, v5, v6, v7, v8, v9)
#define CODEC_PASTE11(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10) CODEC_PASTE2(func, v1) CODEC_PASTE10(func, v2, v3, v4, v5, v6, v7, v8, v9, v10)
#define CODEC_PASTE12(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11) CODEC_PASTE2(func, v1) CODEC_PASTE11(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11)
#define CODEC_PASTE13(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12) CODEC_PASTE2(func, v1) CODEC_PASTE12(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12)
#define CODEC_PASTE14(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13) CODEC_PASTE2(func, v1) CODEC_PASTE13(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13)
#define CODEC_PASTE15(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14) CODEC_PASTE2(func, v1) CODEC_PASTE14(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14)
#define CODEC_PASTE16(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15) CODEC_PASTE2(func, v1) CODEC_PASTE15(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15)
#define CODEC_PASTE17(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16) CODEC_PASTE2(func, v1) CODEC_PASTE16(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16)
#define CODEC_PASTE18(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17) CODEC_PASTE2(func, v1) CODEC_PASTE17(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17)
#define CODEC_PASTE19(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18) CODEC_PASTE2(func, v1) CODEC_PASTE18(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18)
#define CODEC_PASTE20(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19) CODEC_PASTE2(func, v1) CODEC_PASTE19(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19)
#define CODEC_PASTE21(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20) CODEC_PASTE2(func, v1) CODEC_PASTE20(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20)
#define CODEC_PASTE22(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21) CODEC_PASTE2(func, v1) CODEC_PASTE21(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21)
#define CODEC_PASTE23(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22) CODEC_PASTE2(func, v1) CODEC_PASTE22(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22)
#define CODEC_PASTE24(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23) CODEC_PASTE2(func, v1) CODEC_PASTE23(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23)
#define CODEC_PASTE25(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24) CODEC_PASTE2(func, v1) CODEC_PASTE24(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24)
#define CODEC_PASTE26(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25) CODEC_PASTE2(func, v1) CODEC_PASTE25(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25)
#define CODEC_PASTE27(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26) CODEC_PASTE2(func, v1) CODEC_PASTE26(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26)
#define CODEC_PASTE28(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27) CODEC_PASTE2(func, v1) CODEC_PASTE27(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27)
#define CODEC_PASTE29(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28) CODEC_PASTE2(func, v1) CODEC_PASTE28(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28)
#define CODEC_PASTE30(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29) CODEC_PASTE2(func, v1) CODEC_PASTE29(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29)
#define CODEC_PASTE31(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30) CODEC_PASTE2(func, v1) CODEC_PASTE30(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30)
#define CODEC_PASTE32(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31) CODEC_PASTE2(func, v1) CODEC_PASTE31(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31)
#define CODEC_PASTE33(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32) CODEC_PASTE2(func, v1) CODEC_PASTE32(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32)
#define CODEC_PASTE34(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33) CODEC_PASTE2(func, v1) CODEC_PASTE33(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33)
#define CODEC_PASTE35(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34) CODEC_PASTE2(func, v1) CODEC_PASTE34(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34)
#define CODEC_PASTE36(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35) CODEC_PASTE2(func, v1) CODEC_PASTE35(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35)
#define CODEC_PASTE37(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36) CODEC_PASTE2(func, v1) CODEC_PASTE36(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36)
#define CODEC_PASTE38(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37) CODEC_PASTE2(func, v1) CODEC_PASTE37(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37)
#define CODEC_PASTE39(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38) CODEC_PASTE2(func, v1) CODEC_PASTE38(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38)
#define CODEC_PASTE40(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39) CODEC_PASTE2(func, v1) CODEC_PASTE39(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39)
#define CODEC_PASTE41(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40) CODEC_PASTE2(func, v1) CODEC_PASTE40(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40)
#define CODEC_PASTE42(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41) CODEC_PASTE2(func, v1) CODEC_PASTE41(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41)
#define CODEC_PASTE43(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42) CODEC_PASTE2(func, v1) CODEC_PASTE42(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42)
#define CODEC_PASTE44(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43) CODEC_PASTE2(func, v1) CODEC_PASTE43(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43)
#define CODEC_PASTE45(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44) CODEC_PASTE2(func, v1) CODEC_PASTE44(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44)
#define CODEC_PASTE46(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45) CODEC_PASTE2(func, v1) CODEC_PASTE45(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45)
#define CODEC_PASTE47(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46) CODEC_PASTE2(func, v1) CODEC_PASTE46(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46)
#define CODEC_PASTE48(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47) CODEC_PASTE2(func, v1) CODEC_PASTE47(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47)
#define CODEC_PASTE49(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48) CODEC_PASTE2(func, v1) CODEC_PASTE48(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48)
#define CODEC_PASTE50(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49) CODEC_PASTE2(func, v1) CODEC_PASTE49(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49)
#define CODEC_PASTE51(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50) CODEC_PASTE2(func, v1) CODEC_PASTE50(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50)
#define CODEC_PASTE52(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51) CODEC_PASTE2(func, v1) CODEC_PASTE51(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51)
#define CODEC_PASTE53(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52) CODEC_PASTE2(func, v1) CODEC_PASTE52(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52)
#define CODEC_PASTE54(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53) CODEC_PASTE2(func, v1) CODEC_PASTE53(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53)
#define CODEC_PASTE55(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54) CODEC_PASTE2(func, v1) CODEC_PASTE54(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54)
#define CODEC_PASTE56(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55) CODEC_PASTE2(func, v1) CODEC_PASTE55(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55)
#define CODEC_PASTE57(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56) CODEC_PASTE2(func, v1) CODEC_PASTE56(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56)
#define CODEC_PASTE58(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57) CODEC_PASTE2(func, v1) CODEC_PASTE57(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57)
#define CODEC_PASTE59(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58) CODEC_PASTE2(func, v1) CODEC_PASTE58(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58)
#define CODEC_PASTE60(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59) CODEC_PASTE2(func, v1) CODEC_PASTE59(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59)
#define CODEC_PASTE61(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60) CODEC_PASTE2(func, v1) CODEC_PASTE60(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60)
#define CODEC_PASTE62(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61) CODEC_PASTE2(func, v1) CODEC_PASTE61(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61)
#define CODEC_PASTE63(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62) CODEC_PASTE2(func, v1) CODEC_PASTE62(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62)
#define CODEC_PASTE64(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62, v63) CODEC_PASTE2(func, v1) CODEC_PASTE63(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62, v63)

#define CODEC_JSON_KEY_CHAR(v1) \
    static const char *v1() { return #v1; }
#define CODEC_JSON_KEY_CHAR16(v1) \
    static const char16_t *v1() { return u## #v1; }
#define CODEC_JSON_KEY_CHAR32(v1) \
    static const char32_t *v1() { return U## #v1; }
#define CODEC_JSON_KEY_WCHAR(v1) \
    static const wchar_t *v1() { return L## #v1; }
#define CODEC_STATIC_ASSERT(v1) \
    static_assert(Codec<decltype(std::declval<Type>().v1)>::enable, "fail to find impl of Codec");
#define CODEC_TO_JSON_MEMBER(v1) \
    Codec<decltype(t.v1)>::template to_json_member(allocator, jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::v1(), t.v1);
#define CODEC_FROM_JSON_MEMBER(v1) \
    Codec<decltype(t.v1)>::template from_json_member(jsonValue, details::JsonKey<Type, typename JsonValueType::Ch>::v1(), t.v1);
#define CODEC_TO_BINARY(v1) \
    Codec<decltype(t.v1)>::template to_binary<isNeedConvert>(ostream, t.v1);
#define CODEC_FROM_BINARY(v1) \
    Codec<decltype(t.v1)>::template from_binary<isNeedConvert>(istream, t.v1);

#define CODEC_UNIQUE_PTR(CodecType)                                                \
    namespace serialization {                                                      \
        template<>                                                                 \
        struct Codec<std::unique_ptr<CodecType>> : BaseUniquePtrCodec<CodecType> { \
                                                                                   \
            using Type = std::unique_ptr<CodecType>;                               \
                                                                                   \
            constexpr static bool enable = true;                                   \
        };                                                                         \
    }

#define CODEC_ENUM(CodecType, ParentType)                                \
    namespace serialization {                                            \
        template<>                                                       \
        struct Codec<CodecType> : BaseEnumCodec<CodecType, ParentType> { \
                                                                         \
            using Type = CodecType;                                      \
                                                                         \
            constexpr static bool enable = true;                         \
        };                                                               \
    }

#define CODEC_REGISTER_JSON_KEY(CodecType, ...)                 \
    namespace serialization {                                   \
        namespace details {                                     \
                                                                \
            template<>                                          \
            struct JsonKey<CodecType, char> {                   \
                constexpr static bool enable = true;            \
                CODEC_PASTE(CODEC_JSON_KEY_CHAR, __VA_ARGS__)   \
            };                                                  \
                                                                \
            template<>                                          \
            struct JsonKey<CodecType, char16_t> {               \
                constexpr static bool enable = true;            \
                CODEC_PASTE(CODEC_JSON_KEY_CHAR16, __VA_ARGS__) \
            };                                                  \
                                                                \
            template<>                                          \
            struct JsonKey<CodecType, char32_t> {               \
                constexpr static bool enable = true;            \
                CODEC_PASTE(CODEC_JSON_KEY_CHAR16, __VA_ARGS__) \
            };                                                  \
                                                                \
            template<>                                          \
            struct JsonKey<CodecType, wchar_t> {                \
                constexpr static bool enable = true;            \
                CODEC_PASTE(CODEC_JSON_KEY_WCHAR, __VA_ARGS__)  \
            };                                                  \
        }                                                       \
    }

#define CODEC_NONE_WITH_PARENT(CodecType, ParentType)                                        \
    template<>                                                                               \
    struct serialization::Codec<CodecType> : serialization::BaseCodec<CodecType> {           \
                                                                                             \
        using Type = CodecType;                                                              \
                                                                                             \
        static_assert(std::is_same<ParentType, void>::value || Codec<ParentType>::enable);   \
                                                                                             \
        constexpr static bool enable = true;                                                 \
                                                                                             \
        template<typename JsonValueType>                                                     \
        static void to_json(typename JsonValueType::AllocatorType &allocator,                \
                            JsonValueType &jsonValue,                                        \
                            const Type &t) {                                                 \
            WrappedCodec<Type, ParentType>::template to_json(allocator, jsonValue, t);       \
        }                                                                                    \
                                                                                             \
        template<typename JsonValueType>                                                     \
        static void from_json(const JsonValueType &jsonValue,                                \
                              Type &t) {                                                     \
            WrappedCodec<Type, ParentType>::template from_json(jsonValue, t);                \
        }                                                                                    \
                                                                                             \
        template<bool isNeedConvert>                                                         \
        static void to_binary(std::ostream &ostream,                                         \
                              const Type &t) {                                               \
            WrappedCodec<Type, ParentType>::template to_binary<isNeedConvert>(ostream, t);   \
        }                                                                                    \
                                                                                             \
        template<bool isNeedConvert>                                                         \
        static void from_binary(std::istream &istream,                                       \
                                Type &t) {                                                   \
            WrappedCodec<Type, ParentType>::template from_binary<isNeedConvert>(istream, t); \
        }                                                                                    \
    };

#define CODEC_WITH_PARENT(CodecType, ParentType, ...)                                                                  \
    CODEC_REGISTER_JSON_KEY(CodecType, __VA_ARGS__)                                                                    \
                                                                                                                       \
    template<>                                                                                                         \
    struct serialization::Codec<CodecType> : serialization::BaseCodec<CodecType> {                                     \
                                                                                                                       \
        using Type = CodecType;                                                                                        \
                                                                                                                       \
        static_assert(std::is_same<ParentType, void>::value || Codec<ParentType>::enable);                             \
        CODEC_PASTE(CODEC_STATIC_ASSERT, __VA_ARGS__)                                                                  \
                                                                                                                       \
        constexpr static bool enable = true;                                                                           \
                                                                                                                       \
        template<typename JsonValueType>                                                                               \
        static void to_json(typename JsonValueType::AllocatorType &allocator,                                          \
                            JsonValueType &jsonValue,                                                                  \
                            const Type &t) {                                                                           \
            WrappedCodec<Type, ParentType>::template to_json(allocator, jsonValue, t);                                 \
            static_assert(details::JsonKey<Type, typename JsonValueType::Ch>::enable, "fail to find impl of JsonKey"); \
            CODEC_PASTE(CODEC_TO_JSON_MEMBER, __VA_ARGS__)                                                             \
        }                                                                                                              \
                                                                                                                       \
        template<typename JsonValueType>                                                                               \
        static void from_json(const JsonValueType &jsonValue,                                                          \
                              Type &t) {                                                                               \
            WrappedCodec<Type, ParentType>::template from_json(jsonValue, t);                                          \
            static_assert(details::JsonKey<Type, typename JsonValueType::Ch>::enable, "fail to find impl of JsonKey"); \
            CODEC_PASTE(CODEC_FROM_JSON_MEMBER, __VA_ARGS__)                                                           \
        }                                                                                                              \
                                                                                                                       \
        template<bool isNeedConvert>                                                                                   \
        static void to_binary(std::ostream &ostream,                                                                   \
                              const Type &t) {                                                                         \
            WrappedCodec<Type, ParentType>::template to_binary<isNeedConvert>(ostream, t);                             \
            CODEC_PASTE(CODEC_TO_BINARY, __VA_ARGS__)                                                                  \
        }                                                                                                              \
                                                                                                                       \
        template<bool isNeedConvert>                                                                                   \
        static void from_binary(std::istream &istream,                                                                 \
                                Type &t) {                                                                             \
            WrappedCodec<Type, ParentType>::template from_binary<isNeedConvert>(istream, t);                           \
            CODEC_PASTE(CODEC_FROM_BINARY, __VA_ARGS__)                                                                \
        }                                                                                                              \
    };

#define CODEC_NONE(CodecType) CODEC_NONE_WITH_PARENT(CodecType, void)

#define CODEC(CodecType, ...) CODEC_WITH_PARENT(CodecType, void, __VA_ARGS__)

#define CODEC_NONE_WITH_UNIQUE_PTR(CodecType) \
    CODEC_NONE(CodecType)                     \
    CODEC_UNIQUE_PTR(CodecType)

#define CODEC_WITH_UNIQUE_PTR(CodecType) \
    CODEC(CodecType)                     \
    CODEC_UNIQUE_PTR(CodecType)

#endif//SERIALIZATION_H
