//
// Created by Yancey on 2024/2/24.
//

#include <chelper/CHelperCore.h>
#include <chelper/parser/Parser.h>

namespace CHelper {

    CHelperCore::CHelperCore(std::unique_ptr<CPack> cpack, ASTNode astNode)
        : cpack(std::move(cpack)),
          astNode(std::move(astNode)) {}

    CHelperCore *CHelperCore::create(const std::function<std::unique_ptr<CPack>()> &getCPack) {
        try {
#if CHelperOnlyReadBinary != true
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
#endif
            std::unique_ptr<CPack> cPack = getCPack();
#if CHelperOnlyReadBinary != true
            end = std::chrono::high_resolution_clock::now();
            CHELPER_INFO("CPack load successfully ({})", std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) + "ms");
#endif
            ASTNode astNode = Parser::parse(u"", cPack.get());
            return new CHelperCore(std::move(cPack), std::move(astNode));
        } catch (const std::exception &e) {
            CHELPER_ERROR("CPack load failed");
            CHelper::Profile::printAndClear(e);
            return nullptr;
        }
    }

#if CHelperOnlyReadBinary != true
    CHelperCore *CHelperCore::createByDirectory(const std::filesystem::path &cpackPath) {
        return create([&cpackPath]() {
            return CPack::createByDirectory(cpackPath);
        });
    }

    CHelperCore *CHelperCore::createByJson(const std::filesystem::path &cpackPath) {
        return create([&cpackPath]() {
            return CPack::createByJson(JsonUtil::getJsonFromFile(cpackPath));
        });
    }

    CHelperCore *CHelperCore::createByBson(const std::filesystem::path &cpackPath) {
        return create([&cpackPath]() {
            return CPack::createByJson(JsonUtil::getBsonFromFile(cpackPath));
        });
    }

    CHelperCore *CHelperCore::createByBinary(const std::filesystem::path &cpackPath) {
        return create([&cpackPath]() {
            // 检查文件名后缀
            std::string fileType = ".cpack";
            std::string cpackPathStr = cpackPath.string();
            if (HEDLEY_UNLIKELY(cpackPathStr.size() < fileType.size() || cpackPathStr.substr(cpackPathStr.length() - fileType.size()) != fileType)) {
                Profile::push("error file type -> {}", cpackPathStr);
                throw std::runtime_error("error file type");
            }
            // 打开文件
            std::ifstream is(cpackPath, std::ios::binary);
            if (HEDLEY_UNLIKELY(!is.is_open())) {
                Profile::push("fail to read file -> {}", cpackPathStr);
                throw std::runtime_error("fail to read file");
            }
            // 读取文件
            BinaryReader binaryReader(true, is);
            std::unique_ptr<CPack> result = CPack::createByBinary(binaryReader);
            // 检查文件是否读完
            if (HEDLEY_UNLIKELY(!is.eof())) {
                char ch;
                is.read(&ch, 1);
                if (HEDLEY_UNLIKELY(is.gcount() > 0)) {
                    Profile::push("file is not read completed -> {}", cpackPathStr);
                    throw std::runtime_error("file is not read completed");
                }
            }
            // 关闭文件
            is.close();
            return std::move(result);
        });
    }
#endif

    void CHelperCore::onTextChanged(const std::u16string &content, size_t index0) {
        if (HEDLEY_LIKELY(input != content)) {
            input = content;
            astNode = Parser::parse(input, cpack.get());
            suggestions = nullptr;
        }
        onSelectionChanged(index0);
    }

    void CHelperCore::onSelectionChanged(size_t index0) {
        if (HEDLEY_LIKELY(index != index0)) {
            index = index0;
            suggestions = nullptr;
        }
    }

    [[nodiscard]] const CPack *CHelperCore::getCPack() const {
        return cpack.get();
    }

    [[nodiscard]] const ASTNode *CHelperCore::getAstNode() const {
        return &astNode;
    }

    [[nodiscard]] std::u16string CHelperCore::getDescription() const {
        return astNode.getDescription(index);
    }

    [[nodiscard]] std::vector<std::shared_ptr<ErrorReason>> CHelperCore::getErrorReasons() const {
        return astNode.getErrorReasons();
    }

    std::vector<Suggestion> *CHelperCore::getSuggestions() {
        if (HEDLEY_LIKELY(suggestions == nullptr)) {
            suggestions = std::make_shared<std::vector<Suggestion>>(astNode.getSuggestions(index));
        }
        return suggestions.get();
    }

    [[nodiscard]] std::u16string CHelperCore::getStructure() const {
        return astNode.getStructure();
    }

    [[nodiscard]] ColoredString CHelperCore::getColors() const {
        return astNode.getColors(settings.theme);
    }

    std::optional<std::pair<std::u16string, size_t>> CHelperCore::onSuggestionClick(size_t which) {
        if (HEDLEY_UNLIKELY(suggestions == nullptr || which >= suggestions->size())) {
            return std::nullopt;
        }
        return suggestions->at(which).apply(this, astNode.tokens.toString());
    }

    std::u16string CHelperCore::old2new(const Old2New::BlockFixData &blockFixData, const std::u16string &old) {
        return Old2New::old2new(blockFixData, old);
    }

}// namespace CHelper