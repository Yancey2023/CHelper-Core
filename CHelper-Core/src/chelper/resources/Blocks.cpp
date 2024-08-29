//
// Created by Yancey on 2024-08-28.
//

#include <chelper/resources/Blocks.h>

namespace CHelper {

    Property::~Property() {
        if (type == PropertyType::STRING) {
            delete defaultValue.string;
        }
    }

    BlockPropertyValue::~BlockPropertyValue() {
        if (type == PropertyType::STRING) {
            for (const auto item: validValues) {
                delete item.string;
            }
        }
    }

#if CHelperOnlyReadBinary != true
    PropertyType::PropertyType from_json(const nlohmann::json &j, PropertyValue &t) {
        if (HEDLEY_UNLIKELY(j.is_number_integer())) {
            t.integer = j.get<int32_t>();
            return PropertyType::INTEGER;
        } else if (HEDLEY_UNLIKELY(j.is_boolean())) {
            t.boolean = j.get<bool>();
            return PropertyType::BOOLEAN;
        } else {
            t.string = new std::u16string(utf8::utf8to16(j.get<std::string>()));
            return PropertyType::STRING;
        }
    }

    void to_json(nlohmann::json &j, const PropertyValue &t, PropertyType::PropertyType propertyType) {
        switch (propertyType) {
            case PropertyType::STRING:
                j = *t.string;
                break;
            case PropertyType::BOOLEAN:
                j = t.boolean;
                break;
            case PropertyType::INTEGER:
                j = t.integer;
                break;
        }
    }
#endif

    void from_binary(BinaryReader &binaryReader, PropertyValue &t, PropertyType::PropertyType propertyType) {
        switch (propertyType) {
            case PropertyType::STRING:
                t.string = new std::u16string(binaryReader.read<std::u16string>());
                break;
            case PropertyType::BOOLEAN:
                binaryReader.decode(t.boolean);
                break;
            case PropertyType::INTEGER:
                binaryReader.decode(t.integer);
                break;
        }
    }

#if CHelperOnlyReadBinary != true
    void to_binary(BinaryWriter &binaryWriter, const PropertyValue &t, PropertyType::PropertyType propertyType) {
        binaryWriter.encode((uint8_t) propertyType);
        switch (propertyType) {
            case PropertyType::STRING:
                binaryWriter.encode(*t.string);
                break;
            case PropertyType::BOOLEAN:
                binaryWriter.encode(t.boolean);
                break;
            case PropertyType::INTEGER:
                binaryWriter.encode(t.integer);
                break;
        }
    }
#endif

#if CHelperOnlyReadBinary != true
    void from_json(const nlohmann::json &j, Property &t) {
        JsonUtil::decode(j, "name", t.name);
        t.type = from_json(j.at("defaultValue"), t.defaultValue);
    }

    void to_json(nlohmann::json &j, const Property &t) {
        JsonUtil::encode(j, "name", t.name);
        to_json(j["value"], t.defaultValue, t.type);
    }
#endif

    void from_binary(BinaryReader &binaryReader, Property &t) {
        binaryReader.decode(t.name);
        from_binary(binaryReader, t.defaultValue, t.type);
    }

#if CHelperOnlyReadBinary != true
    void to_binary(BinaryWriter &binaryWriter, const Property &t) {
        binaryWriter.encode(t.name);
        to_binary(binaryWriter, t.defaultValue, t.type);
    }
#endif

    CODEC(BlockStateValue, name, description, properties)

#if CHelperOnlyReadBinary != true
    void from_json(const nlohmann::json &j, BlockPropertyValue &t) {
        JsonUtil::decode(j, "propertyName", t.propertyName);
        const nlohmann::json &jsonValue = j.at("defaultValue");
        if (HEDLEY_UNLIKELY(jsonValue.is_number_integer())) {
            t.type = PropertyType::INTEGER;
            t.defaultValue.integer = jsonValue.get<int32_t>();
        } else if (HEDLEY_UNLIKELY(jsonValue.is_boolean())) {
            t.type = PropertyType::BOOLEAN;
            t.defaultValue.boolean = jsonValue.get<bool>();
        } else {
            t.type = PropertyType::STRING;
            t.defaultValue.string = new std::u16string(utf8::utf8to16(jsonValue.get<std::string>()));
        }
    }

    void to_json(nlohmann::json &j, const BlockPropertyValue &t) {
        JsonUtil::encode(j, "propertyName", t.propertyName);
        switch (t.type) {
            case PropertyType::STRING:
                JsonUtil::encode(j, "value", *t.defaultValue.string);
                break;
            case PropertyType::BOOLEAN:
                JsonUtil::encode(j, "value", t.defaultValue.boolean);
                break;
            case PropertyType::INTEGER:
                JsonUtil::encode(j, "value", t.defaultValue.integer);
                break;
        }
    }
#endif

    void from_binary(BinaryReader &binaryReader, BlockPropertyValue &t) {
        binaryReader.decode(t.propertyName);
        switch (t.type) {
            case PropertyType::STRING:
                t.defaultValue.string = new std::u16string(binaryReader.read<std::u16string>());
                break;
            case PropertyType::BOOLEAN:
                binaryReader.decode(t.defaultValue.boolean);
                break;
            case PropertyType::INTEGER:
                binaryReader.decode(t.defaultValue.integer);
                break;
        }
    }

#if CHelperOnlyReadBinary != true
    void to_binary(BinaryWriter &binaryWriter, const BlockPropertyValue &t) {
        binaryWriter.encode(t.propertyName);
        binaryWriter.encode((uint8_t) t.type);
        switch (t.type) {
            case PropertyType::STRING:
                binaryWriter.encode(*t.defaultValue.string);
                break;
            case PropertyType::BOOLEAN:
                binaryWriter.encode(t.defaultValue.boolean);
                break;
            case PropertyType::INTEGER:
                binaryWriter.encode(t.defaultValue.integer);
                break;
        }
    }
#endif

}// namespace CHelper
