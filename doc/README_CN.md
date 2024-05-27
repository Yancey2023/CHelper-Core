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
  "name": "扩展包示例",
  "description": "CHelper扩展包的官方示例",
  "minecraftVersion": "1.20",
  "author": "Yancey",
  "updateDate": "2023-11-7",
  "packID": "ExamplePack-1.20",
  "requiredPack": [
    "BasicPack-1.20"
  ],
  "versionCode": 1,
  "isBasicPack": false
}
```

|        名字        | 类型  |     含义      |          备注           |     必需      |
|:----------------:|:---:|:-----------:|:---------------------:|:-----------:|
|       name       | 字符串 |   扩展包的名字    |         用于显示          |  否（不建议忽略）   |
|   description    | 字符串 |   扩展包的简介    |         用于显示          |      否      |
| minecraftVersion | 字符串 |   对应的游戏版本   |         用于显示          |      否      |
|      author      | 字符串 |     作者      |         用于显示          |      否      |
|    updateDate    | 字符串 |    更新日期     |         用于显示          |      否      |
|      packId      | 字符串 |   扩展包的ID    |        用于识别扩展包        |      是      |
|   requiredPack   | 数组  | 需要依赖的扩展包的ID |     用于识别扩展包的依赖关系      |  否（默认没有依赖）  |
|   versionCode    | 整数  |     版本号     |       用于显示和判断版本       |      是      |
|   isBasicPack    | 布尔值 |  是否是命令基础包   | 只能同时加载一个命令基础包和多个命令扩展包 | 否（默认为false） |

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
      "ignoreError": true,
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
  "type": "block",
  "blocks": [
    {
      "idNamespace": "minecraft",
      "name": "acacia_button",
      "description": "金合欢木按钮",
      "blockStates": [
        {
          "key": "button_pressed_bit",
          "description": "按钮是否被激活",
          "defaultValue": 0,
          "values": [
            {
              "value": 0,
              "description": "按钮没有被激活"
            },
            {
              "value": 1,
              "description": "按钮被激活"
            }
          ]
        },
        ...
      ]
    },
    ...
  ]
}
```

|   名字   | 类型  |  含义  | 备注 | 必需 |
|:------:|:---:|:----:|:--:|:--:|
|  type  | 字符串 | ID类型 | -  | 是  |
| blocks | 数组  | ID列表 | -  | 是  |

每个ID：

|     名字      | 类型  |  含义  |               备注               | 必需 |
|:-----------:|:---:|:----:|:------------------------------:|:--:|
| idNamespace | 字符串 | 命名空间 | 默认为minecraft，如果是minecraft，建议省略 | 否  |
|    name     | 字符串 | ID名字 |               -                | 是  |
| description | 字符串 | ID介绍 |               -                | 否  |
| blockStates | 数组  | 方块状态 |               -                | 否  |

每个方块状态：

|      名字      | 类型  |     含义      |  备注   | 必需 |
|:------------:|:---:|:-----------:|:-----:|:--:|
|     key      | 字符串 |   方块状态参数名   |   -   | 是  |
| description  | 字符串 |    ID介绍     |   -   | 否  |
| defaultValue | 正整数 | 默认值是第几个方块状态 | 暂时没用到 | 是  |
|    values    | 数组  |  方块状态参数值列表  |   -   | 是  |

每个方块状态的值：

|     名字      |       类型        |   含义    | 备注 | 必需 |
|:-----------:|:---------------:|:-------:|:--:|:--:|
|    value    | 布尔值 / 正整数 / 字符串 | 方块状态参数值 | -  | 是  |
| description |       字符串       |   介绍    | -  | 否  |

- items

```json
{
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

|  名字   | 类型  |  含义  | 备注 | 必需 |
|:-----:|:---:|:----:|:--:|:--:|
| type  | 字符串 | ID类型 | -  | 是  |
| items | 数组  | ID列表 | -  | 是  |

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

|       类型        |    含义    |                 示例                 |        备注         |
|:---------------:|:--------:|:----------------------------------:|:-----------------:|
|     UNKNOWN     |    未知    |                 -                  |       不可使用        |
|      BLOCK      |    方块    | minecraft:lever\["open_bit"=true\] |         -         |
|     BOOLEAN     |   布尔值    |                true                |         -         |
|     COMMAND     |    命令    |      give @s minecraft:stone       |         -         |
|  COMMAND_NAME   |   命令名    |              setblock              |         -         |
|      FLOAT      |    小数    |                1.5                 |         -         |
|     INTEGER     |    整数    |                 2                  |         -         |
|      ITEM       |    物品    |     minecraft:fire_charge 1 12     |         -         |
|       LF        |   结束节点   |                 -                  | 不可使用，但可以使用LF这个ID  |
|  NAMESPACE_ID   | 带命名空间的ID |          minecraft:stone           |         -         |
|    NORMAL_ID    |  普通的ID   |              creative              |         -         |
|   PER_COMMAND   |   每条命令   |                 -                  |       不可使用        |
|    POSITION     |    位置    |             ~1~0.2~-5              |         -         |
| RELATIVE_FLOAT  |   相对坐标   |                ~1.5                |         -         |
|     REPEAT      |  重复的参数   |                 -                  | 不可在定义JSON数据的时候使用  |
|     STRING      |   字符串    |               "a a"                |         -         |
| TARGET_SELECTOR |  目标选择器   |             @e\[r=5\]              |         -         |
|      TEXT       |    文字    |               score                |         -         |
|      RANGE      |    范围    |                1..2                |         -         |
|   XP_INTEGER    |   经验值    |                 3L                 |         -         |
|      JSON       |  Json文本  |  {"rawtext":\[{"text":"Hello"}\]}  | 不可在定义JSON数据的时候使用  |
|   JSON_OBJECT   |  Json对象  |      {"...":..., "...", ...}       | 只能在定义JSON数据的时候使用  |
|    JSON_LIST    |  Json列表  |            \[..., ...]             | 只能在定义JSON数据的时候使用  |
|   JSON_STRING   | Json字符串  |               "..."                | 只能在定义JSON数据的时候使用  |
|  JSON_INTEGER   |  Json整数  |                 12                 | 只能在定义JSON数据的时候使用  |
|   JSON_FLOAT    |  Json小数  |                1.5                 | 只能在定义JSON数据的时候使用  |
|  JSON_BOOLEAN   | Json布尔值  |                true                | 只能在定义JSON数据的时候使用- |
|    JSON_NULL    |  Json空值  |                null                | 只能在定义JSON数据的时候使用  |

- BLOCK 方块

```mcfunction
含义：方块ID+方块状态 / 方块ID
例子：minecraft:stone["stone_type"="diorite"]
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

- POSITION 位置

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

> 文档还没有写完，敬请期待