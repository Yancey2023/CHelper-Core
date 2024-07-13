//
// Created by Yancey on2024-5-21.
//

#include "../chelper/node/json/NodeJsonBoolean.h"
#include "../chelper/node/json/NodeJsonFloat.h"
#include "../chelper/node/json/NodeJsonInteger.h"
#include "../chelper/node/json/NodeJsonNull.h"
#include "../chelper/resources/CPack.h"
#include "param_deliver.h"
#include <gtest/gtest.h>

template<class T>
void test(std::function<T()> getInstance,
          std::function<void(const T &, const T &)> testEqual,
          bool isTargetSmallEndian) {
    std::ostringstream oss;
    CHelper::BinaryWriter binaryWriter(isTargetSmallEndian, oss);
    T t1 = getInstance();
    binaryWriter.encode(t1);
    std::istringstream iss(oss.str());
    CHelper::BinaryReader binaryReader(isTargetSmallEndian, iss);
    T t2 = binaryReader.read<T>();
    testEqual(t1, t2);
    EXPECT_FALSE(iss.eof());
}

template<class T>
void test(std::function<T()> getInstance,
          std::function<void(const T &, const T &)> testEqual) {
    test(getInstance, testEqual, true);
    test(getInstance, testEqual, false);
}

template<class T>
void testNode(CHelper::CPack &cpack,
              std::function<T()> getInstance,
              std::function<void(const T &, const T &)> testEqual,
              bool isTargetSmallEndian) {
    std::ostringstream oss;
    CHelper::BinaryWriter binaryWriter(isTargetSmallEndian, oss);
    T t1 = getInstance();
    binaryWriter.encode(t1);
    std::istringstream iss(oss.str());
    CHelper::BinaryReader binaryReader(isTargetSmallEndian, iss);
    T t2 = binaryReader.read<T>();
    t2.init(cpack);
    testEqual(t1, t2);
    EXPECT_FALSE(iss.eof());
}

template<class T>
void testNode(CHelper::CPack &cpack,
              std::function<T()> getInstance,
              std::function<void(const T &, const T &)> testEqual) {
    testNode(cpack, getInstance, testEqual, true);
    testNode(cpack, getInstance, testEqual, false);
}

template<class T>
void test(std::vector<std::function<T()>> getInstance,
          std::function<void(const T &, const T &)> testEqual) {
    for (const auto &item: getInstance) {
        test(item, testEqual);
    }
}

template<class T>
void testNode(CHelper::CPack &cpack,
              std::vector<std::function<T()>> getInstance,
              std::function<void(const T &, const T &)> testEqual) {
    for (const auto &item: getInstance) {
        testNode(cpack, item, testEqual);
    }
}

TEST(BinaryUtilTest, String) {
    auto getInstance1 = []() { return ""; };
    auto getInstance2 = []() { return "Yancey"; };
    auto testEqual = [](const auto &t1, const auto &t2) -> void {
        EXPECT_EQ(t1, t2);
    };
    test<std::string>({getInstance1, getInstance2}, testEqual);
}

TEST(BinaryUtilTest, OptioanlString) {
    auto getInstance1 = []() { return ""; };
    auto getInstance2 = []() { return "Yancey"; };
    auto getInstance3 = []() { return std::nullopt; };
    auto testEqual = [](const auto &t1, const auto &t2) -> void {
        EXPECT_EQ(t1, t2);
    };
    test<std::optional<std::string>>(
            {getInstance1, getInstance2, getInstance3},
            testEqual);
}

TEST(BinaryUtilTest, Manifest) {
    auto getInstance1 = []() -> CHelper::Manifest {
        return {"name", "description",
                "minecraftVersion", "author",
                "updateDate", "packId",
                1, true, true};
    };
    auto getInstance2 = []() -> CHelper::Manifest {
        return {"name", std::nullopt,
                "minecraftVersion", "author",
                "updateDate", "packId",
                2, std::nullopt, true};
    };
    auto getInstance3 = []() -> CHelper::Manifest {
        return {"name", "description",
                std::nullopt, "author",
                "updateDate", "packId",
                3, false, std::nullopt};
    };
    auto getInstance4 = []() -> CHelper::Manifest {
        return {std::nullopt, std::nullopt,
                std::nullopt, "author",
                "updateDate", "packId",
                5, std::nullopt, std::nullopt};
    };
    auto testEqual = [](const auto &t1, const auto &t2) -> void {
        EXPECT_EQ(t1.name, t2.name);
        EXPECT_EQ(t1.description, t2.description);
        EXPECT_EQ(t1.minecraftVersion, t2.minecraftVersion);
        EXPECT_EQ(t1.author, t2.author);
        EXPECT_EQ(t1.updateDate, t2.updateDate);
        EXPECT_EQ(t1.packId, t2.packId);
        EXPECT_EQ(t1.versionCode, t2.versionCode);
        EXPECT_EQ(t1.isBasicPack, t2.isBasicPack);
        EXPECT_EQ(t1.isDefault, t2.isDefault);
    };
    test<CHelper::Manifest>({getInstance1, getInstance2, getInstance3, getInstance4}, testEqual);
}

TEST(BinaryUtilTest, NormalId) {
    auto getInstance1 = []() { return CHelper::NormalId::make("name", "description"); };
    auto getInstance2 = []() { return CHelper::NormalId::make("name", ""); };
    auto getInstance3 = []() { return CHelper::NormalId::make("name", std::nullopt); };
    auto testEqual = [](const auto &t1, const auto &t2) -> void {
        EXPECT_EQ(t1->name, t2->name);
        EXPECT_EQ(t1->description, t2->description);
    };
    test<std::shared_ptr<CHelper::NormalId>>({getInstance1, getInstance2, getInstance3}, testEqual);
}

TEST(BinaryUtilTest, NamespaceId) {
    auto testEqual = [](const auto &t1, const auto &t2) -> void {
        EXPECT_EQ(t1.name, t2.name);
        EXPECT_EQ(t1.description, t2.description);
        EXPECT_EQ(t1.idNamespace, t2.idNamespace);
    };
    std::filesystem::path projectDir(PROJECT_DIR);
    nlohmann::json j = CHelper::JsonUtil::getJsonFromFile(projectDir / "resources" / "beta" / "vanilla" / "id" / "entities.json");
    std::vector<std::function<CHelper::NamespaceId()>> getInstance;
    for (const auto &item: j.at("content")) {
        CHelper::NamespaceId namespaceId = item;
        getInstance.emplace_back([namespaceId]() { return namespaceId; });
    }
    test<CHelper::NamespaceId>(getInstance, testEqual);
}

TEST(BinaryUtilTest, ItemId) {
    auto testEqual = [](const auto &t1, const auto &t2) -> void {
        EXPECT_EQ(t1.name, t2.name);
        EXPECT_EQ(t1.description, t2.description);
        EXPECT_EQ(t1.idNamespace, t2.idNamespace);
        EXPECT_EQ(t1.max, t2.max);
        EXPECT_EQ(t1.descriptions, t2.descriptions);
    };
    std::filesystem::path projectDir(PROJECT_DIR);
    nlohmann::json j = CHelper::JsonUtil::getJsonFromFile(projectDir / "resources" / "beta" / "vanilla" / "id" / "items.json");
    std::vector<std::function<CHelper::ItemId()>> getInstance;
    for (const auto &item: j.at("items")) {
        CHelper::ItemId itemId(item);
        getInstance.emplace_back([itemId]() { return itemId; });
    }
    test<CHelper::ItemId>(getInstance, testEqual);
}

TEST(BinaryUtilTest, BlockId) {
    auto testEqual = [](const CHelper::BlockId &t1, const CHelper::BlockId &t2) -> void {
        EXPECT_EQ(t1.name, t2.name);
        EXPECT_EQ(t1.description, t2.description);
        EXPECT_EQ(t1.idNamespace, t2.idNamespace);
        ASSERT_EQ(!t1.blockStates.has_value() || t1.blockStates.value().empty(), !t2.blockStates.has_value());
        if (HEDLEY_LIKELY(t2.blockStates.has_value())) {
            ASSERT_EQ(t1.blockStates->size(), t2.blockStates->size());
            for (int i = 0; i < t2.blockStates->size(); ++i) {
                EXPECT_EQ(t1.blockStates.value()[i].key, t2.blockStates.value()[i].key);
                EXPECT_EQ(t1.blockStates.value()[i].description, t2.blockStates.value()[i].description);
                ASSERT_EQ(t1.blockStates.value()[i].values.size(), t2.blockStates.value()[i].values.size());
                for (int j = 0; j < t1.blockStates.value()[i].values.size(); ++j) {
                    EXPECT_EQ(t1.blockStates.value()[i].values[j].description, t2.blockStates.value()[i].values[j].description);
                    EXPECT_EQ((uint8_t) t1.blockStates.value()[i].values[j].type, (uint8_t) t2.blockStates.value()[i].values[j].type);
                    EXPECT_EQ(t1.blockStates.value()[i].values[j].value, t2.blockStates.value()[i].values[j].value);
                }
                EXPECT_EQ(t1.blockStates.value()[i].defaultValue, t2.blockStates.value()[i].defaultValue);
            }
        }
    };
    std::filesystem::path projectDir(PROJECT_DIR);
    nlohmann::json j = CHelper::JsonUtil::getJsonFromFile(projectDir / "resources" / "beta" / "vanilla" / "id" / "blocks.json");
    std::vector<std::function<CHelper::BlockId()>> getInstance;
    for (const auto &item: j.at("blocks")) {
        CHelper::BlockId blockId(item);
        getInstance.emplace_back([blockId]() { return blockId; });
    }
    test<CHelper::BlockId>(getInstance, testEqual);
}

TEST(BinaryUtilTest, PerCPackNormalIds) {

    std::unique_ptr<CHelper::CPack> cpack;
    try {
        std::filesystem::path projectDir(PROJECT_DIR);
        nlohmann::json j = CHelper::JsonUtil::getJsonFromFile(projectDir / "resources" / "beta" / "vanilla" / "id" / "blocks.json");
#ifdef _WIN32
        cpack = CHelper::CPack::createByDirectory(std::filesystem::path(
                projectDir / "resources" / "beta" / "vanilla"));
#else
        cpack = CHelper::CPack::createByDirectory(std::filesystem::path(
                R"(/home/yancey/CLionProjects/CHelper-Core/resources/beta/vanilla)"));
#endif
    } catch (const std::exception &e) {
        CHelper::Exception::printStackTrace(e);
        CHelper::Profile::clear();
        exit(-1);
    }
    auto testEqual = [](const auto &t1, const auto &t2) -> void {
        ASSERT_EQ(t1->size(), t2->size());
        for (int i = 0; i < t1->size(); ++i) {
            EXPECT_EQ(t1->at(i)->name, t2->at(i)->name);
            EXPECT_EQ(t1->at(i)->description, t2->at(i)->description);
        }
    };
    std::vector<std::function<std::shared_ptr<std::vector<std::shared_ptr<CHelper::NormalId>>>()>> getInstance;
    for (const auto &item: cpack->normalIds) {
        const auto &ids = item.second;
        getInstance.emplace_back([&ids]() { return ids; });
    }
    test<std::shared_ptr<std::vector<std::shared_ptr<CHelper::NormalId>>>>(getInstance, testEqual);
}

TEST(BinaryUtilTest, CPackNormalIds) {
    std::unique_ptr<CHelper::CPack> cpack;
    try {
        std::filesystem::path projectDir(PROJECT_DIR);
        cpack = CHelper::CPack::createByDirectory(projectDir / "resources" / "beta" / "vanilla");
    } catch (const std::exception &e) {
        CHelper::Exception::printStackTrace(e);
        CHelper::Profile::clear();
        exit(-1);
    }
    auto testEqual = [](
                             const std::unordered_map<std::string, std::shared_ptr<std::vector<std::shared_ptr<CHelper::NormalId>>>> &t1,
                             const std::unordered_map<std::string, std::shared_ptr<std::vector<std::shared_ptr<CHelper::NormalId>>>> &t2) -> void {
        ASSERT_EQ(t1.size(), t2.size());
        auto it1 = t1.begin();
        while (it1 != t1.end()) {
            auto it2 = t2.find(it1->first);
            ASSERT_TRUE(it2 != t2.end());
            EXPECT_EQ(it1->first, it2->first);
            ASSERT_EQ(it1->second->size(), it2->second->size());
            for (int i = 0; i < it1->second->size(); ++i) {
                EXPECT_EQ(it1->second->at(i)->name, it2->second->at(i)->name);
                EXPECT_EQ(it1->second->at(i)->description, it2->second->at(i)->description);
            }
            it1++;
        }
    };
    test<std::unordered_map<std::string, std::shared_ptr<std::vector<std::shared_ptr<CHelper::NormalId>>>>>(
            [&cpack]() { return cpack->normalIds; }, testEqual);
}

TEST(BinaryUtilTest, CPackNamespaceId) {
    std::unique_ptr<CHelper::CPack> cpack;
    try {
        std::filesystem::path projectDir(PROJECT_DIR);
        cpack = CHelper::CPack::createByDirectory(projectDir / "resources" / "beta" / "vanilla");
    } catch (const std::exception &e) {
        CHelper::Exception::printStackTrace(e);
        CHelper::Profile::clear();
        exit(-1);
    }
    auto testEqual = [](
                             const std::unordered_map<std::string, std::shared_ptr<std::vector<std::shared_ptr<CHelper::NamespaceId>>>> &t1,
                             const std::unordered_map<std::string, std::shared_ptr<std::vector<std::shared_ptr<CHelper::NamespaceId>>>> &t2) -> void {
        ASSERT_EQ(t1.size(), t2.size());
        auto it1 = t1.begin();
        while (it1 != t1.end()) {
            auto it2 = t2.find(it1->first);
            ASSERT_TRUE(it2 != t2.end());
            EXPECT_EQ(it1->first, it2->first);
            ASSERT_EQ(it1->second->size(), it2->second->size());
            for (int i = 0; i < it1->second->size(); ++i) {
                EXPECT_EQ(it1->second->at(i)->name, it2->second->at(i)->name);
                EXPECT_EQ(it1->second->at(i)->description, it2->second->at(i)->description);
                EXPECT_EQ(it1->second->at(i)->idNamespace, it2->second->at(i)->idNamespace);
            }
            it1++;
        }
    };
    test<std::unordered_map<std::string, std::shared_ptr<std::vector<std::shared_ptr<CHelper::NamespaceId>>>>>(
            [&cpack]() { return cpack->namespaceIds; }, testEqual);
}

TEST(BinaryUtilTest, NodeJsonElement) {
    std::unique_ptr<CHelper::CPack> cpack;
    std::filesystem::path projectDir(PROJECT_DIR);
    try {
        cpack = CHelper::CPack::createByDirectory(projectDir / "resources" / "beta" / "vanilla");
    } catch (const std::exception &e) {
        CHelper::Exception::printStackTrace(e);
        CHelper::Profile::clear();
        exit(-1);
    }
    auto testEqual = [](
                             const CHelper::Node::NodeJsonBoolean &t1,
                             const CHelper::Node::NodeJsonBoolean &t2) -> void {
        EXPECT_EQ(t1.id, t2.id);
        EXPECT_EQ(t1.brief, t2.brief);
        EXPECT_EQ(t1.description, t2.description);
        EXPECT_EQ(t1.isMustAfterWhiteSpace, t2.isMustAfterWhiteSpace);
        EXPECT_EQ(t1.descriptionTrue, t2.descriptionTrue);
        EXPECT_EQ(t1.descriptionFalse, t2.descriptionFalse);
    };
    CHelper::Node::NodeJsonBoolean node(
            "ID", "description",
            "descriptionTrue", "descriptionFalse");
    testNode<CHelper::Node::NodeJsonBoolean>(
            *cpack,
            [&node]() { return node; },
            testEqual);
}

TEST(BinaryUtilTest, NodeJsonInteger) {
    std::unique_ptr<CHelper::CPack> cpack;
    try {
        std::filesystem::path projectDir(PROJECT_DIR);
        cpack = CHelper::CPack::createByDirectory(projectDir / "resources" / "beta" / "vanilla");
    } catch (const std::exception &e) {
        CHelper::Exception::printStackTrace(e);
        CHelper::Profile::clear();
        exit(-1);
    }
    auto testEqual = [](
                             const CHelper::Node::NodeJsonInteger &t1,
                             const CHelper::Node::NodeJsonInteger &t2) -> void {
        EXPECT_EQ(t1.id, t2.id);
        EXPECT_EQ(t1.brief, t2.brief);
        EXPECT_EQ(t1.description, t2.description);
        EXPECT_EQ(t1.isMustAfterWhiteSpace, t2.isMustAfterWhiteSpace);
        EXPECT_EQ(t1.min, t2.min);
        EXPECT_EQ(t1.max, t2.max);
    };
    testNode<CHelper::Node::NodeJsonInteger>(
            *cpack,
            {
                    []() { return CHelper::Node::NodeJsonInteger{
                                   "ID", "description",
                                   0, 3}; },
                    []() { return CHelper::Node::NodeJsonInteger{
                                   "ID", "description",
                                   std::nullopt, 3}; },
                    []() { return CHelper::Node::NodeJsonInteger{
                                   "ID", "description",
                                   1, std::nullopt}; },
                    []() { return CHelper::Node::NodeJsonInteger{
                                   "ID", "description",
                                   std::nullopt, std::nullopt}; },
            },
            testEqual);
}

TEST(BinaryUtilTest, NodeJsonFloat) {
    std::unique_ptr<CHelper::CPack> cpack;
    try {
        std::filesystem::path projectDir(PROJECT_DIR);
        cpack = CHelper::CPack::createByDirectory(projectDir / "resources" / "beta" / "vanilla");
    } catch (const std::exception &e) {
        CHelper::Exception::printStackTrace(e);
        CHelper::Profile::clear();
        exit(-1);
    }
    auto testEqual = [](
                             const CHelper::Node::NodeJsonFloat &t1,
                             const CHelper::Node::NodeJsonFloat &t2) -> void {
        EXPECT_EQ(t1.id, t2.id);
        EXPECT_EQ(t1.brief, t2.brief);
        EXPECT_EQ(t1.description, t2.description);
        EXPECT_EQ(t1.isMustAfterWhiteSpace, t2.isMustAfterWhiteSpace);
        EXPECT_EQ(t1.min, t2.min);
        EXPECT_EQ(t1.max, t2.max);
    };
    testNode<CHelper::Node::NodeJsonFloat>(
            *cpack,
            {
                    []() { return CHelper::Node::NodeJsonFloat{
                                   "ID", "description",
                                   0, 3}; },
                    []() { return CHelper::Node::NodeJsonFloat{
                                   "ID", "description",
                                   std::nullopt, 3}; },
                    []() { return CHelper::Node::NodeJsonFloat{
                                   "ID", "description",
                                   1, std::nullopt}; },
                    []() { return CHelper::Node::NodeJsonFloat{
                                   "ID", "description",
                                   std::nullopt, std::nullopt}; },
            },
            testEqual);
}

TEST(BinaryUtilTest, NodeJsonNull) {
    std::unique_ptr<CHelper::CPack> cpack;
    try {
        std::filesystem::path projectDir(PROJECT_DIR);
        cpack = CHelper::CPack::createByDirectory(projectDir / "resources" / "beta" / "vanilla");
    } catch (const std::exception &e) {
        CHelper::Exception::printStackTrace(e);
        CHelper::Profile::clear();
        exit(-1);
    }
    auto testEqual = [](
                             const CHelper::Node::NodeJsonNull &t1,
                             const CHelper::Node::NodeJsonNull &t2) -> void {
        EXPECT_EQ(t1.id, t2.id);
        EXPECT_EQ(t1.brief, t2.brief);
        EXPECT_EQ(t1.description, t2.description);
        EXPECT_EQ(t1.isMustAfterWhiteSpace, t2.isMustAfterWhiteSpace);
    };
    testNode<CHelper::Node::NodeJsonNull>(
            *cpack,
            []() { return CHelper::Node::NodeJsonNull{
                           "ID", "description"}; },
            testEqual);
}
