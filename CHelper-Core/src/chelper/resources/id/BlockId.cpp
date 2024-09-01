//
// Created by Yancey on 2023/11/7.
//

#include <chelper/node/NodeBase.h>
#include <chelper/node/param/NodeBoolean.h>
#include <chelper/node/param/NodeString.h>
#include <chelper/node/param/NodeText.h>
#include <chelper/node/template/NodeNumber.h>
#include <chelper/node/util/NodeEntry.h>
#include <chelper/node/util/NodeList.h>
#include <chelper/node/util/NodeOr.h>
#include <chelper/node/util/NodeSingleSymbol.h>
#include <chelper/resources/id/BlockId.h>

namespace CHelper {

    static std::unique_ptr<Node::NodeBase> nodeBlockStateLeftBracket = std::make_unique<Node::NodeSingleSymbol>(
            u"BLOCK_STATE_LEFT_BRACKET", u"方块状态左括号", '[');
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryKey = std::make_unique<Node::NodeString>(
            u"BLOCK_STATE_ENTRY_KEY", u"方块状态键值对的键", false, false, false);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntrySeparator = std::make_unique<Node::NodeSingleSymbol>(
            u"BLOCK_STATE_ENTRY_SEPARATOR", u"方块状态键值对分隔符", '=');
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryValueBoolean = std::make_unique<Node::NodeBoolean>(
            u"BLOCK_STATE_ENTRY_VALUE_BOOLEAN", u"方块状态键值对的值（布尔值）",
            std::nullopt, std::nullopt);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryValueInteger = Node::NodeInteger::make(
            u"BLOCK_STATE_ENTRY_VALUE_INTEGER", u"方块状态键值对的值（整数）",
            std::nullopt, std::nullopt);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryValueFloat = Node::NodeFloat::make(
            u"BLOCK_STATE_ENTRY_VALUE_FLOAT", u"方块状态键值对的值（小数）",
            std::nullopt, std::nullopt);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryValueString = std::make_unique<Node::NodeString>(
            u"BLOCK_STATE_ENTRY_VALUE_STRING", u"方块状态键值对的值（字符串）",
            false, true, false);
    static std::unique_ptr<Node::NodeBase> nodeBlockStateEntryAllValue = std::make_unique<Node::NodeOr>(
            u"BLOCK_STATE_ENTRY_VALUE", u"方块状态键值对的值",
            std::vector<const Node::NodeBase *>{
                    nodeBlockStateEntryValueBoolean.get(), nodeBlockStateEntryValueInteger.get(),
                    nodeBlockStateEntryValueFloat.get(), nodeBlockStateEntryValueString.get()},
            false, false, true,
            u"类型不匹配，当前内容不是有效的方块状态值");
    static std::unique_ptr<Node::NodeBase> nodeBlockStateAllEntry = std::make_unique<Node::NodeEntry>(
            u"BLOCK_STATE_ENTRY", u"方块状态键值对", nodeBlockStateEntryKey.get(),
            nodeBlockStateEntrySeparator.get(), nodeBlockStateEntryAllValue.get());
    static std::unique_ptr<Node::NodeBase> nodeBlockStateSeparator = std::make_unique<Node::NodeSingleSymbol>(
            u"BLOCK_STATE_SEPARATOR", u"方块状态分隔符", ',');
    static std::unique_ptr<Node::NodeBase> nodeBlockStateRightBracket = std::make_unique<Node::NodeSingleSymbol>(
            u"BLOCK_STATE_RIGHT_BRACKET", u"方块状态右括号", ']');
    static std::unique_ptr<Node::NodeBase> nodeAllBlockState = std::make_unique<Node::NodeList>(
            u"BLOCK_STATE", u"方块状态",
            nodeBlockStateLeftBracket.get(), nodeBlockStateAllEntry.get(),
            nodeBlockStateSeparator.get(), nodeBlockStateRightBracket.get());

    Property::Property(const Property &aProperty) noexcept {
        type = aProperty.type;
        name = aProperty.name;
        if (type == PropertyType::STRING) {
            defaultValue.string = new std::u16string(*aProperty.defaultValue.string);
            if (aProperty.valid.has_value()) {
                size_t size = aProperty.valid.value().size();
                valid = std::vector<PropertyValue>(size);
                for (int i = 0; i < size; ++i) {
                    valid.value()[i].string = new std::u16string(*aProperty.valid.value()[i].string);
                }
            } else {
                valid = std::nullopt;
            }
        } else {
            defaultValue = aProperty.defaultValue;
            valid = aProperty.valid;
        }
    }

    Property::Property(Property &&aProperty) noexcept {
        type = aProperty.type;
        aProperty.type = PropertyType::BOOLEAN;
        name = std::move(aProperty.name);
        defaultValue = aProperty.defaultValue;
        aProperty.defaultValue.boolean = true;
        valid = std::move(aProperty.valid);
    }

    Property &Property::operator=(const Property &aProperty) noexcept {
        type = aProperty.type;
        name = aProperty.name;
        if (type == PropertyType::STRING) {
            defaultValue.string = new std::u16string(*aProperty.defaultValue.string);
            if (aProperty.valid.has_value()) {
                size_t size = aProperty.valid.value().size();
                valid = std::vector<PropertyValue>(size);
                for (int i = 0; i < size; ++i) {
                    valid.value()[i].string = new std::u16string(*aProperty.valid.value()[i].string);
                }
            } else {
                valid = std::nullopt;
            }
        } else {
            defaultValue = aProperty.defaultValue;
            valid = aProperty.valid;
        }
        return *this;
    }

    Property &Property::operator=(Property &&aProperty) noexcept {
        type = aProperty.type;
        aProperty.type = PropertyType::BOOLEAN;
        name = std::move(aProperty.name);
        defaultValue = aProperty.defaultValue;
        aProperty.defaultValue.boolean = true;
        valid = std::move(aProperty.valid);
        return *this;
    }

    Property::~Property() {
        if (type == PropertyType::STRING) {
            delete defaultValue.string;
            if (valid.has_value()) {
                for (const auto &item: valid.value()) {
                    delete item.string;
                }
            }
        }
    }

    void Property::release() {
        if (type == PropertyType::STRING) {
            delete defaultValue.string;
            if (valid.has_value()) {
                for (const auto &item: valid.value()) {
                    delete item.string;
                }
            }
        }
        type = PropertyType::BOOLEAN;
        defaultValue.boolean = true;
        valid = std::nullopt;
    }

    BlockPropertyDescription::BlockPropertyDescription(const BlockPropertyDescription &aBlockPropertyDescription) noexcept {
        type = aBlockPropertyDescription.type;
        propertyName = aBlockPropertyDescription.propertyName;
        description = aBlockPropertyDescription.description;
        if (type == PropertyType::STRING) {
            size_t size = aBlockPropertyDescription.values.size();
            values.resize(size);
            for (int i = 0; i < size; ++i) {
                BlockPropertyValueDescription &t1 = values[i];
                const BlockPropertyValueDescription &t2 = aBlockPropertyDescription.values[i];
                t1.valueName.string = new std::u16string(*t2.valueName.string);
                t1.description = t2.description;
            }
        } else {
            values = aBlockPropertyDescription.values;
        }
    }

    BlockPropertyDescription::BlockPropertyDescription(BlockPropertyDescription &&aBlockPropertyDescription) noexcept {
        type = aBlockPropertyDescription.type;
        aBlockPropertyDescription.type = PropertyType::BOOLEAN;
        propertyName = std::move(aBlockPropertyDescription.propertyName);
        description = std::move(aBlockPropertyDescription.description);
        values = std::move(aBlockPropertyDescription.values);
    }

    BlockPropertyDescription &BlockPropertyDescription::operator=(const BlockPropertyDescription &aBlockPropertyDescription) noexcept {
        type = aBlockPropertyDescription.type;
        propertyName = aBlockPropertyDescription.propertyName;
        description = aBlockPropertyDescription.description;
        if (type == PropertyType::STRING) {
            size_t size = aBlockPropertyDescription.values.size();
            values.resize(size);
            for (int i = 0; i < size; ++i) {
                BlockPropertyValueDescription &t1 = values[i];
                const BlockPropertyValueDescription &t2 = aBlockPropertyDescription.values[i];
                t1.valueName.string = new std::u16string(*t2.valueName.string);
                t1.description = t2.description;
            }
        } else {
            values = aBlockPropertyDescription.values;
        }
        return *this;
    }

    BlockPropertyDescription &BlockPropertyDescription::operator=(BlockPropertyDescription &&aBlockPropertyDescription) noexcept {
        type = aBlockPropertyDescription.type;
        aBlockPropertyDescription.type = PropertyType::BOOLEAN;
        propertyName = std::move(aBlockPropertyDescription.propertyName);
        description = std::move(aBlockPropertyDescription.description);
        values = std::move(aBlockPropertyDescription.values);
        return *this;
    }

    BlockPropertyDescription::~BlockPropertyDescription() {
        if (type == PropertyType::STRING) {
            for (const auto &item: values) {
                delete item.valueName.string;
            }
        }
    }
    void BlockPropertyDescription::release() {
        if (type == PropertyType::STRING) {
            for (const auto &item: values) {
                delete item.valueName.string;
            }
        }
        type = PropertyType::BOOLEAN;
        values.clear();
    }

    const BlockPropertyDescription &BlockPropertyDescriptions::getPropertyDescription(
            const std::u16string &blockIdWithNamespace,
            const std::u16string &blockId,
            const std::u16string &propertyName) const {
        for (const auto &item: block) {
            if ((std::find(item.blocks.begin(), item.blocks.end(), blockId) != item.blocks.end() ||
                 std::find(item.blocks.begin(), item.blocks.end(), blockIdWithNamespace) != item.blocks.end())) {
                const auto &it = std::find_if(
                        item.properties.begin(), item.properties.end(),
                        [&propertyName](const BlockPropertyDescription &item1) -> bool {
                            return item1.propertyName == propertyName;
                        });
                if (HEDLEY_LIKELY(it != item.properties.end())) {
                    return *it;
                }
            }
        }
        const auto &it = std::find_if(
                common.begin(), common.end(),
                [&propertyName](const BlockPropertyDescription &item1) -> bool {
                    return item1.propertyName == propertyName;
                });
        if (HEDLEY_LIKELY(it != common.end())) {
            return *it;
        }
        Profile::push("fail to find block property value by block id {} and property name {}", blockIdWithNamespace, propertyName);
        throw std::runtime_error("fail to find block property value by block id and property name");
    }

    std::shared_ptr<Node::NodeBase> getBlockStateValueNode(
            const BlockPropertyValueDescription &blockPropertyValueDescription,
            const PropertyType::PropertyType &type,
            const std::optional<std::u16string> &defaultDescription,
            bool isDefaultValue,
            bool isInvalid) {
        std::optional<std::u16string> description;
        if (blockPropertyValueDescription.description.has_value()) {
            description = blockPropertyValueDescription.description.value();
        } else if (defaultDescription.has_value()) {
            description = defaultDescription.value();
        }
        if (isDefaultValue) {
            if (description.has_value()) {
                description = u"（默认值）" + description.value();
            } else {
                description = u"（默认值）";
            }
        }
        if (isInvalid) {
            if (description.has_value()) {
                description = u"（无效）" + description.value();
            } else {
                description = u"（无效）";
            }
        }
        switch (type) {
            case PropertyType::STRING:
                return std::make_shared<Node::NodeText>(
                        u"BLOCK_STATE_ENTRY_VALUE_STRING", u"方块状态键值对的键（字符串）",
                        NormalId::make(u'\"' + *blockPropertyValueDescription.valueName.string + u'\"', description));
            case PropertyType::INTEGER:
                return std::make_shared<Node::NodeText>(
                        u"BLOCK_STATE_ENTRY_VALUE_INTEGER", u"方块状态键值对的键（整数）",
                        NormalId::make(utf8::utf8to16(std::to_string(blockPropertyValueDescription.valueName.integer)), description),
                        [](const Node::NodeBase *node1, TokenReader &tokenReader) -> ASTNode {
                            return tokenReader.readIntegerASTNode(node1);
                        });
            case PropertyType::BOOLEAN:
                return std::make_shared<Node::NodeText>(
                        u"BLOCK_STATE_ENTRY_VALUE_BOOLEAN", u"方块状态键值对的键（布尔值）",
                        NormalId::make(blockPropertyValueDescription.valueName.boolean ? u"true" : u"false", description));
            default:
                HEDLEY_UNREACHABLE();
        }
    }

    std::shared_ptr<Node::NodeBase> getBlockStateNode(
            std::vector<std::shared_ptr<Node::NodeBase>> &nodeChildren,
            const BlockPropertyDescription &blockPropertyDescription,
            PropertyValue defaultValue,
            const std::optional<std::vector<PropertyValue>> &valid) {
        std::vector<const Node::NodeBase *> valueNodes;
        valueNodes.reserve(blockPropertyDescription.values.size());
        for (auto &item: blockPropertyDescription.values) {
            bool isDefaultValue;
            switch (blockPropertyDescription.type) {
                case PropertyType::STRING:
                    isDefaultValue = *item.valueName.string == *defaultValue.string;
                    break;
                case PropertyType::BOOLEAN:
                    isDefaultValue = item.valueName.boolean == defaultValue.boolean;
                    break;
                case PropertyType::INTEGER:
                    isDefaultValue = item.valueName.integer == defaultValue.integer;
                    break;
                default:
                    HEDLEY_UNREACHABLE();
            }
            bool isInvalid;
            if (valid.has_value()) {
                isInvalid = true;
                switch (blockPropertyDescription.type) {
                    case PropertyType::STRING:
                        for (const auto &item1: valid.value()) {
                            if (*item.valueName.string == *item1.string) {
                                isInvalid = false;
                                break;
                            }
                        }
                        break;
                    case PropertyType::BOOLEAN:
                        for (const auto &item1: valid.value()) {
                            if (item.valueName.boolean == item1.boolean) {
                                isInvalid = false;
                                break;
                            }
                        }
                        break;
                    case PropertyType::INTEGER:
                        for (const auto &item1: valid.value()) {
                            if (item.valueName.integer == item1.integer) {
                                isInvalid = false;
                                break;
                            }
                        }
                        break;
                    default:
                        HEDLEY_UNREACHABLE();
                }
            } else {
                isInvalid = false;
            }
            std::shared_ptr<Node::NodeBase> node = getBlockStateValueNode(
                    item, blockPropertyDescription.type,
                    blockPropertyDescription.description, isDefaultValue, isInvalid);
            valueNodes.push_back(node.get());
            nodeChildren.push_back(std::move(node));
        }
        //key = value
        auto nodeKey = std::make_shared<Node::NodeText>(
                u"BLOCK_STATE_ENTRY_KEY", u"方块状态键值对的键",
                NormalId::make(u'\"' + blockPropertyDescription.propertyName + u'\"', blockPropertyDescription.description));
        auto nodeValue = std::make_shared<Node::NodeOr>(
                u"BLOCK_STATE_ENTRY_VALUE", u"方块状态键值对的值",
                std::move(valueNodes), false);
        auto result = std::make_shared<Node::NodeEntry>(
                u"BLOCK_STATE_PER_ENTRY", u"方块状态单个键值对",
                nodeKey.get(),
                nodeBlockStateEntrySeparator.get(),
                nodeValue.get());
        nodeChildren.push_back(std::move(nodeKey));
        nodeChildren.push_back(std::move(nodeValue));
        return std::move(result);
    }

    std::shared_ptr<Node::NodeBase> BlockId::getNode(const BlockPropertyDescriptions &blockPropertyDescriptions) {
        if (node == nullptr) {
            std::vector<const Node::NodeBase *> blockStateEntryChildNode2;
            //已知的方块状态
            if (HEDLEY_LIKELY(properties.has_value())) {
                blockStateEntryChildNode2.reserve(2);
                std::vector<const Node::NodeBase *> blockStateEntryChildNode1;
                blockStateEntryChildNode1.reserve(properties.value().size());
                std::transform(properties.value().begin(), properties.value().end(),
                               std::back_inserter(blockStateEntryChildNode1),
                               [this, &blockPropertyDescriptions](const auto &item) -> const Node::NodeBase * {
                                   const BlockPropertyDescription &blockPropertyDescription = blockPropertyDescriptions.getPropertyDescription(
                                           getIdWithNamespace()->name,
                                           name,
                                           item.name);
                                   std::shared_ptr<Node::NodeBase> node1 = getBlockStateNode(
                                           nodeChildren, blockPropertyDescription,
                                           item.defaultValue, item.valid);
                                   Node::NodeBase *nodePtr = node1.get();
                                   nodeChildren.push_back(std::move(node1));
                                   return nodePtr;
                               });
                auto nodeChild = std::make_shared<Node::NodeOr>(
                        u"BLOCK_STATE_ENTRY", u"方块状态键值对",
                        std::move(blockStateEntryChildNode1), false);
                blockStateEntryChildNode2.push_back(nodeChild.get());
                nodeChildren.push_back(std::move(nodeChild));
            }
            //其他未知的方块状态
            blockStateEntryChildNode2.push_back(nodeBlockStateAllEntry.get());
            //把所有方块状态拼在一起
            auto nodeValue = std::make_shared<Node::NodeOr>(u"BLOCK_STATE_ENTRY", u"方块状态键值对",
                                                            std::move(blockStateEntryChildNode2), false, true);
            node = std::make_shared<Node::NodeList>(
                    u"BLOCK_STATE",
                    u"方块状态",
                    nodeBlockStateLeftBracket.get(),
                    nodeValue.get(),
                    nodeBlockStateSeparator.get(),
                    nodeBlockStateRightBracket.get());
            nodeChildren.push_back(std::move(nodeValue));
        }
        return node;
    }

    Node::NodeBase *BlockId::getNodeAllBlockState() {
        return nodeAllBlockState.get();
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

    void to_json(nlohmann::json &j, const PropertyValue &t, const PropertyType::PropertyType &propertyType) {
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
            default:
                HEDLEY_UNREACHABLE();
        }
    }
#endif

    void from_binary(BinaryReader &binaryReader, PropertyValue &t, const PropertyType::PropertyType &propertyType) {
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
            default:
                throw std::runtime_error("error block state property type");
        }
    }

#if CHelperOnlyReadBinary != true
    void to_binary(BinaryWriter &binaryWriter, const PropertyValue &t, const PropertyType::PropertyType &propertyType) {
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
            default:
#if CHelperDebug == true
                throw std::runtime_error("error block state property type");
#else
                HEDLEY_UNREACHABLE();
#endif
        }
    }
#endif

#if CHelperOnlyReadBinary != true
    void from_json(const nlohmann::json &j, Property &t) {
        t.release();
        JsonUtil::decode(j, "name", t.name);
        t.type = from_json(j.at("defaultValue"), t.defaultValue);
        const auto &it = j.find("valid");
        if (it == j.end()) {
            t.valid = std::nullopt;
        } else {
            t.valid = std::make_optional<std::vector<PropertyValue>>();
            t.valid.value().reserve(it.value().size());
            for (const auto &item: it.value()) {
                PropertyValue propertyValue;
                PropertyType::PropertyType type = from_json(item, propertyValue);
                if (t.type != type) {
                    if (type == PropertyType::STRING) {
                        delete propertyValue.string;
                    }
                    throw std::runtime_error("error block state property type");
                }
                t.valid.value().push_back(propertyValue);
            }
        }
    }

    void to_json(nlohmann::json &j, const Property &t) {
        JsonUtil::encode(j, "name", t.name);
        to_json(j["defaultValue"], t.defaultValue, t.type);
        if (t.valid.has_value()) {
            nlohmann::json jsonValid;
            for (const auto &item: t.valid.value()) {
                nlohmann::json jsonValue;
                to_json(jsonValue, item, t.type);
                jsonValid.push_back(std::move(jsonValue));
            }
            j["valid"] = std::move(jsonValid);
        }
    }
#endif

    void from_binary(BinaryReader &binaryReader, Property &t) {
        t.release();
        binaryReader.decode(t.name);
        binaryReader.decode((uint8_t &) t.type);
#if CHelperDebug == true
        if (t.type != PropertyType::BOOLEAN && t.type != PropertyType::STRING && t.type != PropertyType::INTEGER) {
            throw std::runtime_error("error block state property type");
        }
#endif
        from_binary(binaryReader, t.defaultValue, t.type);
        if (binaryReader.read<bool>()) {
            t.valid = std::make_optional<std::vector<PropertyValue>>();
            size_t size = binaryReader.readSize();
            t.valid.value().reserve(size);
            for (int i = 0; i < size; ++i) {
                PropertyValue propertyValue;
                from_binary(binaryReader, propertyValue, t.type);
                t.valid.value().push_back(propertyValue);
            }
        } else {
            t.valid = std::nullopt;
        }
    }

#if CHelperOnlyReadBinary != true
    void to_binary(BinaryWriter &binaryWriter, const Property &t) {
        binaryWriter.encode(t.name);
#if CHelperDebug == true
        if (t.type != PropertyType::BOOLEAN && t.type != PropertyType::STRING && t.type != PropertyType::INTEGER) {
            throw std::runtime_error("error block state property type");
        }
#endif
        binaryWriter.encode((uint8_t &) t.type);
        to_binary(binaryWriter, t.defaultValue, t.type);
        binaryWriter.encode(t.valid.has_value());
        if (t.valid.has_value()) {
            binaryWriter.encodeSize(t.valid.value().size());
            for (const auto &item: t.valid.value()) {
                to_binary(binaryWriter, item, t.type);
            }
        }
    }
#endif

#if CHelperOnlyReadBinary != true
    void from_json(const nlohmann::json &j, BlockPropertyDescription &t) {
        t.release();
        JsonUtil::decode(j, "propertyName", t.propertyName);
        JsonUtil::decode(j, "description", t.description);
        const nlohmann::json &jsonValues = j.at("values");
        bool hasPropertyType = false;
        for (const auto &item: jsonValues) {
            BlockPropertyValueDescription blockPropertyValueDescription;
            PropertyType::PropertyType type = from_json(item["valueName"], blockPropertyValueDescription.valueName);
            if (HEDLEY_LIKELY(hasPropertyType)) {
                if (t.type != type) {
                    if (type == PropertyType::STRING) {
                        delete blockPropertyValueDescription.valueName.string;
                    }
                    throw std::runtime_error("error block state property type");
                }
            } else {
                hasPropertyType = true;
                t.type = type;
            }
            JsonUtil::decode(item, "description", blockPropertyValueDescription.description);
            t.values.push_back(std::move(blockPropertyValueDescription));
        }
    }

    void to_json(nlohmann::json &j, const BlockPropertyDescription &t) {
        JsonUtil::encode(j, "propertyName", t.propertyName);
        JsonUtil::encode(j, "description", t.description);
        nlohmann::json jsonValues;
        for (const auto &item: t.values) {
            nlohmann::json jsonValue;
            to_json(jsonValue["valueName"], item.valueName, t.type);
            JsonUtil::encode(jsonValue, "description", item.description);
            jsonValues.push_back(std::move(jsonValue));
        }
        j["values"] = std::move(jsonValues);
    }
#endif

    void from_binary(BinaryReader &binaryReader, BlockPropertyDescription &t) {
        t.release();
        binaryReader.decode(t.propertyName);
        binaryReader.decode(t.description);
        binaryReader.decode((uint8_t &) t.type);
        size_t size = binaryReader.readSize();
        t.values.reserve(size);
        for (int i = 0; i < size; ++i) {
            BlockPropertyValueDescription blockPropertyValueDescription;
            from_binary(binaryReader, blockPropertyValueDescription.valueName, t.type);
            binaryReader.decode(blockPropertyValueDescription.description);
            t.values.push_back(std::move(blockPropertyValueDescription));
        }
    }

#if CHelperOnlyReadBinary != true
    void to_binary(BinaryWriter &binaryWriter, const BlockPropertyDescription &t) {
        binaryWriter.encode(t.propertyName);
        binaryWriter.encode(t.description);
        binaryWriter.encode((uint8_t &) t.type);
        binaryWriter.encodeSize(t.values.size());
        for (const auto &item: t.values) {
            to_binary(binaryWriter, item.valueName, t.type);
            binaryWriter.encode(item.description);
        }
    }
#endif

    CODEC(PerBlockPropertyDescription, blocks, properties)

    CODEC(BlockPropertyDescriptions, common, block)

    CODEC_WITH_PARENT(BlockId, NamespaceId, properties)

    CODEC(BlockIds, blockStateValues, blockPropertyDescriptions)

}// namespace CHelper
