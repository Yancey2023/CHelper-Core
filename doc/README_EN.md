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
  "version": 1,
  "isBasicPack": false
}
```

|        名字        | 类型  |     含义      |          备注           |    是否必须有    |
|:----------------:|:---:|:-----------:|:---------------------:|:-----------:|
|       name       | 字符串 |   扩展包的名字    |         用于显示          |  否（不建议忽略）   |
|   description    | 字符串 |   扩展包的简介    |         用于显示          |      否      |
| minecraftVersion | 字符串 |   对应的游戏版本   |         用于显示          |      否      |
|      author      | 字符串 |     作者      |         用于显示          |      否      |
|    updateDate    | 字符串 |    更新日期     |         用于显示          |      否      |
|      packId      | 字符串 |   扩展包的ID    |        用于识别扩展包        |      是      |
|   requiredPack   | 数组  | 需要依赖的扩展包的ID |     用于识别扩展包的依赖关系      |  否（默认没有依赖）  |
|     version      | 整数  |     版本号     |       用于显示和判断版本       |      是      |
|   isBasicPack    | 布尔值 |  是否是命令基础包   | 同时只能存在一个命令基础包和多个命令扩展包 | 否（默认为false） |