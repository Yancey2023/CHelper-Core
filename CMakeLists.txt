cmake_minimum_required(VERSION 3.26)
project(CHelper)

#set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps=obj")
#set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -save-temps=obj")

include_directories(include)

add_executable(CHelper
        main.cpp
        main.h
        core/lexer/Token.cpp
        core/lexer/Token.h
        core/lexer/Pos.cpp
        core/lexer/Pos.h
        core/lexer/Lexer.cpp
        core/lexer/Lexer.h
        core/lexer/StringReader.cpp
        core/lexer/StringReader.h
        core/parser/Parser.cpp
        core/parser/Parser.h
        core/resources/Manifest.cpp
        core/resources/Manifest.h
        core/resources/CPack.cpp
        core/resources/CPack.h
        core/resources/id/NormalId.cpp
        core/resources/id/NormalId.h
        core/util/JsonUtil.cpp
        core/util/JsonUtil.h
        core/resources/id/BlockId.cpp
        core/resources/id/BlockId.h
        core/resources/id/ItemId.cpp
        core/resources/id/ItemId.h
        core/resources/command/Command.cpp
        core/resources/command/Command.h
        core/resources/command/node/NodeBase.cpp
        core/resources/command/node/NodeBase.h
        core/resources/command/node/NodeInteger.cpp
        core/resources/command/node/NodeInteger.h
        core/resources/command/node/NodeFloat.cpp
        core/resources/command/node/NodeFloat.h
        core/resources/command/node/NodeBlock.cpp
        core/resources/command/node/NodeBlock.h
        core/resources/command/node/NodeBoolean.cpp
        core/resources/command/node/NodeBoolean.h
        core/resources/command/node/NodeNormalId.cpp
        core/resources/command/node/NodeNormalId.h
        core/resources/command/node/NodeCommand.cpp
        core/resources/command/node/NodeCommand.h
        core/resources/command/node/NodeCommandName.cpp
        core/resources/command/node/NodeCommandName.h
        core/resources/id/NamespaceId.cpp
        core/resources/id/NamespaceId.h
        core/resources/command/node/NodeNamespaceId.cpp
        core/resources/command/node/NodeNamespaceId.h
        core/util/HashUtil.cpp
        core/util/HashUtil.h
        core/resources/command/node/NodeLF.cpp
        core/resources/command/node/NodeLF.h
        include/pch.h
        core/util/CPackUtil.cpp
        core/util/CPackUtil.h
        core/util/Exception.cpp
        core/util/Exception.h
        core/util/SimpleLogger.cpp
        core/util/SimpleLogger.h
        core/resources/command/node/NodeTargetSelector.cpp
        core/resources/command/node/NodeTargetSelector.h
        core/resources/command/node/NodeText.cpp
        core/resources/command/node/NodeText.h
        core/resources/command/node/NodeItem.cpp
        core/resources/command/node/NodeItem.h
        core/resources/command/node/NodePosition.cpp
        core/resources/command/node/NodePosition.h
        core/resources/command/node/NodeString.cpp
        core/resources/command/node/NodeString.h
        core/resources/command/type/NodeType.cpp
        core/resources/command/type/NodeType.h
)