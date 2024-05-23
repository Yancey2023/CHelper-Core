//
// Created by Yancey on 24-5-21.
//

#include <gtest/gtest.h>

#include "../chelper/lexer/Lexer.h"
#include "../chelper/Core.h"
#include "../chelper/parser/Parser.h"

#pragma GCC diagnostic ignored "-Wunused-result"

namespace CHelper::Test {

    /**
     * 测试程序是否可以正常运行
     */
    [[maybe_unused]] void
    test(const std::string &cpackPath, const std::vector<std::string> &commands) {
        std::shared_ptr<Core> core;
        try {
            std::unique_ptr<CPack> cPack = CPack::createByDirectory(cpackPath);
            ASTNode astNode = Parser::parse("", cPack.get());
            core = std::make_shared<Core>(std::move(cPack), std::move(astNode));
        } catch (const std::exception &e) {
            Exception::printStackTrace(e);
            Profile::clear();
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
                std::cout << ColorStringBuilder()
                        .green("parse command: ")
                        .purple(command)
                        .build() << std::endl;
                Exception::printStackTrace(e);
                Profile::clear();
                flag = true;
            }
        }
        if (flag) {
            FAIL();
        }
    }

    void writeBson(const std::string &input, const std::string &output) {
        try {
            auto core = Core::createByDirectory(input);
            std::cout << std::endl;
            if (core == nullptr) {
                return;
            }
            std::chrono::high_resolution_clock::time_point start, end;
            start = std::chrono::high_resolution_clock::now();
            core->getCPack()->writeBsonToFile(output);
            end = std::chrono::high_resolution_clock::now();
            std::cout << ColorStringBuilder()
                    .green("write successfully(")
                    .purple(std::to_string(std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
                            end - start).count()) + "ms")
                    .green(")")
                    .build() << std::endl;
            std::cout << std::endl;
            auto core2 = Core::createByBson(output);
            std::cout << std::endl;
        } catch (const std::exception &e) {
            Exception::printStackTrace(e);
            Profile::clear();
            throw e;
        }
    }

} // CHelper::Test

TEST(MainTest, ParseCommand) {
    CHelper::Test::test(
            R"(D:\CLion\project\CHelper-Core\resources\beta\vanilla)",
            std::vector<std::string>{
                    R"(execute run clear )",
                    R"(give @s[hasitem=[{item=air,data=1},{item=minecraft:bed}],has_property={minecraft:is_rolled_up=true,m)",
                    R"(give @s command_block 112 12 {"minecraft:can_place_on":{"blocks":[")",
                    R"(tellraw @a {"rawtext":[{"text":"aaa","selector":"@a[type=\")",
                    R"(execute if block 12~23~)",
                    R"(give @s stone 12 21 {"minecraft:item_lock":{"mode":"l)",
                    R"(execute if block ~~~ anvil["aaa"=90.5] run g)",
                    R"(setblock ~~~ stone[)",
                    R"(give @s stone 12 1)",
                    R"(tag @s add "\\\"\u1110\/\b\f\n\r\t\p\")",
                    R"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":["minecraft:acacia_door"]}})",
                    R"(give @s s)",
                    R"(give @s stone 1 1 {)",
                    R"(give @s 石头)",
                    R"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":[")",
                    R"(give)",
                    R"(give @)",
                    R"(give @a[x=^,has_property={""=!..12,="..}})",
                    R"(execute as @a run)",
                    R"(execute run)",
                    R"(execute if block ~~~ command_block run)",
                    R"(execute if block ~~~ bamboo)",
                    R"(give @s apple 12 1)",
                    R"(spreadplayers ~ ~ 0 1200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000)",
                    R"(camerashake add @a 10000000000000000000000000000000000000000000000000000 3402823466385288598117041834845169254401)",
                    R"(setblock ~~~ candle_cake[lit=)",
            }
    );
}

TEST(MainTest, Bson) {
    // release
    CHelper::Test::writeBson(R"(D:\CLion\project\CHelper-Core\resources\release\vanilla)",
                             R"(D:\CLion\project\CHelper-Core\run\release-vanilla-1.20.80.05.cpack)");
    CHelper::Test::writeBson(R"(D:\CLion\project\CHelper-Core\resources\release\experiment)",
                             R"(D:\CLion\project\CHelper-Core\run\release-experiment-1.20.80.05.cpack)");
    // beta
    CHelper::Test::writeBson(R"(D:\CLion\project\CHelper-Core\resources\beta\vanilla)",
                             R"(D:\CLion\project\CHelper-Core\run\beta-vanilla-1.21.0.23.cpack)");
    CHelper::Test::writeBson(R"(D:\CLion\project\CHelper-Core\resources\beta\experiment)",
                             R"(D:\CLion\project\CHelper-Core\run\beta-experiment-1.21.0.23.cpack)");
    // netease
//    CHelper::Test::writeBson(R"(D:\CLion\project\CHelper-Core\resources\netease\vanilla)",
//                             R"(D:\CLion\project\CHelper-Core\run\netease-1.21.0.23.cpack)");
//    CHelper::Test::writeBson(R"(D:\CLion\project\CHelper-Core\resources\netease\experiment)",
//                             R"(D:\CLion\project\CHelper-Core\run\netease-1.21.0.23.cpack)");
}
