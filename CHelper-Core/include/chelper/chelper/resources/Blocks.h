//
// Created by Yancey on 2024-08-28.
//

#ifndef CHELPER_BLOCKS_H
#define CHELPER_BLOCKS_H

namespace CHelper {

    namespace PropertyType {
        enum PropertyType : uint8_t {
            STRING,
            BOOLEAN,
            INTEGER
        };
    }

    typedef union {
        std::u16string *string;
        bool boolean;
        int32_t integer;
    } PropertyValue;

    class Property {
    public:
        PropertyType::PropertyType type = PropertyType::BOOLEAN;
        std::u16string name;
        PropertyValue defaultValue;

        ~Property();
    };

    class BlockStateValue {
    public:
        std::u16string name;
        std::optional<std::u16string> description;
        std::optional<Property> properties;
    };

    class BlockPropertyValue {
    public:
        PropertyType::PropertyType type = PropertyType::BOOLEAN;
        std::u16string propertyName;
        std::vector<PropertyValue> validValues;

        ~BlockPropertyValue();
    };

    class BlockPropertyDescription {
    public:
        std::u16string propertyName;
        std::vector<PropertyValue> values;
    };

    class PerBlockPropertyDescription {
    public:
        std::vector<std::u16string> blocks;
        std::vector<BlockPropertyDescription> properties;
    };

    class BlockPropertyDescriptions {
    public:
        std::vector<BlockPropertyDescription> common;
        std::vector<PerBlockPropertyDescription> block;
    };

    class Blocks {
    public:
        std::vector<BlockStateValue> blockStateValue;
        std::vector<BlockPropertyValue> blockPropertyValues;
        BlockPropertyDescriptions blockPropertyDescriptions;
    };

    CODEC_H(Property)

    CODEC_H(BlockStateValue)

    CODEC_H(BlockPropertyValue)

    CODEC_H(BlockPropertyDescription)

    CODEC_H(PerBlockPropertyDescription)

    CODEC_H(Blocks)

}// namespace CHelper

#endif//CHELPER_BLOCKS_H
