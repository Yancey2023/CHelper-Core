//
// Created by Yancey on 2024-5-21.
//

#include <chelper/CHelperCore.h>
#include <chelper/parser/Parser.h>
#include <gtest/gtest.h>

namespace CHelper::Test {

#pragma warning(disable : 4068)
#pragma warning(disable : 4834)

    std::ostream &operator<<(std::ostream &os, const CHelper::TokenType::TokenType &tokenType) {
        switch (tokenType) {
            case CHelper::TokenType::NUMBER:
                os << "NUMBER";
                break;
            case CHelper::TokenType::STRING:
                os << "STRING";
                break;
            case CHelper::TokenType::SYMBOL:
                os << "SYMBOL";
                break;
            case CHelper::TokenType::WHITE_SPACE:
                os << "WHITE_SPACE";
                break;
            default:
                os << "UNKNOWN";
                break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const CHelper::Token &token) {
        return os << '['
                  << token.type
                  << "] "
                  << token.pos
                  << ' '
                  << utf8::utf16to8(token.content);
    }

    /**
     * 测试程序是否可以正常运行
     */
    [[maybe_unused]] void testLex(const std::vector<std::u16string> &commands) {
        try {
            for (const auto &command: commands) {
                CHELPER_INFO("lex command: {}", command);
                for (const auto &item: Lexer::lex(command).allTokens) {
                    std::cout << item << std::endl;
                }
            }
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            FAIL();
        }
    }

    /**
     * 测试程序是否可以正常运行
     */
    [[maybe_unused]] void testParse(const std::filesystem::path &cpackPath,
                                    const std::vector<std::u16string> &commands) {
        std::shared_ptr<CHelperCore> core;
        try {
            std::unique_ptr<CPack> cPack = CPack::createByDirectory(cpackPath);
            ASTNode astNode = Parser::parse(u"", cPack.get());
            core = std::make_shared<CHelperCore>(std::move(cPack), std::move(astNode));
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            FAIL();
        }
        bool flag = false;
        for (const auto &command: commands) {
            try {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-result"
                core->onTextChanged(command, command.length());
                core->getDescription();
                core->getSuggestions();
                core->getSuggestions();
                core->getStructure();
#pragma clang diagnostic pop
#pragma GCC diagnostic pop
            } catch (const std::exception &e) {
                CHELPER_INFO("parse command: {}", command);
                CHelper::Profile::printAndClear(e);
                flag = true;
            }
        }
        if (HEDLEY_UNLIKELY(flag)) {
            FAIL();
        }
    }

}// namespace CHelper::Test

TEST(MainTest, LexCommand) {
    CHelper::Test::testLex(
            std::vector<std::u16string>{
                    uR"(execute run clear )",
                    uR"(give @s[hasitem=[{item=air,data=1},{item=minecraft:bed}],has_property={minecraft:is_rolled_up=true,m)",
                    uR"(give @s command_block 112 12 {"minecraft:can_place_on":{"blocks":[")",
                    uR"(tellraw @a {"rawtext":[{"text":"aaa","selector":"@a[type=\")",
                    uR"(execute if block 12~23~)",
                    uR"(give @s stone 12 21 {"minecraft:item_lock":{"mode":"l)",
                    uR"(execute if block ~~~ anvil["aaa"=90.5] run g)",
                    uR"(setblock ~~~ stone[)",
                    uR"(give @s stone 12 1)",
                    uR"(tag @s add "\\\"\u1110\/\b\f\n\r\t\p\")",
                    uR"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":["minecraft:acacia_door"]}})",
                    uR"(give @s s)",
                    uR"(give @s stone 1 1 {)",
                    uR"(give @s 石头)",
                    uR"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":[")",
                    uR"(give)",
                    uR"(give @)",
                    uR"(give @a[x=^,has_property={""=!..12,="..}})",
                    uR"(execute as @a run)",
                    uR"(execute run)",
                    uR"(execute if block ~~~ command_block run)",
                    uR"(execute if block ~~~ bamboo)",
                    uR"(give @s apple 12 1)",
                    uR"(spreadplayers ~ ~ 0 1200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000)",
                    uR"(camerashake add @a 10000000000000000000000000000000000000000000000000000 3402823466385288598117041834845169254401)",
                    uR"(setblock ~~~ candle_cake[lit=)",
                    uR"(give @s repeating_command_block)",
            });
}

TEST(MainTest, ParseCommand) {
    std::filesystem::path resourceDir(RESOURCE_DIR);
    CHelper::Test::testParse(
            resourceDir / "resources" / "beta" / "vanilla",
            std::vector<std::u16string>{
                    uR"(execute run clear )",
                    uR"(give @s[hasitem=[{item=air,data=1},{item=minecraft:bed}],has_property={minecraft:is_rolled_up=true,m)",
                    uR"(give @s command_block 112 12 {"minecraft:can_place_on":{"blocks":[")",
                    uR"(tellraw @a {"rawtext":[{"text":"aaa","selector":"@a[type=\")",
                    uR"(execute if block 12~23~)",
                    uR"(give @s stone 12 21 {"minecraft:item_lock":{"mode":"l)",
                    uR"(execute if block ~~~ anvil["aaa"=90.5] run g)",
                    uR"(setblock ~~~ stone[)",
                    uR"(give @s stone 12 1)",
                    uR"(tag @s add "\\\"\u1110\/\b\f\n\r\t\p\")",
                    uR"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":["minecraft:acacia_door"]}})",
                    uR"(give @s s)",
                    uR"(give @s stone 1 1 {)",
                    uR"(give @s 石头)",
                    uR"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":[")",
                    uR"(give)",
                    uR"(give @)",
                    uR"(give @a[x=^,has_property={""=!..12,="..}})",
                    uR"(execute as @a run)",
                    uR"(execute run)",
                    uR"(execute if block ~~~ command_block run)",
                    uR"(execute if block ~~~ bamboo)",
                    uR"(give @s apple 12 1)",
                    uR"(spreadplayers ~ ~ 0 1200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000)",
                    uR"(camerashake add @a 10000000000000000000000000000000000000000000000000000 3402823466385288598117041834845169254401)",
                    uR"(setblock ~~~ candle_cake[lit=)",
                    uR"(give @s repeating_command_block)",
            });
}