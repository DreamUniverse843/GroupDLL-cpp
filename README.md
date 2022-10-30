# GroupDLL-cpp for Baozipu QQ Group
为包子铺群机器人设计的 MiraiCP 机器人插件。

目前正在进行迁移重构（Yiyuyan -> C++）以提升运行性能。

## 迁移进度
### 基本监听业务
- [ ] 入群申请提示(等待 JoinRequestEvent 查明)
- [x] 成员离开事件(主动、踢出)
- [x] 遍历群成员
- [x] 自动检测群名片
- [x] 入群成员提示语
### 文件操作业务
- [ ] 离群日志
- [ ] 入群黑名单
- [x] 自动发送落地文件<br>(日志采用本地写出而不再使用逐个发送 MessageChain)
