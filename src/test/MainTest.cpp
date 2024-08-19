//
// Created by Yancey on2024-5-21.
//

#include <gtest/gtest.h>

#include "../chelper/Core.h"
#include "../chelper/parser/Parser.h"
#include "param_deliver.h"
#include <codecvt>
#include <locale>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

// these two method is slow and codecvt is deprecated in c++17
// do not use this implementation in your project.
// you should implement it depend on your platform, such as use Windows API.

static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;

std::string wstring2string(const std::wstring &wstring) {
    return utf8_conv.to_bytes(wstring);
}

std::wstring string2wstring(const std::string &string) {
    return utf8_conv.from_bytes(string);
}

#pragma clang diagnostic pop

#pragma GCC diagnostic ignored "-Wunused-result"

namespace CHelper::Test {

    /**
     * 测试程序是否可以正常运行
     */
    [[maybe_unused]] void
    test(const std::filesystem::path &cpackPath, const std::vector<std::wstring> &commands) {
        std::shared_ptr<Core> core;
        try {
            std::unique_ptr<CPack> cPack = CPack::createByDirectory(cpackPath);
            ASTNode astNode = Parser::parse(L"", cPack.get());
            core = std::make_shared<Core>(std::move(cPack), std::move(astNode));
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            FAIL();
        }
        bool flag = false;
        for (const auto &command: commands) {
            try {
                core->onTextChanged(command, command.length());
                core->getDescription();
                core->getSuggestions();
                core->getSuggestions();
                core->getStructure();
            } catch (const std::exception &e) {
                CHELPER_INFO(L"parse command: {}", command);
                CHelper::Profile::printAndClear(e);
                flag = true;
            }
        }
        if (HEDLEY_UNLIKELY(flag)) {
            FAIL();
        }
    }

    void writeBson(const std::filesystem::path &input, const std::filesystem::path &output) {
        try {
            auto core = Core::createByDirectory(input);
            std::cout << std::endl;
            if (HEDLEY_UNLIKELY(core == nullptr)) {
                return;
            }
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            core->getCPack()->writeBsonToFile(output);
            end = std::chrono::high_resolution_clock::now();
            CHELPER_INFO(L"write successfully({})", std::to_wstring(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start).count()) + L"ms");
            std::cout << std::endl;
            [[maybe_unused]] auto core2 = Core::createByBson(output);
            std::cout << std::endl;
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            throw e;
        }
    }

}// namespace CHelper::Test

TEST(MainTest, ParseCommand) {
    std::locale::global(std::locale("zh_cn.UTF-8"));
    std::filesystem::path projectDir(PROJECT_DIR);
    CHelper::Test::test(
            projectDir / L"resources" / L"beta" / L"vanilla",
            std::vector<std::wstring>{
                    LR"(execute run clear )",
                    LR"(give @s[hasitem=[{item=air,data=1},{item=minecraft:bed}],has_property={minecraft:is_rolled_up=true,m)",
                    LR"(give @s command_block 112 12 {"minecraft:can_place_on":{"blocks":[")",
                    LR"(tellraw @a {"rawtext":[{"text":"aaa","selector":"@a[type=\")",
                    LR"(execute if block 12~23~)",
                    LR"(give @s stone 12 21 {"minecraft:item_lock":{"mode":"l)",
                    LR"(execute if block ~~~ anvil["aaa"=90.5] run g)",
                    LR"(setblock ~~~ stone[)",
                    LR"(give @s stone 12 1)",
                    LR"(tag @s add "\\\"\u1110\/\b\f\n\r\t\p\")",
                    LR"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":["minecraft:acacia_door"]}})",
                    LR"(give @s s)",
                    LR"(give @s stone 1 1 {)",
                    LR"(give @s 石头)",
                    LR"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":[")",
                    LR"(give)",
                    LR"(give @)",
                    LR"(give @a[x=^,has_property={""=!..12,="..}})",
                    LR"(execute as @a run)",
                    LR"(execute run)",
                    LR"(execute if block ~~~ command_block run)",
                    LR"(execute if block ~~~ bamboo)",
                    LR"(give @s apple 12 1)",
                    LR"(spreadplayers ~ ~ 0 1200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000)",
                    LR"(camerashake add @a 10000000000000000000000000000000000000000000000000000 3402823466385288598117041834845169254401)",
                    LR"(setblock ~~~ candle_cake[lit=)",
                    LR"(give @s repeating_command_block)",
            });
}