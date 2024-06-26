# CHelper

I want to make a command helper for Minecraft Bedrock Edition , so here is CHelper, which means Command Helper.

## Plan

- [x] **Lexer**: transforms command strings into a token list
- [x] **CPack**: a resource pack containing IDs and commands
- [x] **Parser**: builds an Abstract Syntax Tree (AST) and identifies structural errors
- [x] **Description**: retrieves the description of the currently written node
- [x] **ID Checker**: finds ID errors based on the AST
- [x] **Suggestion Getter**: provides completion suggestions for IDs based on the AST
- [x] **Structure Retriever**: constructs a command structure string
- [x] **Target Selector**: target selector command param
- [x] **json support**: json command param
- [x] **block state support**: block states command param
- [ ] **highlight command**: obtains the highlight color for displayed commands

## Why choosing c++

Because I am leaning c++ now. I want to improve myself through this project.

## Third-party

### CHelper-Core

|                  project                   |                                    description                                     |                                           license                                            |
|:------------------------------------------:|:----------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------------------:|
| [Hedley](https://github.com/nemequ/hedley) | enable compiler-specific features while retaining compatibility with all compilers | [Creative Commons Zero v1.0 Universal](https://github.com/nemequ/hedley/blob/master/COPYING) |
|  [json](https://github.com/nlohmann/json)  |                                JSON for Modern C++                                 |           [MIT license](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT)           |

### CHelper-Qt

|         project          |          description          |                     license                      |
|:------------------------:|:-----------------------------:|:------------------------------------------------:|
| [Qt](https://www.qt.io/) | desktop application framework | [LGPL license](https://doc.qt.io/qt-5/lgpl.html) |

### CHelper-Android

|                 project                  |          description           |                           license                            |
|:----------------------------------------:|:------------------------------:|:------------------------------------------------------------:|
| [NDK](https://developer.android.com/ndk) | android native development kit | [Apache License](http://www.apache.org/licenses/LICENSE-2.0) |

### CHelper-Test

|                      project                       |         description         |                                              license                                              |
|:--------------------------------------------------:|:---------------------------:|:-------------------------------------------------------------------------------------------------:|
| [GoogleTest](https://github.com/google/googletest) | Google's C++ test framework | [BSD 3-Clause "New" or "Revised" License](https://github.com/google/googletest/blob/main/LICENSE) |

## CPack Document

Cpack(Command Pack) is the resource pack of CHelper(Command Helper).

[English](doc/README.md)

[Chinese](doc/README_CN.md)

## Special Thanks


|                       project                       |                        description                         |                                      license                                      |
|:---------------------------------------------------:|:----------------------------------------------------------:|:---------------------------------------------------------------------------------:|
|  [caidlist](https://github.com/XeroAlpha/caidlist)  | provide IDs and descriptions for Minecraft Bedrock Edition |   [GPL-3.0 license](https://github.com/XeroAlpha/caidlist/blob/master/LICENSE)    |
| [chinese minecraft wiki](https://zh.minecraft.wiki) |              provide information in Minecraft              | [CC BY-NC-SA 3.0](https://creativecommons.org/licenses/by-nc-sa/3.0/deed.zh-hans) |

- JetBrains License for Open Source Development

  ![JetBrains Logo (Main) logo](https://resources.jetbrains.com/storage/products/company/brand/logos/jb_beam.svg)
