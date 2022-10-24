// MiraiCP依赖文件(只需要引入这一个)
#include <MiraiCP.hpp>
using namespace MiraiCP;

const PluginConfig CPPPlugin::config{
        "cn.xiaobaomc.group",          // 插件id
        "Xiaobao-QQGroup-Plugin",        // 插件名称
        "1.0.0",            // 插件版本
        "DreamUniverse843",        // 插件作者
        "Whatelse~",  // 可选：插件描述
        "2022-10-18"        // 可选：日期
};

int isKickIgnoreEnable=0;

// 插件实例
class Main : public CPPPlugin {
public:
  // 配置插件信息
  Main() : CPPPlugin() {}
  ~Main() override = default;

  // 入口函数
  void onEnable() override {
    // 请在此处监听
      Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent GroupMessage) { //群内消息的事件注册
          Group ReceivedGroup(GroupMessage.group.id(), GroupMessage.bot.id); // 实例化接收到的群对象
          if(GroupMessage.group.id() == 604890935) // 群消息来源：包子铺
          {
              if(GroupMessage.message == MessageChain(PlainText(".dismisskick"))) //.dismisskick 调用的命令
              {
                  if(isKickIgnoreEnable == 0)
                  {
                      isKickIgnoreEnable = 1;
                      GroupMessage.group.sendMessage(PlainText("已忽视踢出状态。\n机器人将不会监听成员被踢事件。"));
                  }
                  else
                  {
                      isKickIgnoreEnable = 0;
                      GroupMessage.group.sendMessage("已恢复成员踢出监听。");
                  }
              }
          }
          if(GroupMessage.group.id() == 1070074632 || GroupMessage.group.id() == 181327275){
              if(GroupMessage.message == MessageChain(PlainText(".cpptest")))
                  GroupMessage.group.sendMessage("测试成功。\n此消息由基于 C++ 的 MiraiCP 插件发出。");
          }

      });
      //Event::registerEvent<MemberJoinRequestEvent>([](MemberJoinRequestEvent JoinRequestEvent){ //成员申请入群的事件注册
      //
      //});
  }

  // 退出函数
  void onDisable() override {
    /*插件结束前执行*/
    Logger::logger.info("[Info]插件正在关闭。");
  }
};

// 绑定当前插件实例
void MiraiCP::enrollPlugin() {
  MiraiCP::enrollPlugin(new Main);
}
