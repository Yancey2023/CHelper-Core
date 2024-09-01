//
// Created by Yancey on 2023/11/7.
//

#pragma once

#ifndef CHELPER_BLOCKID_H
#define CHELPER_BLOCKID_H

#include "NamespaceId.h"
namespace CHelper {

    namespace PropertyType {
        enum PropertyType : uint8_t {
            STRING,
            BOOLEAN,
            INTEGER
        };
    }

    union PropertyValue {
        std::u16string *string;
        bool boolean = true;
        int32_t integer;
    };

    class Property {
    public:
        PropertyType::PropertyType type = PropertyType::BOOLEAN;
        std::u16string name{};
        PropertyValue defaultValue;
        std::optional<std::vector<PropertyValue>> valid;

        Property() = default;

        Property(const Property &aProperty) noexcept;

        Property(Property &&aProperty) noexcept;

        Property &operator=(const Property &aProperty) noexcept;

        Property &operator=(Property &&aProperty) noexcept;

        ~Property();

        void release();
    };

    class BlockPropertyValueDescription {
    public:
        PropertyValue valueName;
        std::optional<std::u16string> description{};
    };

    class BlockPropertyDescription {
    public:
        PropertyType::PropertyType type = PropertyType::BOOLEAN;
        std::u16string propertyName{};
        std::optional<std::u16string> description{};
        std::vector<BlockPropertyValueDescription> values{};

        BlockPropertyDescription() = default;

        BlockPropertyDescription(const BlockPropertyDescription &aBlockPropertyDescription) noexcept;

        BlockPropertyDescription(BlockPropertyDescription &&aBlockPropertyDescription) noexcept;

        BlockPropertyDescription &operator=(const BlockPropertyDescription &aBlockPropertyDescription) noexcept;

        BlockPropertyDescription &operator=(BlockPropertyDescription &&aBlockPropertyDescription) noexcept;

        ~BlockPropertyDescription();

        void release();
    };

    class PerBlockPropertyDescription {
    public:
        std::vector<std::u16string> blocks{};
        std::vector<BlockPropertyDescription> properties{};
    };

    class BlockPropertyDescriptions {
    public:
        std::vector<BlockPropertyDescription> common{};
        std::vector<PerBlockPropertyDescription> block{};

        [[nodiscard]] const BlockPropertyDescription &getPropertyDescription(
                const std::u16string &blockIdWithNamespace,
                const std::u16string &blockId,
                const std::u16string &propertyName) const;
    };

    class BlockId : public NamespaceId {
    public:
        std::optional<std::vector<Property>> properties{};

    private:
        std::vector<std::shared_ptr<Node::NodeBase>> nodeChildren{};
        std::shared_ptr<Node::NodeBase> node{};

    public:
        std::shared_ptr<Node::NodeBase> getNode(const BlockPropertyDescriptions &blockPropertyDescriptions);

        static Node::NodeBase *getNodeAllBlockState();
    };

    class BlockIds {
    public:
        std::shared_ptr<std::vector<std::shared_ptr<BlockId>>> blockStateValues{};
        BlockPropertyDescriptions blockPropertyDescriptions;
    };

    CODEC_H(Property)

    CODEC_H(BlockPropertyDescription)

    CODEC_H(PerBlockPropertyDescription)

    CODEC_H(BlockPropertyDescriptions)

    CODEC_WITH_PARENT_H(BlockId)

    CODEC_H(BlockIds)

}// namespace CHelper

#endif//CHELPER_BLOCKID_H
