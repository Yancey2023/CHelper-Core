//
// Created by Yancey on2024-5-23.
//

#ifndef CHELPER_CODEC_H
#define CHELPER_CODEC_H

#pragma warning(disable:4068)
#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-macro-parentheses"

#if CHelperOnlyReadBinary == true
#define CHELPER_CODEC_JSON_TO(v1)
#define CHELPER_CODEC_JSON_FROM(v1)
#define CHELPER_CODEC_BINARY_TO(v1)
#else
#define CHELPER_CODEC_JSON_TO(v1) JsonUtil::encode(j, #v1, t.v1);
#define CHELPER_CODEC_JSON_FROM(v1) JsonUtil::decode(j, #v1, t.v1);
#define CHELPER_CODEC_BINARY_TO(v1) binaryWriter.encode(t.v1);
#endif
#define CHELPER_CODEC_BINARY_FROM(v1) binaryReader.decode(t.v1);

#if CHelperOnlyReadBinary == true
#define CODEC_H(Type)                                                   \
    void from_binary(CHelper::BinaryReader &binaryReader, Type &t);
#else
#define CODEC_H(Type)                                                   \
    void to_json(nlohmann::json &j, const Type &t);                     \
    void from_json(const nlohmann::json &j, Type &t);                   \
    void to_binary(CHelper::BinaryWriter &binaryWriter, const Type &t); \
    void from_binary(CHelper::BinaryReader &binaryReader, Type &t);
#endif

#if CHelperOnlyReadBinary == true
#define CODEC(Type, ...)                                                                  \
    void from_binary(CHelper::BinaryReader &binaryReader, Type &t) {                      \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(CHELPER_CODEC_BINARY_FROM, __VA_ARGS__)) \
    }
#else
#define CODEC(Type, ...)                                                                  \
    void to_json(nlohmann::json &j, const Type &t) {                                      \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(CHELPER_CODEC_JSON_TO, __VA_ARGS__))     \
    }                                                                                     \
    void from_json(const nlohmann::json &j, Type &t) {                                    \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(CHELPER_CODEC_JSON_FROM, __VA_ARGS__))   \
    }                                                                                     \
    void to_binary(CHelper::BinaryWriter &binaryWriter, const Type &t) {                  \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(CHELPER_CODEC_BINARY_TO, __VA_ARGS__))   \
    }                                                                                     \
    void from_binary(CHelper::BinaryReader &binaryReader, Type &t) {                      \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(CHELPER_CODEC_BINARY_FROM, __VA_ARGS__)) \
    }
#endif

#define CODEC_UNIQUE_PTR_H(Type) CODEC_H(std::unique_ptr<Type>)

#if CHelperOnlyReadBinary == true
#define CODEC_UNIQUE_PTR(Type)                                                        \
    void from_binary(CHelper::BinaryReader &binaryReader, std::unique_ptr<Type> &t) { \
        t = std::make_unique<Type>();                                                 \
        binaryReader.decode(*t);                                                      \
    }
#else
#define CODEC_UNIQUE_PTR(Type)                                                            \
    void to_json(nlohmann::json &j, const std::unique_ptr<Type> &t) {                     \
        j = *t;                                                                           \
    }                                                                                     \
    void from_json(const nlohmann::json &j, std::unique_ptr<Type> &t) {                   \
        t = std::make_unique<Type>();                                                     \
        j.get_to(*t);                                                                     \
    }                                                                                     \
    void to_binary(CHelper::BinaryWriter &binaryWriter, const std::unique_ptr<Type> &t) { \
        binaryWriter.encode(*t);                                                          \
    }                                                                                     \
    void from_binary(CHelper::BinaryReader &binaryReader, std::unique_ptr<Type> &t) {     \
        t = std::make_unique<Type>();                                                     \
        binaryReader.decode(*t);                                                          \
    }
#endif

#define CODEC_WITH_UNIQUE_PTR_H(Type) \
    CODEC_H(Type)                     \
    CODEC_UNIQUE_PTR_H(Type)

#define CODEC_WITH_UNIQUE_PTR(Type, ...) \
    CODEC(Type, __VA_ARGS__)             \
    CODEC_UNIQUE_PTR(Type)

#define CODEC_WITH_PARENT_H(Type) CODEC_H(Type)

#if CHelperOnlyReadBinary == true
#define CODEC_WITH_PARENT(Type, ParentType, ...)                                          \
    void from_binary(CHelper::BinaryReader &binaryReader, Type &t) {                      \
        from_binary(binaryReader, (ParentType &) t);                                      \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(CHELPER_CODEC_BINARY_FROM, __VA_ARGS__)) \
    }
#else
#define CODEC_WITH_PARENT(Type, ParentType, ...)                                          \
    void to_json(nlohmann::json &j, const Type &t) {                                      \
        to_json(j, (ParentType &) t);                                                     \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(CHELPER_CODEC_JSON_TO, __VA_ARGS__))     \
    }                                                                                     \
    void from_json(const nlohmann::json &j, Type &t) {                                    \
        from_json(j, (ParentType &) t);                                                   \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(CHELPER_CODEC_JSON_FROM, __VA_ARGS__))   \
    }                                                                                     \
    void to_binary(CHelper::BinaryWriter &binaryWriter, const Type &t) {                  \
        to_binary(binaryWriter, (ParentType &) t);                                        \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(CHELPER_CODEC_BINARY_TO, __VA_ARGS__))   \
    }                                                                                     \
    void from_binary(CHelper::BinaryReader &binaryReader, Type &t) {                      \
        from_binary(binaryReader, (ParentType &) t);                                      \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(CHELPER_CODEC_BINARY_FROM, __VA_ARGS__)) \
    }
#endif

#define CODEC_NODE_H(Type) CODEC_WITH_UNIQUE_PTR_H(Type)

#define CODEC_NODE(Type, ...)                      \
    CODEC_WITH_PARENT(Type, NodeBase, __VA_ARGS__) \
    CODEC_UNIQUE_PTR(Type)

#define CODEC_ENUM_H(Type)                                               \
    static_assert(std::is_enum<Type>::value, #Type " must be an enum!"); \
    CODEC_H(Type)

#if CHelperOnlyReadBinary == true
#define CODEC_ENUM(Type, ValueType)                                      \
    static_assert(std::is_enum<Type>::value, #Type " must be an enum!"); \
    void from_binary(CHelper::BinaryReader &binaryReader, Type &t) {     \
        t = static_cast<Type>(binaryReader.read<ValueType>());           \
    }
#else
#define CODEC_ENUM(Type, ValueType)                                      \
    static_assert(std::is_enum<Type>::value, #Type " must be an enum!"); \
    void to_json(nlohmann::json &j, const Type &t) {                     \
        j = static_cast<ValueType>(t);                                   \
    }                                                                    \
    void from_json(const nlohmann::json &j, Type &t) {                   \
        j.get_to(reinterpret_cast<ValueType &>(t));                      \
    }                                                                    \
    void to_binary(CHelper::BinaryWriter &binaryWriter, const Type &t) { \
        binaryWriter.encode(static_cast<ValueType>(t));                  \
    }                                                                    \
    void from_binary(CHelper::BinaryReader &binaryReader, Type &t) {     \
        t = static_cast<Type>(binaryReader.read<ValueType>());           \
    }
#endif

#pragma clang diagnostic pop

#endif//CHELPER_CODEC_H
