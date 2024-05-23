//
// Created by Yancey on 24-5-23.
//

#ifndef CHELPER_CODEC_H
#define CHELPER_CODEC_H

#define CHELPER_CODEC_JSON_TO(v1) JsonUtil::encode(nlohmann_json_j, #v1, nlohmann_json_t.v1);
#define CHELPER_CODEC_JSON_FROM(v1) JsonUtil::decode(nlohmann_json_j, #v1, nlohmann_json_t.v1);
#define CHELPER_CODEC_BINARY_TO(v1) chelper_binary_writer_j.encode(chelper_binary_writer_t.v1);
#define CHELPER_CODEC_BINARY_FROM(v1) chelper_binary_reader_j.decode(chelper_binary_reader_t.v1);

#define CODEC_H(Type)                                                                                    \
    void to_json(nlohmann::json &nlohmann_json_j, const Type &nlohmann_json_t);                          \
    void from_json(const nlohmann::json &nlohmann_json_j, Type &nlohmann_json_t);                        \
    void to_binary(CHelper::BinaryWriter &chelper_binary_writer_j, const Type &chelper_binary_writer_t); \
    void from_binary(CHelper::BinaryReader &chelper_binary_reader_j, Type &chelper_binary_reader_t);

#define CODEC(Type, ...)                                                                                                                                                                    \
    void to_json(nlohmann::json &nlohmann_json_j, const Type &nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(CHELPER_CODEC_JSON_TO, __VA_ARGS__)) }                            \
    void from_json(const nlohmann::json &nlohmann_json_j, Type &nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(CHELPER_CODEC_JSON_FROM, __VA_ARGS__)) }                        \
    void to_binary(CHelper::BinaryWriter &chelper_binary_writer_j, const Type &chelper_binary_writer_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(CHELPER_CODEC_BINARY_TO, __VA_ARGS__)) } \
    void from_binary(CHelper::BinaryReader &chelper_binary_reader_j, Type &chelper_binary_reader_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(CHELPER_CODEC_BINARY_FROM, __VA_ARGS__)) }

#endif//CHELPER_CODEC_H
