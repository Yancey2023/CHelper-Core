[English](README.md) | 简体中文

# CPack说明文档

## CPack是什么

Cpack(Command Pack)是CHelper(Command Helper)的资源包。

CHelper是我的世界基岩版的命令助手。

项目源码: <https://github.com/Yancey2023/CHelper-Core>

## manifest.json

在资源包的根目录里应该存在一个叫做`manifest.json`的文件用来存储命令包的基本信息：

```json
{
    "name": "资源包示例",
    "description": "CHelper扩展包的官方示例",
    "version": "1.20",
    "versionType": "release",
    "branch": "vanilla",
    "author": "Yancey",
    "updateDate": "2025-5-30",
    "packId": "ExamplePack-1.20-vanilla",
    "requiredPack": [
        "BasicPack-1.20"
    ],
    "versionCode": 1,
    "isBasicPack": true,
    "isDefault": true
}
```

|      名字      | 类型  |     含义      |          备注           |     必需      |
|:------------:|:---:|:-----------:|:---------------------:|:-----------:|
|     name     | 字符串 |   资源包的名字    |         用于显示          |  否（不建议忽略）   |
| description  | 字符串 |   资源包的简介    |         用于显示          |      否      |
|   version    | 字符串 |   对应的游戏版本   |         用于显示          |      否      |
| versionType  | 字符串 |   游戏版本类型    |         用于显示          |      否      |
|    branch    | 字符串 |   游戏版本分支    |         用于显示          |      否      |
|    author    | 字符串 |     作者      |         用于显示          |      否      |
|  updateDate  | 字符串 |    更新日期     |         用于显示          |      否      |
|    packId    | 字符串 |   资源包的ID    |        用于识别资源包        |      是      |
| requiredPack | 数组  | 需要依赖的资源包的ID |     用于识别资源包的依赖关系      |  否（默认没有依赖）  |
| versionCode  | 整数  |     版本号     |       用于显示和判断版本       |      是      |
| isBasicPack  | 布尔值 |  是否是命令基础包   | 只能同时加载一个命令基础包和多个命令扩展包 | 否（默认为false） |

> 注意：暂时还不支持命令拓展包，目前只支持命令基础包

## 命令的注册

在个根目录中，有一个`command`的文件夹用于存储命令，里面的每个json文件对应一个命令。

```json
{
    "name": [
        "ability"
    ],
    "description": "赋予或剥夺玩家的能力",
    "start": [
        "player"
    ],
    "node": [
        {
            "type": "TARGET_SELECTOR",
            "id": "player",
            "description": "要赋予或剥夺能力的玩家",
            "isOnlyOne": false,
            "isMustPlayer": true,
            "isMustNPC": false
        },
        {
            "type": "NORMAL_ID",
            "id": "ability",
            "description": "要操作的能力",
            "ignoreError": false,
            "contents": [
                {
                    "name": "worldbuilder",
                    "description": "给予玩家成为世界建造者的能力"
                },
                {
                    "name": "mayfly",
                    "description": "给予飞行的能力"
                },
                {
                    "name": "mute",
                    "description": "将玩家禁言，聊天时其他人将无法看见或听见目标"
                }
            ]
        },
        {
            "type": "BOOLEAN",
            "id": "value",
            "description": "此能力是否对玩家可用",
            "descriptionTrue": "此能力对玩家可用",
            "descriptionFalse": "此能力对玩家不可用"
        }
    ],
    "ast": [
        ["player", "ability", "LF"],
        ["ability", "value"],
        ["value", "LF"]
    ]
}
```

|     名字      |  类型   |   含义    |                  备注                  | 必需 |
|:-----------:|:-----:|:-------:|:------------------------------------:|:--:|
|    name     | 字符串列表 |  命令的名字  |                  -                   | 是  |
| description |  字符串  |  命令的介绍  |                  -                   | 否  |
|    node     | 节点列表  |  节点列表   |               列举所有命令参数               | 是  |
|    start    | 字符串列表 | 命令的起始节点 |           由于节点类型太多，会在后面单独讲           | 是  |
|     ast     | 关系列表  |  关系绑定   | 存储着多个数组，其中的每个数组第一个ID代表父节点，后面的元素当作子节点 | 否  |

解释：在node节点列表中，每个节点有个ID，在加载好节点列表后，程序再根据后面的start和ast用这个ID来绑定节点的位置。

## ID的注册

在个根目录中，有一个`id`的文件夹用于存储命令，里面的每个json文件对应一种id。

ID 有4种类型：

|    类型     |    含义    |   备注    |
|:---------:|:--------:|:-------:|
|  normal   |   普通ID   |    -    |
| namespace | 带命名空间的ID |    -    |
|   block   |   方块ID   | 带有方块状态  |
|   item    |   物品ID   | 带有物品数据值 |

- NormalID

```json
{
    "type": "normal",
    "id": "fogs",
    "content": [
        {
            "name": "minecraft:fog_bamboo_jungle",
            "description": "竹林"
        },
        ...
    ]
}
```

|   名字    | 类型  |   含义    | 备注 | 必需 |
|:-------:|:---:|:-------:|:--:|:--:|
|  type   | 字符串 |  ID类型   | -  | 是  |
|   id    | 字符串 | ID列表的名字 | -  | 是  |
| content | 数组  |  ID列表   | -  | 是  |

每个ID：

|     名字      | 类型  |  含义  | 备注 | 必需 |
|:-----------:|:---:|:----:|:--:|:--:|
|    name     | 字符串 | ID名字 | -  | 是  |
| description | 字符串 | ID介绍 | -  | 否  |

- NamespaceID

```json
{
    "type": "namespace",
    "id": "entities",
    "content": [
        {
            "idNamespace": "minecraft",
            "name": "allay",
            "description": "悦灵"
        },
        ...
    ]
}
```

|   名字    | 类型  |   含义    | 备注 | 必需 |
|:-------:|:---:|:-------:|:--:|:--:|
|  type   | 字符串 |  ID类型   | -  | 是  |
|   id    | 字符串 | ID列表的名字 | -  | 是  |
| content | 数组  |  ID列表   | -  | 是  |

每个ID：

|     名字      | 类型  |  含义  |               备注               | 必需 |
|:-----------:|:---:|:----:|:------------------------------:|:--:|
| idNamespace | 字符串 | 命名空间 | 默认为minecraft，如果是minecraft，建议省略 | 否  |
|    name     | 字符串 | ID名字 |               -                | 是  |
| description | 字符串 | ID介绍 |               -                | 否  |

- blocks

```json
{
    "id": "blocks",
    "type": "block",
    "blocks": {
        "blockStateValues": [
            {
                "name": "acacia_button",
                "description": "金合欢木按钮",
                "properties": [
                    {
                        "name": "button_pressed_bit",
                        "defaultValue": false
                    },
                    {
                        "name": "facing_direction",
                        "defaultValue": 0
                    }
                ]
            },
            ...
        ],
        "blockPropertyDescriptions": {
            "common": [
                {
                    "propertyName": "age",
                    "description": "植物的生长阶段 刚种下或刚生长出的植物方块的该属性值为0 该值随着时间变化不断增长，直到15时可以继续在上方生长出新的植物方块",
                    "values": [
                        {
                            "valueName": 0
                        },
                        {
                            "valueName": 1
                        },
                        ...
                    ]
                },
                ...
            ],
            "block": [
                {
                    "blocks": [
                        "creaking_heart"
                    ],
                    "properties": [
                        {
                            "propertyName": "active",
                            "values": [
                                {
                                    "valueName": true,
                                    "description": "嘎枝之心处于激活状态"
                                },
                                {
                                    "valueName": false,
                                    "description": "嘎枝之心处于未激活状态"
                                }
                            ]
                        }
                    ]
                },
                ...
            ]
        }
    }
}
```

|   名字   | 类型  |  含义  |    备注     | 必需 |
|:------:|:---:|:----:|:---------:|:--:|
|   id   | 字符串 | ID名字 | 永远是blocks | 否  |
|  type  | 字符串 | ID类型 |     -     | 是  |
| blocks | 数组  | ID列表 |     -     | 是  |

blocks：
| 名字 | 类型 | 含义 | 备注 | 必需 |
|:------:|:---:|:----:|:---------:|:--:|
| blockStateValues | 数组 | 所有方块及其方块状态属性名和默认值 | - | 是 |
| blockPropertyDescriptions | 所有方块状态值及注释 | ID类型 | - | 是 |

blockStateValues每个ID：

|     名字      | 类型  |  含义  |               备注               | 必需 |
|:-----------:|:---:|:----:|:------------------------------:|:--:|
| idNamespace | 字符串 | 命名空间 | 默认为minecraft，如果是minecraft，建议省略 | 否  |
|    name     | 字符串 | ID名字 |               -                | 是  |
| description | 字符串 | ID介绍 |               -                | 否  |
| properties  | 数组  | 方块状态 |               -                | 否  |

properties每个方块状态：

|      名字      | 类型  |     含义      |  备注   | 必需 |
|:------------:|:---:|:-----------:|:-----:|:--:|
|     name     | 字符串 |   方块状态参数名   |   -   | 是  |
| defaultValue | 正整数 | 默认值是第几个方块状态 | 暂时没用到 | 是  |

blockPropertyDescriptions：

|   名字   | 类型 |     含义      | 备注 | 必需 |
|:------:|:--:|:-----------:|:--:|:--:|
| common | 数组 | 所有方块共用的方块状态 | -  | 是  |
| block  | 数组 | 部分方块特有的方块状态 | -  | 是  |

common每个方块状态：

|      名字      | 类型  |    含义     | 备注 | 必需 |
|:------------:|:---:|:---------:|:--:|:--:|
| propertyName | 字符串 |  方块状态参数名  | -  | 是  |
| description  | 字符串 |   ID介绍    | -  | 否  |
|    values    | 数组  | 方块状态参数值列表 | -  | 是  |

values每个方块状态的值：

|     名字      |       类型        |   含义    | 备注 | 必需 |
|:-----------:|:---------------:|:-------:|:--:|:--:|
|    value    | 布尔值 / 正整数 / 字符串 | 方块状态参数值 | -  | 是  |
| description |       字符串       |   介绍    | -  | 否  |

block中的每个值：

|     名字     |   类型   |  含义  | 备注 | 必需 |
|:----------:|:------:|:----:|:--:|:--:|
|   blocks   | 字符串数组  | 方块ID | -  | 是  |
| properties | 方块状态数组 | 方块状态 | -  | 是  |

properties中的每个方块状态：

|      名字      | 类型  |    含义     | 备注 | 必需 |
|:------------:|:---:|:---------:|:--:|:--:|
| propertyName | 字符串 |  方块状态参数名  | -  | 是  |
| description  | 字符串 |   ID介绍    | -  | 否  |
|    values    | 数组  | 方块状态参数值列表 | -  | 是  |

values每个方块状态的值：

|     名字      |       类型        |   含义    | 备注 | 必需 |
|:-----------:|:---------------:|:-------:|:--:|:--:|
|    value    | 布尔值 / 正整数 / 字符串 | 方块状态参数值 | -  | 是  |
| description |       字符串       |   介绍    | -  | 否  |

- items

```json
{
    "id": "items",
    "type": "item",
    "items": [
        {
            "name": "stone_block_slab2",
            "description": "红砂岩台阶/紫珀台阶/海晶石台阶/暗海晶石台阶/海晶石砖台阶/苔石台阶/平滑砂岩台阶/红色下界砖台阶",
            "descriptions": [
                "红砂岩台阶",
                "紫珀台阶",
                "海晶石台阶",
                "暗海晶石台阶",
                "海晶石砖台阶",
                "苔石台阶",
                "平滑砂岩台阶",
                "红色下界砖台阶"
            ]
        },
        ...
    ]
}
```

|  名字   | 类型  |  含义  |    备注    | 必需 |
|:-----:|:---:|:----:|:--------:|:--:|
|  id   | 字符串 | ID名字 | 永远是items | 否  |
| type  | 字符串 | ID类型 |    -     | 是  |
| items | 数组  | ID列表 |    -     | 是  |

每个ID：

|      名字      |  类型   |       含义       |        备注         |       必需        |
|:------------:|:-----:|:--------------:|:-----------------:|:---------------:|
|  namespace   |  字符串  |      命名空间      | 如果是minecraft，建议省略 | 否(默认为minecraft) |
|     name     |  字符串  |      ID名字      |         -         |        是        |
| description  |  字符串  |      ID介绍      |         -         |        否        |
|     max      |  正整数  |     数据值最大值     |         -         |        否        |
| descriptions | 字符串列表 | 每个数据值的介绍（从0开始） |         -         |        否        |

## 节点的定义

在前面已经讲了命令是如何注册的，其中每个命令需要在nodes里面写出所有用到的的节点，每个节点的json如何写就是接下来要讲的内容

|        类型         |     含义     |                 示例                 |          备注           |
|:-----------------:|:----------:|:----------------------------------:|:---------------------:|
|       BLOCK       |     方块     | minecraft:lever\["open_bit"=true\] |           -           |
|      BOOLEAN      |    布尔值     |                true                |           -           |
|      COMMAND      |     命令     |      give @s minecraft:stone       |           -           |
|   COMMAND_NAME    |    命令名     |              setblock              |           -           |
|       FLOAT       |     小数     |                1.5                 |           -           |
|      INTEGER      |     整数     |                 2                  |           -           |
| INTEGER_WITH_UNIT |   带单位的整数   |                 2L                 |           -           |
|       ITEM        |     物品     |     minecraft:fire_charge 1 12     |           -           |
|        LF         |    结束节点    |                 -                  | 内置节点，不可使用，但可以使用LF这个ID |
|   NAMESPACE_ID    |  带命名空间的ID  |          minecraft:stone           |           -           |
|     NORMAL_ID     |   普通的ID    |              creative              |           -           |
|    PER_COMMAND    |    每条命令    |                 -                  |       内置节点，不可使用       |
|     POSITION      |     位置     |             ~1~0.2~-5              |           -           |
|  RELATIVE_FLOAT   |    相对坐标    |                ~1.5                |           -           |
|      REPEAT       |   重复的参数    |                 -                  |   不可在定义JSON数据的时候使用    |
|      STRING       |    字符串     |               "a a"                |           -           |
|  TARGET_SELECTOR  |   目标选择器    |             @e\[r=5\]              |           -           |
|       TEXT        |     文字     |               score                |           -           |
|       RANGE       |     范围     |                1..2                |           -           |
|       JSON        |   Json文本   |  {"rawtext":\[{"text":"Hello"}\]}  |   不可在定义JSON数据的时候使用    |
|   JSON_BOOLEAN    |  Json布尔值   |                true                |   只能在定义JSON数据的时候使用-   |
|   JSON_ELEMENT    |   Json元素   |                 -                  |       内置节点，不可使用       |
|    JSON_ENTRY     |  Json键值对   |                 -                  |       内置节点，不可使用       |
|    JSON_FLOAT     |   Json小数   |                1.5                 |   只能在定义JSON数据的时候使用    |
|   JSON_INTEGER    |   Json整数   |                 12                 |   只能在定义JSON数据的时候使用    |
|     JSON_LIST     |   Json列表   |            \[..., ...\]            |   只能在定义JSON数据的时候使用    |
|     JSON_NULL     |   Json空值   |                null                |   只能在定义JSON数据的时候使用    |
|    JSON_OBJECT    |   Json对象   |      {"...": ..., "...": ...}      |   只能在定义JSON数据的时候使用    |
|    JSON_STRING    |  Json字符串   |               "..."                |   只能在定义JSON数据的时候使用    |
|        AND        |    和节点     |                 -                  |       内置节点，不可使用       |
|        ANY        |    任何节点    |                 -                  |       内置节点，不可使用       |
|       ENTRY       |    键值对     |                 -                  |       内置节点，不可使用       |
|    EQUAL_ENTRY    | 可以是不等号的键值对 |                 -                  |       内置节点，不可使用       |
|       LIST        |     数组     |                 -                  |       内置节点，不可使用       |
|        OR         |    或节点     |                 -                  |       内置节点，不可使用       |
|   SINGLE_SYMBOL   |    单个字符    |                 -                  |       内置节点，不可使用       |

- BLOCK 方块

```mcfunction
含义：方块ID+方块状态 / 方块ID
例子：minecraft:lever["open_bit"=true]
```

```json
{
    "type": "BLOCK",
    "id": "block",
    "description": "更改后的新方块",
    "nodeBlockType": 0
}
```

|      名字       | 类型  |           含义           |            备注             | 必需 |
|:-------------:|:---:|:----------------------:|:-------------------------:|:--:|
|     type      | 字符串 |          节点类型          |             -             | 是  |
|      id       | 字符串 |          节点ID          |             -             | 否  |
|     brief     | 字符串 |          简要说明          | 如果description太长，推荐填写brief | 否  |
|  description  | 字符串 |          节点介绍          |             -             | 否  |
| nodeBlockType | 正整数 | 0代表方块ID+方块状态，1代表只有方块ID |             -             | 是  |

- BOOLEAN 布尔值

```mcfunction
含义：布尔值，只能是true或false
例子：true
```

```json
{
    "type": "BOOLEAN",
    "id": "lock",
    "description": "是否锁定日夜更替",
    "descriptionTrue": "锁定昼夜更替",
    "descriptionFalse": "不锁定昼夜更替"
}
```

|       名字        | 类型  |    含义     |            备注             | 必需 |
|:---------------:|:---:|:---------:|:-------------------------:|:--:|
|      type       | 字符串 |   节点类型    |             -             | 是  |
|       id        | 字符串 |   节点ID    |             -             | 否  |
|      brief      | 字符串 |   简要说明    | 如果description太长，推荐填写brief | 否  |
|   description   | 字符串 |   节点介绍    |             -             | 否  |
| descriptionTrue | 字符串 | true值得介绍  |             -             | 否  |
| descriptionTrue | 字符串 | false值得介绍 |             -             | 否  |

- COMMAND 命令

```mcfunction
含义：用于命令中嵌套任意命令
例子：give @s minecraft:stone
```

```json
{
    "type": "COMMAND",
    "id": "command",
    "description": "命令"
}
```

|     名字      | 类型  |  含义  |            备注             | 必需 |
|:-----------:|:---:|:----:|:-------------------------:|:--:|
|    type     | 字符串 | 节点类型 |             -             | 是  |
|     id      | 字符串 | 节点ID |             -             | 否  |
|    brief    | 字符串 | 简要说明 | 如果description太长，推荐填写brief | 否  |
| description | 字符串 | 节点介绍 |             -             | 否  |

- COMMAND_NAME 命令名

```mcfunction
含义：命令名
例子：setblock
```

```json
{
    "type": "COMMAND_NAME",
    "id": "command",
    "brief": "命令名",
    "description": "要提供帮助的命令名称"
}
```

|     名字      | 类型  |  含义  |            备注             | 必需 |
|:-----------:|:---:|:----:|:-------------------------:|:--:|
|    type     | 字符串 | 节点类型 |             -             | 是  |
|     id      | 字符串 | 节点ID |             -             | 否  |
|    brief    | 字符串 | 简要说明 | 如果description太长，推荐填写brief | 否  |
| description | 字符串 | 节点介绍 |             -             | 否  |

- FLOAT 小数

```mcfunction
含义：小数
例子：1.5
```

```json
{
    "type": "FLOAT",
    "id": "fadeInSeconds",
    "brief": "淡入时间",
    "description": "相机视角的淡入时间",
    "min": 0,
    "max": 10
}
```

|     名字      | 类型  |  含义  |            备注             | 必需 |
|:-----------:|:---:|:----:|:-------------------------:|:--:|
|    type     | 字符串 | 节点类型 |             -             | 是  |
|     id      | 字符串 | 节点ID |             -             | 否  |
|    brief    | 字符串 | 简要说明 | 如果description太长，推荐填写brief | 否  |
| description | 字符串 | 节点介绍 |             -             | 否  |
|     min     | 小数  | 最小值  |             -             | 否  |
|     max     | 小数  | 最大值  |             -             | 否  |

- INTEGER 整数

```mcfunction
含义：整数
例子：2
```

```json
{
    "type": "INTEGER",
    "id": "page",
    "brief": "页码",
    "description": "要展示的命令列表的页码（小于1的数字会被视为1，大于总页数会被默认为展示最后一页）"
}
```

|     名字      | 类型  |  含义  |            备注             | 必需 |
|:-----------:|:---:|:----:|:-------------------------:|:--:|
|    type     | 字符串 | 节点类型 |             -             | 是  |
|     id      | 字符串 | 节点ID |             -             | 否  |
|    brief    | 字符串 | 简要说明 | 如果description太长，推荐填写brief | 否  |
| description | 字符串 | 节点介绍 |             -             | 否  |
|     min     | 整数  | 最小值  |             -             | 否  |
|     max     | 整数  | 最大值  |             -             | 否  |

- INTEGER_WITH_UNIT 带单位的整数

```mcfunction
含义：带单位的整数
例子：2L
```

```json
{
    "type": "INTEGER_WITH_UNIT",
    "id": "amount",
    "brief": "经验值数量",
    "description": "给予玩家的经验值数量",
    "units": [
        {
            "name": "L",
            "description": "一个经验等级"
        }
    ]
}
```

|     名字      | 类型  |  含义  |            备注             | 必需 |
|:-----------:|:---:|:----:|:-------------------------:|:--:|
|    type     | 字符串 | 节点类型 |             -             | 是  |
|     id      | 字符串 | 节点ID |             -             | 否  |
|    brief    | 字符串 | 简要说明 | 如果description太长，推荐填写brief | 否  |
| description | 字符串 | 节点介绍 |             -             | 否  |
|    units    | 数组  | 单位列表 |             -             | 是  |

每个单位：

|     名字      | 类型  | 含义  | 备注 | 必需 |
|:-----------:|:---:|:---:|:--:|:--:|
|    name     | 字符串 | 单位名 | -  | 是  |
| description | 字符串 | 介绍  | -  | 否  |

- ITEM 物品

```mcfunction
含义：物品
例子：minecraft:fire_charge 1 12
```

```json
{
    "type": "ITEM",
    "id": "item",
    "description": "要给予实体的物品",
    "nodeItemType": 0
}
```

|      名字      | 类型  |                          含义                          |            备注             | 必需 |
|:------------:|:---:|:----------------------------------------------------:|:-------------------------:|:--:|
|     type     | 字符串 |                         节点类型                         |             -             | 是  |
|      id      | 字符串 |                         节点ID                         |             -             | 否  |
|    brief     | 字符串 |                         简要说明                         | 如果description太长，推荐填写brief | 否  |
| description  | 字符串 |                         节点介绍                         |             -             | 否  |
| nodeItemType | 正整数 | 0代表<物品ID> <物品数量> <附加值> [物品组件]，1代表<物品ID> <附加值> <物品数量> |             -             | 是  |

- NAMESPACE_ID 带命名空间的ID

```mcfunction
含义：带命名空间的ID
例子：minecraft:stone
```

```json
{
    "type": "NAMESPACE_ID",
    "id": "entityType",
    "brief": "实体类型",
    "description": "要被召唤的实体类型",
    "key": "entities",
    "ignoreError": true
}
```

|     名字      | 类型  |               含义                |            备注             | 必需 |
|:-----------:|:---:|:-------------------------------:|:-------------------------:|:--:|
|    type     | 字符串 |              节点类型               |             -             | 是  |
|     id      | 字符串 |              节点ID               |             -             | 否  |
|    brief    | 字符串 |              简要说明               | 如果description太长，推荐填写brief | 否  |
| description | 字符串 |              节点介绍               |             -             | 否  |
|     key     | 字符串 |        id的键值，用于在id列表索引id        |    key和contents至少存在一个     | 否  |
| ignoreError | 布尔值 | 如果用户输入的内容与id列表不一致时是否报错，默认为false |             -             | 否  |
|  contents   | 数组  |              id列表               |    key和contents至少存在一个     | 否  |

contents每个ID：

|     名字      | 类型  |  含义  |               备注               | 必需 |
|:-----------:|:---:|:----:|:------------------------------:|:--:|
| idNamespace | 字符串 | 命名空间 | 默认为minecraft，如果是minecraft，建议省略 | 否  |
|    name     | 字符串 | ID名字 |               -                | 是  |
| description | 字符串 | ID介绍 |               -                | 否  |

- NORMAL_ID 普通的ID

```mcfunction
含义：普通的ID
例子：creative
```

```json
{
    "type": "NORMAL_ID",
    "id": "hud_element",
    "brief": "HUD元素",
    "description": "将被修改的HUD元素",
    "key": "hudElement",
    "ignoreError": true
}
```

|     名字      | 类型  |               含义                |            备注             | 必需 |
|:-----------:|:---:|:-------------------------------:|:-------------------------:|:--:|
|    type     | 字符串 |              节点类型               |             -             | 是  |
|     id      | 字符串 |              节点ID               |             -             | 否  |
|    brief    | 字符串 |              简要说明               | 如果description太长，推荐填写brief | 否  |
| description | 字符串 |              节点介绍               |             -             | 否  |
|     key     | 字符串 |        id的键值，用于在id列表索引id        |    key和contents至少存在一个     | 否  |
| ignoreError | 布尔值 | 如果用户输入的内容与id列表不一致时是否报错，默认为false |             -             | 否  |
|  contents   | 数组  |              id列表               |    key和contents至少存在一个     | 否  |

contents每个ID：

|     名字      | 类型  |  含义  | 备注 | 必需 |
|:-----------:|:---:|:----:|:--:|:--:|
|    name     | 字符串 | ID名字 | -  | 是  |
| description | 字符串 | ID介绍 | -  | 否  |

- POSITION 位置

```mcfunction
含义：位置
例子：~1~0.2~-5
```

```json
{
    "type": "POSITION",
    "id": "position",
    "brief": "方块位置",
    "description": "要被更改方块的位置"
}
```

|     名字      | 类型  |  含义  |            备注             | 必需 |
|:-----------:|:---:|:----:|:-------------------------:|:--:|
|    type     | 字符串 | 节点类型 |             -             | 是  |
|     id      | 字符串 | 节点ID |             -             | 否  |
|    brief    | 字符串 | 简要说明 | 如果description太长，推荐填写brief | 否  |
| description | 字符串 | 节点介绍 |             -             | 否  |

- RELATIVE_FLOAT 相对坐标

```mcfunction
含义：相对坐标
例子：~1.5
```

```json
{
    "type": "RELATIVE_FLOAT",
    "id": "xRot",
    "brief": "rx",
    "description": "相机视角绕Y轴旋转的旋转角度(-180.0表示北，-90.0表示东，0.0表示南，90.0表示西)",
    "canUseCaretNotation": false
}
```

|         名字          | 类型  |      含义       |            备注             | 必需 |
|:-------------------:|:---:|:-------------:|:-------------------------:|:--:|
|        type         | 字符串 |     节点类型      |             -             | 是  |
|         id          | 字符串 |     节点ID      |             -             | 否  |
|        brief        | 字符串 | 简要说明，用于显示命令结构 | 如果description太长，推荐填写brief | 否  |
|     description     | 字符串 |     节点介绍      |             -             | 否  |
| canUseCaretNotation | 布尔值 |  是否可以使用局部坐标   |             -             | 否  |

- REPEAT 重复的参数

```mcfunction
含义：重复的参数
例子：-
```

```json
{
    "type": "REPEAT",
    "id": "executeParam",
    "description": "子命令",
    "key": "execute"
}
```

|     名字      | 类型  |        含义         |            备注             | 必需 |
|:-----------:|:---:|:-----------------:|:-------------------------:|:--:|
|    type     | 字符串 |       节点类型        |             -             | 是  |
|     id      | 字符串 |       节点ID        |             -             | 否  |
|    brief    | 字符串 |   简要说明，用于显示命令结构   | 如果description太长，推荐填写brief | 否  |
| description | 字符串 |       节点介绍        |             -             | 否  |
|     key     | 字符串 | 键值，用于在repeat中进行索引 |             -             | 是  |

- STRING 字符串

```mcfunction
含义：字符串
例子："a a"
```

```json
{
    "type": "STRING",
    "id": "trackName",
    "brief": "播放音乐名",
    "description": "必须为音乐名或record.<music_name>或music.game.<music_name>",
    "canContainSpace": true,
    "ignoreLater": false
}
```

|       名字        | 类型  |               含义               |            备注             | 必需 |
|:---------------:|:---:|:------------------------------:|:-------------------------:|:--:|
|      type       | 字符串 |              节点类型              |             -             | 是  |
|       id        | 字符串 |              节点ID              |             -             | 否  |
|      brief      | 字符串 |              简要说明              | 如果description太长，推荐填写brief | 否  |
|   description   | 字符串 |              节点介绍              |             -             | 否  |
| canContainSpace | 布尔值 |     是否可以包含空格，也就是说是否支持使用双引号     |             -             | 是  |
|   ignoreLater   | 布尔值 | 是否忽略后面的东西，也就是说是否把后面的所有文字都当作字符串 |             -             | 是  |

- TARGET_SELECTOR 目标选择器

```mcfunction
含义：目标选择器
例子：@e[r=5]
```

```json
{
    "type": "TARGET_SELECTOR",
    "id": "target",
    "description": "被给予物品的玩家",
    "isOnlyOne": false,
    "isMustPlayer": true,
    "isMustNPC": false,
    "isWildcard": false
}
```

|      名字      | 类型  |     含义     |            备注             | 必需 |
|:------------:|:---:|:----------:|:-------------------------:|:--:|
|     type     | 字符串 |    节点类型    |             -             | 是  |
|      id      | 字符串 |    节点ID    |             -             | 否  |
|    brief     | 字符串 |    简要说明    | 如果description太长，推荐填写brief | 否  |
| description  | 字符串 |    节点介绍    |             -             | 否  |
|  isOnlyOne   | 布尔值 | 是否只能选择1个对象 |             -             | 是  |
| isMustPlayer | 布尔值 |  是否只能选择玩家  |             -             | 是  |
|  isMustNPC   | 布尔值 | 是否只能选择NPC  |             -             | 是  |
|  isWildcard  | 布尔值 |  是否可以使用*   |             -             | 是  |

- TEXT 文字

```mcfunction
含义：文字
例子：score
```

```json
{
    "type": "TEXT",
    "id": "play",
    "description": "播放音乐",
    "data": {
        "name": "play",
        "description": "播放音乐"
    }
}
```

|     名字      | 类型  |  含义  |            备注             | 必需 |
|:-----------:|:---:|:----:|:-------------------------:|:--:|
|    type     | 字符串 | 节点类型 |             -             | 是  |
|     id      | 字符串 | 节点ID |             -             | 否  |
|    brief    | 字符串 | 简要说明 | 如果description太长，推荐填写brief | 否  |
| description | 字符串 | 节点介绍 |             -             | 否  |
|    data     | 数组  |  内容  |             -             | 是  |

data：

|     名字      | 类型  |  含义  | 备注 | 必需 |
|:-----------:|:---:|:----:|:--:|:--:|
|    name     | 字符串 | ID名字 | -  | 是  |
| description | 字符串 | ID介绍 | -  | 否  |

- RANGE 范围

```mcfunction
含义：范围
例子：1..2
```

```json
{
    "type": "RANGE",
    "id": "range",
    "description": "分数范围"
}
```

|     名字      | 类型  |  含义  |            备注             | 必需 |
|:-----------:|:---:|:----:|:-------------------------:|:--:|
|    type     | 字符串 | 节点类型 |             -             | 是  |
|     id      | 字符串 | 节点ID |             -             | 否  |
|    brief    | 字符串 | 简要说明 | 如果description太长，推荐填写brief | 否  |
| description | 字符串 | 节点介绍 |             -             | 否  |

- JSON Json文本

```mcfunction
含义：Json文本
例子：{"rawtext":[{"text":"Hello"}]}
```

```json
{
    "type": "JSON",
    "id": "raw json message",
    "brief": "JSON文本",
    "description": "要发送的消息",
    "key": "rawtext"
}
```

|     名字      | 类型  |       含义        |            备注             | 必需 |
|:-----------:|:---:|:---------------:|:-------------------------:|:--:|
|    type     | 字符串 |      节点类型       |             -             | 是  |
|     id      | 字符串 |      节点ID       |             -             | 否  |
|    brief    | 字符串 |      简要说明       | 如果description太长，推荐填写brief | 否  |
| description | 字符串 |      节点介绍       |             -             | 否  |
|     key     | 字符串 | 键值，用于在json中进行索引 |             -             | 是  |

- JSON_BOOLEAN Json布尔值

```mcfunction
含义：Json布尔值
例子：true
```

```json
{
    "type": "JSON_BOOLEAN",
    "id": "lock",
    "description": "是否锁定日夜更替",
    "descriptionTrue": "锁定昼夜更替",
    "descriptionFalse": "不锁定昼夜更替"
}
```

|       名字        | 类型  |    含义     |            备注             | 必需 |
|:---------------:|:---:|:---------:|:-------------------------:|:--:|
|      type       | 字符串 |   节点类型    |             -             | 是  |
|       id        | 字符串 |   节点ID    |             -             | 否  |
|      brief      | 字符串 |   简要说明    | 如果description太长，推荐填写brief | 否  |
|   description   | 字符串 |   节点介绍    |             -             | 否  |
| descriptionTrue | 字符串 | true值得介绍  |             -             | 否  |
| descriptionTrue | 字符串 | false值得介绍 |             -             | 否  |

- JSON_FLOAT Json小数

```mcfunction
含义：Json小数
例子：1.5
```

```json
{
    "type": "JSON_FLOAT",
    "id": "score",
    "description": "分数"
}
```

|     名字      | 类型  |  含义  |            备注             | 必需 |
|:-----------:|:---:|:----:|:-------------------------:|:--:|
|    type     | 字符串 | 节点类型 |             -             | 是  |
|     id      | 字符串 | 节点ID |             -             | 否  |
|    brief    | 字符串 | 简要说明 | 如果description太长，推荐填写brief | 否  |
| description | 字符串 | 节点介绍 |             -             | 否  |
|     min     | 小数  | 最小值  |             -             | 否  |
|     max     | 小数  | 最大值  |             -             | 否  |

- JSON_INTEGER Json整数

```mcfunction
含义：Json整数
例子：12
```

```json
{
    "type": "JSON_INTEGER",
    "id": "SCORE_VALUE",
    "description": "可选。如果存在此值，则无论分数是多少，都将使用此值。"
}
```

|     名字      | 类型  |  含义  |            备注             | 必需 |
|:-----------:|:---:|:----:|:-------------------------:|:--:|
|    type     | 字符串 | 节点类型 |             -             | 是  |
|     id      | 字符串 | 节点ID |             -             | 否  |
|    brief    | 字符串 | 简要说明 | 如果description太长，推荐填写brief | 否  |
| description | 字符串 | 节点介绍 |             -             | 否  |
|     min     | 整数  | 最小值  |             -             | 否  |
|     max     | 整数  | 最大值  |             -             | 否  |

- JSON_LIST Json列表

```mcfunction
含义：Json列表
例子：[..., ...]
```

```json
{
    "type": "JSON_LIST",
    "id": "WITH",
    "description": "translate使用的聊天字符串参数的列表。",
    "data": "STRING"
}
```

|     名字      | 类型  |  含义  |            备注             | 必需 |
|:-----------:|:---:|:----:|:-------------------------:|:--:|
|    type     | 字符串 | 节点类型 |             -             | 是  |
|     id      | 字符串 | 节点ID |             -             | 否  |
|    brief    | 字符串 | 简要说明 | 如果description太长，推荐填写brief | 否  |
| description | 字符串 | 节点介绍 |             -             | 否  |
|    data     | 字符串 | 数据类型 |             -             | 是  |

- JSON_NULL Json空值

```mcfunction
含义：Json空值
例子：null
```

```json
{
    "type": "JSON_NULL",
    "id": "null",
    "description": "空值"
}
```

|     名字      | 类型  |  含义  |            备注             | 必需 |
|:-----------:|:---:|:----:|:-------------------------:|:--:|
|    type     | 字符串 | 节点类型 |             -             | 是  |
|     id      | 字符串 | 节点ID |             -             | 否  |
|    brief    | 字符串 | 简要说明 | 如果description太长，推荐填写brief | 否  |
| description | 字符串 | 节点介绍 |             -             | 否  |

- JSON_OBJECT Json对象

```mcfunction
含义：Json对象
例子：{"...": ..., "...": ...}
```

```json
{
    "type": "JSON_OBJECT",
    "id": "BLOCKS",
    "description": "方块类型",
    "data": [
        {
            "key": "blocks",
            "description": "方块类型",
            "value": [
                "BLOCK_ID_LIST"
            ]
        }
    ]
}
```

|     名字      | 类型  |  含义  |            备注             | 必需 |
|:-----------:|:---:|:----:|:-------------------------:|:--:|
|    type     | 字符串 | 节点类型 |             -             | 是  |
|     id      | 字符串 | 节点ID |             -             | 否  |
|    brief    | 字符串 | 简要说明 | 如果description太长，推荐填写brief | 否  |
| description | 字符串 | 节点介绍 |             -             | 否  |
|    data     | 数组  | 键值对  |             -             | 是  |

data中的每个键值对：

|     名字      |  类型   |  含义  | 备注 | 必需 |
|:-----------:|:-----:|:----:|:--:|:--:|
|     key     |  字符串  |  键   | -  | 是  |
| description |  字符串  |  介绍  | -  | 否  |
|    value    | 字符串数组 | 值的ID | -  | 是  |

- JSON_STRING Json字符串

```mcfunction
含义：Json字符串
例子："..."
```

```json
{
    "type": "JSON_STRING",
    "id": "TARGET_SELECTOR",
    "description": "目标选择器",
    "data": [
        {
            "type": "TARGET_SELECTOR",
            "id": "TARGET_SELECTOR",
            "description": "目标选择器",
            "isMustPlayer": false,
            "isOnlyOne": false,
            "isMustNPC": false,
            "isWildcard": false
        },
        {
            "type": "TEXT",
            "id": "ALL_TARGET",
            "description": "目标选择器",
            "data": {
                "name": "*",
                "description": "选择全部实体"
            }
        }
    ]
}
```

|     名字      | 类型  |    含义    |            备注             | 必需 |
|:-----------:|:---:|:--------:|:-------------------------:|:--:|
|    type     | 字符串 |   节点类型   |             -             | 是  |
|     id      | 字符串 |   节点ID   |             -             | 否  |
|    brief    | 字符串 |   简要说明   | 如果description太长，推荐填写brief | 否  |
| description | 字符串 |   节点介绍   |             -             | 否  |
|    data     | 数组  | 字符串文本的节点 |             -             | 否  |
