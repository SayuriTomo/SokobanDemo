# SokobanDemo
库洛游戏 笔试_技术策划 Unreal 5.5.4

## 试玩说明
* 进入Editor后，点击/Game/Sokoban/Maps/Level01.Level01进入Level01开始游玩，总共6关
* 操作说明：
* 移动（W A S D）：玩家会朝对应方向移动1格；如果前进方向有箱子，则将箱子往前进方向推进1格
* 缩放（鼠标滚轮）：可调整玩家镜头的远近
* 过关条件：
* 当所有机关踏板（红色格子）上都有箱子时，门会升起。玩家进入门后，会传送到下一关
* 特殊机制：
* 冰面（蓝色格子）：玩家/箱子进入冰面时，会朝进入方向继续滑行，直到到达非冰面格子或者被阻挡
* 单向门格（带有箭头的格子）：玩家/箱子只能从指定方向进入该格，且进入后不能从反方向返回

## 关卡编辑器说明（Runtime）：
* <img width="200" height="200" alt="image" src="https://github.com/user-attachments/assets/8548bca7-2d09-4efb-838d-de8a7810090a" />
* 进入Editor后，在默认Editor地图InitialMap点击Play，点击左上角关卡编辑器开/关，即可进入关卡编辑模式；再次点击，即可退出关卡编辑模式，进行测试游玩
* 推荐使用流程：在InitialMap，点击Play，点击关卡编辑器开/关，进行关卡布局设计，设置相关属性，输入存档位置，点击存档，再次点击关卡编辑器开/关，进行测试游玩，退出Runtime，找到地图里的BP_SKGridManager，在其Detail面板找到Editor Load，输入之前存档位置，点击LoadSavedLevelInEditor，存档的布局将会显示在地图界面，完成
* 放置物品（鼠标左键点击网格）：将当前毛刷的物品放置在点击网格
* 删除物品（鼠标右键点击网格）：删除放置在点击网格的物品
* 毛刷可通过右侧下拉框，来切换当前毛刷，分别为Wall（墙壁），Floor（地板），Goal（关卡踏板），Door（门），Ice（冰面），One Way Gate（单向门），Box（箱子），Player Start（玩家起始点），Erase（清除）
* <img width="175" height="100" alt="image" src="https://github.com/user-attachments/assets/bbe9f450-491a-4726-ade3-5bbb307a17ad" />
* 支持Goal和Door的n对n关系：切换毛刷至Goal或者Door时，可通过GroupID下拉框来决定后续放置的Goal和Door组合；Door只会在所有相同GroupID的Goal都满足时，允许玩家过关；支持后续在蓝图更改
* <img width="175" height="100" alt="image" src="https://github.com/user-attachments/assets/4b4e7434-10c0-497c-9c1a-e434af369021" />
* Door的关卡跳转设置：切换毛刷至Door时，可通过在【目标关卡】填写Level名，来决定后续放置的Door目标关卡；当文件有该Level名，玩家进入Door时，会传送至对应关卡；当为空，则不会传送；支持后续在蓝图更改
* <img width="175" height="100" alt="image" src="https://github.com/user-attachments/assets/abf4ff7a-d276-4acb-8492-f97c9bcd0619" />
* 地图尺寸设置：在X和Y填写数字后，点击重置网格/重建网格，会按填写的数值更改地图网格，重置网格会清空当前放置的物品，重建网格则会尽可能保留当前放置的物品
* <img width="175" height="100" alt="image" src="https://github.com/user-attachments/assets/f24802f1-b31d-46b7-b0c3-3263289a9311" />
* Runtime存档/读档：在存/读档填写后，点击存档按钮，会按填写的名字进行存档，会将当前布局保存；点击读档按钮，会按存档名进行读取，用保存的布局来覆盖当前布局
* <img width="300" height="300" alt="image" src="https://github.com/user-attachments/assets/5494e61f-0909-42ba-b69f-c96cf9d2e289" />
* Editor读档（非Runtime）：点击地图里放置的BP_SKGridManager，在Detail面板找到Editor Load，在Editor Load Slot Name填写存档名，点击LoadSavedLevelInEditor，会将保存的布局在覆盖在当前地图
* 点击ClearSpawnedLevelActors，会清除当前布局
* 在非Runtime时调整了地图时（例如，在蓝图更改了Door相关设置），点击UpdateCachedLevelDataFromCurrentCellMap来同步进SKGridManager
* 如想要在新地图使用编辑器，需确认地图里有BP_SKGridManager和BP_SKLevelEditorManager；如果没有，可从/Game/Sokoban/Blueprints找到并拖拽到地图

## 代码架构/设计思路说明：
* 采用Grid-based的架构 + 数据驱动关卡生成的设计思路，使玩法逻辑、关卡数据与场景表现解耦，便于扩展新的关卡机制和编辑器功能
* 采用Runtime的可视化编辑模式，允许策划在运行时快速搭建关卡，可以直接修改关卡并立即进行测试玩家移动路径、箱子位置和机关组合，形成“编辑->测试->修改”的快速循环，减少了策划需反复重新进入游戏测试。
* SKType：定义玩法所需的数据结构，包含FSKLevelData（网格地图数据）FSKCellData（单个格子数据）
* GridManager：负责玩法的逻辑运行，包含网格数据管理、Actor生成、箱子/玩家移动、阻碍检测、关卡踏板和门的条件检测、非Runtime时加载关卡布局
* LevelEditorManager：负责编辑器模式，包含编辑器模式切换、鼠标绘制关卡、毛刷切换、关卡runtime时的保存与加载
