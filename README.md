# CHelper

I want to make a command helper for Minecraft Bedrock Edition , so here is CHelper, which means Command Helper.

## Plan

- [x] **CPack**: makes a resource pack containing IDs and commands
- [x] **Lexer**: transforms command strings into a token list
- [x] **Parser**: builds an Abstract Syntax Tree (AST) and identifies structural errors
- [x] **ID Error**: finds ID errors based on the AST
- [x] **Description**: get the description of the currently written command param based on the AST
- [x] **Suggestion**: provides completion suggestions based on the AST
- [x] **Structure**: constructs a command structure string based on the AST
- [x] **Target Selector Support**: target selector command param
- [x] **Json Support**: json command param
- [x] **Block State Support**: block states command param
- [x] **Highlight Command**: obtains the highlight color for displayed commands

## Why choosing c++

Because I am leaning c++ now. I want to improve myself through this project.

## Third-party

### CHelper-Core

|                  project                  |                                    description                                     |                                           license                                           |
|:-----------------------------------------:|:----------------------------------------------------------------------------------:|:-------------------------------------------------------------------------------------------:|
| [json](https://github.com/nlohmann/json)  |                                JSON for Modern C++                                 |           [MIT license](https://github.com/nlohmann/json/blob/master/LICENSE.MIT)           |
|   [fmt](https://github.com/fmtlib/fmt)    |                                 formatting library                                 |              [MIT license](https://github.com/fmtlib/fmt/blob/master/LICENSE)               |
| [Hedley](https://github.com/cblck/hedley) | enable compiler-specific features while retaining compatibility with all compilers | [Creative Commons Zero v1.0 Universal](https://github.com/cblck/hedley/blob/master/LICENSE) |

### CHelper-Qt

|         project          |          description          |                     license                      |
|:------------------------:|:-----------------------------:|:------------------------------------------------:|
| [Qt](https://www.qt.io/) | desktop application framework | [LGPL license](https://doc.qt.io/qt-6/lgpl.html) |

### CHelper-Android

|                 project                  |          description           |                           license                            |
|:----------------------------------------:|:------------------------------:|:------------------------------------------------------------:|
| [NDK](https://developer.android.com/ndk) | android native development kit | [Apache License](http://www.apache.org/licenses/LICENSE-2.0) |

### CHelper-Web

|               project                |                description                 |                                                                  license                                                                   |
|:------------------------------------:|:------------------------------------------:|:------------------------------------------------------------------------------------------------------------------------------------------:|
| [Emscripten](https://emscripten.org) | complete compiler toolchain to WebAssembly | [the MIT license and the University of Illinois/NCSA Open Source License](https://github.com/emscripten-core/emscripten/blob/main/LICENSE) |

### CHelper-Test

|                      project                       |         description         |                                              license                                              |
|:--------------------------------------------------:|:---------------------------:|:-------------------------------------------------------------------------------------------------:|
| [GoogleTest](https://github.com/google/googletest) | Google's C++ test framework | [BSD 3-Clause "New" or "Revised" License](https://github.com/google/googletest/blob/main/LICENSE) |

## CPack Document

Cpack(Command Pack) is the resource pack of CHelper(Command Helper).

[English](doc/README.md)

[Chinese](doc/README_CN.md)

## Special Thanks

|                       project                       |                        description                         |                                   license                                    |
|:---------------------------------------------------:|:----------------------------------------------------------:|:----------------------------------------------------------------------------:|
|  [caidlist](https://github.com/XeroAlpha/caidlist)  | provide IDs and descriptions for Minecraft Bedrock Edition | [GPL-3.0 license](https://github.com/XeroAlpha/caidlist/blob/master/LICENSE) |
| [chinese minecraft wiki](https://zh.minecraft.wiki) |              provide information in Minecraft              |     [CC BY-NC-SA 3.0](https://creativecommons.org/licenses/by-nc-sa/3.0)     |

- JetBrains License for Open Source Development

  ![JetBrains Logo (Main) logo](https://resources.jetbrains.com/storage/products/company/brand/logos/jb_beam.svg)
