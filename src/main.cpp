// MiraiCP依赖文件(只需要引入这一个)
#include <MiraiCP.hpp>
#include <fstream>
#include "windows.h"
using namespace MiraiCP;

const PluginConfig CPPPlugin::config{
        "cn.xiaobaomc.group",          // 插件id
        "Xiaobao-QQGroup-Plugin",        // 插件名称
        "1.0.0",            // 插件版本
        "DreamUniverse843",        // 插件作者
        "Whatelse~",  // 可选：插件描述
        "2022-10-18"        // 可选：日期
};


//是否启用监听踢出（全局变量）
int isKickIgnoreEnable=0;
//常量 - 管理员数组，如发出者账号为管理员则执行对应命令
const long long int Admins[10]={3212903564,344731666,1254207361,767607252,437333682,3476768863,2378612355,3100298105,2720844086};

//群对象，从上至下依次为包子铺、通知群、联合会群。
Group Baozipu(604890935,2651987825);
Group Notify(181327275,2651987825);
Group Lianhehui(1064042359,2651987825);

// 插件实例
class Main : public CPPPlugin {
public:
  // 配置插件信息
  Main() : CPPPlugin() {}
  ~Main() override = default;

  // 入口函数
  void onEnable() override {
    // 请在此处监听
      Logger::logger.info("[Info]插件正在启动。如群内无法接收到机器人消息，则可能处于风控状态。");
      Sleep(1500);//避免风控，缓冲 1500 ms 再继续
      Notify.sendMessage("[Info]插件服务已经启动。");
      Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent GroupMessage) { //群内消息的事件注册
          Group ReceivedGroup(GroupMessage.group.id(), GroupMessage.bot.id); // 实例化接收到的群对象
          if(GroupMessage.group.id() == 604890935 && std::find(std::begin(Admins),std::end(Admins),GroupMessage.sender.id())) // 群消息来源：包子铺，只有管理员操作才能触发命令
          {
              if(GroupMessage.message == MessageChain(PlainText(".dismisskick"))) //.dismisskick 调用的命令
              {
                  if(isKickIgnoreEnable == 0)
                  {
                      isKickIgnoreEnable = 1;
                      GroupMessage.group.sendMessage(PlainText("已忽视踢出状态。\n机器人将不会监听成员被踢事件。"));
                      Logger::logger.info("[Operation]管理员操作:忽略踢出事件");
                  }
                  else
                  {
                      isKickIgnoreEnable = 0;
                      GroupMessage.group.sendMessage("已恢复成员踢出监听。");
                      Logger::logger.info("[Operation]管理员操作:恢复踢出事件");
                  }
              }
              if(GroupMessage.message == MessageChain(PlainText(".detectkick"))) //踢出检测是否启用
              {
                  if(isKickIgnoreEnable == 0)
                  {
                      GroupMessage.group.sendMessage("目前踢出监听正在运行。");
                  }
                  else
                  {
                      GroupMessage.group.sendMessage(("目前踢出监听未在运行。\n如需恢复，请使用 .dismisskick 恢复。"));
                  }
              }
          }
          if(GroupMessage.group.id() == 1070074632 || GroupMessage.group.id() == 181327275){
              if(GroupMessage.message == MessageChain(PlainText(".cpptest")))
              {
                  GroupMessage.group.sendMessage("测试成功。\n此消息由基于 C++ 的 MiraiCP 插件发出。");
                  Logger::logger.info("[Operation]基本命令:测试消息");
              }
              if(GroupMessage.message == MessageChain(PlainText(".checknick")))
              {
                  GroupMessage.group.sendMessage("正在读群成员信息，请稍等。（可能耗时较长）");
                  std::fstream CheckNickLog("C:\\CheckNickLog.txt",std::ios_base::in | std::ios_base::out);
                  std::string FirstLine = "群成员名片检测操作:" ;
                  CheckNickLog << FirstLine << std::endl;//写出操作时间到日志
                  std::vector<unsigned long long>MemberList = Baozipu.getMemberList();
                  GroupMessage.group.sendMessage("获取群成员元素完毕，当前群成员数为 " + std::to_string(MemberList.size()) + "\n已启动检测任务。");
                  for(int i=0;i<=MemberList.size();i++)
                  {
                      MiraiCP::Member DetectedMember(MemberList[i],Baozipu.id(),2651987825); //对检测目标构建对象
                      if(DetectedMember.nickOrNameCard().find("游戏ID:") == std::string::npos && DetectedMember.nickOrNameCard().find("游戏ID：") == std::string::npos && DetectedMember.nickOrNameCard().find("[无账号]") == std::string::npos)
                      {
                          std::string OutputResult = "检测到不合规的群名片。位置：" + std::to_string(i) + " ,成员号: " + std::to_string(DetectedMember.id()) + " ,群名片为: " + DetectedMember.nickOrNameCard();
                          CheckNickLog << OutputResult << std::endl;
                      }
                  }
                  CheckNickLog.close();
                  Group FileTargetGroup(GroupMessage.group.id(),GroupMessage.bot.id);
                  FileTargetGroup.sendFile("/","C:\\CheckNickLog.txt");
                  Sleep(1000);
                  GroupMessage.group.sendMessage("检测结果已发送至群文件，请注意查收。");
              }
          }

      });
      Event::registerEvent<MemberJoinRequestEvent>([](MemberJoinRequestEvent JoinRequestEvent){ //成员申请入群的事件注册
          if(JoinRequestEvent.group == Baozipu)
          {
              //目前无可靠方法获取入群玩家的附言，暂时不设置转发。
              std::string MemberID = std::to_string(JoinRequestEvent.requesterId);
              Notify.sendMessage("检测到包子铺入群申请，请及时处理!\n申请者:" + MemberID);
          }
      });
      Event::registerEvent<MemberLeaveEvent>([](MemberLeaveEvent LeaveEvent){ //群成员离开事件监听
          if(LeaveEvent.group.id() == 604890935 && isKickIgnoreEnable == 0)
          {
              std::string MemberID = std::to_string(LeaveEvent.memberid);
              std::string LeaveText;
              if(LeaveEvent.type == 1) // Type= 1 - 被踢；2 - 自己退群。
              {
                  LeaveText = " 被送了飞机票。";
              }
              else
              {
                  LeaveText = " 离开了包子铺。";

              }
              LeaveEvent.group.sendMessage(MemberID + LeaveText);
              Notify.sendMessage("检测到包子铺离开事件：\n" + MemberID + LeaveText);
          }
      });
      Event::registerEvent<MemberJoinEvent>([](MemberJoinEvent JoinedEvent){
          Sleep(1500); // 延迟 1500 ms 后操作，以免风控
          JoinedEvent.group.sendMessage("");
      });
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
