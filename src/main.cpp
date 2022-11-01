// MiraiCP依赖文件(只需要引入这一个)
#define _CRT_SECURE_NO_WARNINGS 1
#include <MiraiCP.hpp>
#include <fstream>
#include "windows.h"
#include "time.h"
#include "simpleINI.h"
using namespace MiraiCP;

const PluginConfig CPPPlugin::config{
        "cn.xiaobaomc.group",          // 插件id
        "Xiaobao-QQGroup-Plugin",        // 插件名称
        "1.0.0",            // 插件版本
        "DreamUniverse843",        // 插件作者
        "Whatelse~",  // 可选：插件描述
        "2022-10-18"        // 可选：日期
};

int isAbnormalStopChecked = 0;
//是否启用监听踢出（全局变量）
int isKickIgnoreEnable = 0;
//常量 - 管理员数组，如发出者账号为管理员则执行对应命令
const long long int Admins[10]={3212903564,344731666,1254207361,767607252,437333682,3476768863,2378612355,3100298105,2720844086};
//是否取消群成员清理操作(标记使用)
int isCleanCancelled = 0;
CSimpleIniA RobotINI;//ini 文件操作对象
const std::string iniPath = "C:\\Users\\Server\\Desktop\\Mirai.ini";

void iniInitialize(std::string FilePath) //初始化配置文件存储
{
    RobotINI.SetUnicode(); // 默认使用 UTF-8 编码
    RobotINI.LoadFile(FilePath.c_str());
    RobotINI.SetValue("Initialize","True","1");
    RobotINI.SaveFile(FilePath.c_str());
    Logger::logger.info("[System]启动阶段读取 ini 文件请求成功。");
}

std::string iniQuery(std::string FilePath,std::string Section,std::string Name) //查询配置文件内的键值
{
    RobotINI.SetUnicode();
    RobotINI.LoadFile(FilePath.c_str());
    const char * ReturnValue = RobotINI.GetValue(Section.c_str(),Name.c_str(),"未找到");
    Logger::logger.info("[INISystem]查询字段请求发送成功。");
    int ReturnCode = RobotINI.SaveFile(FilePath.c_str());
    if(ReturnCode < 0)
    {
        Logger::logger.info("[INISystem]文件写入出现问题。");
        return "未找到";
    }
    else
    {
        Logger::logger.info("[INISystem]文件写入成功。");
        return ReturnValue;
    }
}

void iniWrite(std::string FilePath,std::string Section,std::string Name,std::string Value)
{
    RobotINI.SetUnicode();
    RobotINI.LoadFile(FilePath.c_str());
    RobotINI.SetValue(Section.c_str(),Name.c_str(),Value.c_str());
    Logger::logger.info("[INISystem]变更字段请求发送成功。");
    int ReturnCode = RobotINI.SaveFile(FilePath.c_str());
    if(ReturnCode < 0)
    {
        Logger::logger.info("[INISystem]文件写入出现问题。");
    }
    else
    {
        Logger::logger.info("[INISystem]文件写入成功。");
    }
}
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
      Logger::logger.info("[System]初始化黑名单数据文件中");
      iniInitialize(iniPath); //初始化数据
      if(iniQuery(iniPath,"Initialize","isShutdownNormally") == "0")
      {
          Logger::logger.error("[Error]检测到插件未正常退出(是否直接关掉了终端，还是发生了 JVM 崩溃？)");
          isAbnormalStopChecked = 1;
      }
      iniWrite(iniPath,"Initialize","isShutdownNormally","0");
      Event::registerEvent<BotOnlineEvent>([](BotOnlineEvent OnlineEvent) {
          Group Notify(181327275,OnlineEvent.bot.id);
          Group Lianhehui(1070074632,OnlineEvent.bot.id);
          if(isAbnormalStopChecked == 1)
          {
              Notify.sendMessage("机器人已从异常错误中恢复。(可能是直接关闭终端或发生了 JVM 崩溃)\n检测到上一次发生了异常退出，请查询后台日志了解详情。");
              Sleep(1300);
              Lianhehui.sendMessage("机器人已从异常错误中恢复。(可能是直接关闭终端或发生了 JVM 崩溃)\n检测到上一次发生了异常退出，请查询后台日志了解详情。");
              isAbnormalStopChecked = 0;
          }
      });
      Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent GroupMessage) { //群内消息的事件注册
          Group ReceivedGroup(GroupMessage.group.id(), GroupMessage.bot.id); // 实例化接收到的群对象
          Group Baozipu(604890935,GroupMessage.bot.id);
          Group Notify(181327275,GroupMessage.bot.id);
          Group Lianhehui(1070074632,GroupMessage.bot.id);
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
          if(GroupMessage.group.id() == 1070074632 || GroupMessage.group.id() == 181327275 || GroupMessage.group.id() == 604890935){
              if(GroupMessage.message == MessageChain(PlainText(".cpptest")))
              {
                  GroupMessage.group.sendMessage("测试成功。\n此消息由基于 C++ 的 MiraiCP 插件发出。");
                  Logger::logger.info("[Operation]基本命令:测试消息");
              }
              if(GroupMessage.message == MessageChain(PlainText(".checknick")))
              {
                  int InvalidNickUserCount=0;
                  Logger::logger.info("[Operation]管理员操作:检测群名片。");
                  auto now = std::chrono::system_clock::now();
                  time_t tt = std::chrono::system_clock::to_time_t(now);
                  auto time_tm = localtime(&tt);
                  char strTime[25] = { 0 };
                  sprintf(strTime, "%d-%02d-%02d_%02d-%02d-%02d", time_tm->tm_year + 1900,
                          time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_hour,
                          time_tm->tm_min, time_tm->tm_sec);
                  std::string CurrentTime(strTime);
                  Logger::logger.info("[System]当前时间是 " + CurrentTime);
                  GroupMessage.group.sendMessage("正在读群成员信息，请稍等。\n当前时间: " + CurrentTime);
                  Sleep(1000);
                  std::ofstream CheckNickLog("C:\\Users\\Server\\Desktop\\CheckNickLog.txt",std::ios_base::out | std::ios_base::trunc);
                  std::string FirstLine = "群成员名片检测操作，请求时间: ";
                  CheckNickLog << FirstLine << strTime << std::endl;//写出操作时间到日志
                  std::vector<unsigned long long>MemberList = Baozipu.getMemberList();
                  GroupMessage.group.sendMessage("获取群成员元素完毕，当前群成员数为 " + std::to_string(MemberList.size()) + "\n已启动检测任务。");
                  Logger::logger.info("[System]群成员清单拉取完毕，长度 " + std::to_string(MemberList.size()));
                  for(int i=0;i<=MemberList.size()-1;i++)
                  {
                      MiraiCP::Member DetectedMember(MemberList.at(i),Baozipu.id(),GroupMessage.bot.id); //对检测目标构建对象
                      if(DetectedMember.nickOrNameCard().find("游戏ID:") == std::string::npos
                            && DetectedMember.nickOrNameCard().find("游戏ID：") == std::string::npos
                            && DetectedMember.nickOrNameCard().find("[无账号]") == std::string::npos
                            && DetectedMember.nickOrNameCard().find("游戏id：") == std::string::npos
                            && DetectedMember.nickOrNameCard().find("游戏id:") == std::string::npos)
                      {
                          std::string OutputResult = "检测到不合规的群名片。位置：" + std::to_string(i) + " ,成员号: " + std::to_string(DetectedMember.id()) + " ,群名片为: " + DetectedMember.nickOrNameCard();
                          CheckNickLog << OutputResult << std::endl;
                          InvalidNickUserCount++;
                      }
                  }
                  CheckNickLog << "\n检测完毕，检测到的不合规群名片数: " + std::to_string(InvalidNickUserCount) << std::endl;
                  CheckNickLog << "此文件由包子铺群机器人系统自动生成。"<< std::endl;
                  CheckNickLog.close();
                  RemoteFile tmp = GroupMessage.group.sendFile("/CheckNickLog-"+CurrentTime+".txt","C:\\Users\\Server\\Desktop\\CheckNickLog.txt");
                  Logger::logger.info("[Operation]文件发送操作：发送群名片检测日志");
                  Sleep(1000);
                  GroupMessage.group.sendMessage("检测完毕，检测到不合规群名片数:"+std::to_string(InvalidNickUserCount)+"\n结果已发送至上方群文件。");
              }
              if(GroupMessage.message == MessageChain(PlainText(".cleancancel")))
              {
                  isCleanCancelled = 1;
                  GroupMessage.group.sendMessage("清理操作取消指令已发出。");
              }
              if(GroupMessage.message == MessageChain(PlainText(".cleannick")))
              {
                  int CleanedUser = 0; //标记已清理的成员数
                  Logger::logger.info("[Operation]管理员操作:清理不合规群名片的群成员。");
                  auto now = std::chrono::system_clock::now();
                  //通过不同精度获取相差的毫秒数
                  time_t tt = std::chrono::system_clock::to_time_t(now);
                  auto time_tm = localtime(&tt);
                  char strTime[25] = { 0 };
                  sprintf(strTime, "%d-%02d-%02d_%02d-%02d-%02d", time_tm->tm_year + 1900,
                          time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_hour,
                          time_tm->tm_min, time_tm->tm_sec);
                  std::string CurrentTime(strTime);
                  Logger::logger.info("[System]当前时间是 " + CurrentTime);
                  isCleanCancelled = 0;
                  Sleep(500);
                  std::ofstream CheckNickLog("C:\\Users\\Server\\Desktop\\CleanNickLog.txt",std::ios_base::out | std::ios_base::trunc);
                  std::string FirstLine = "群成员名片清理操作，请求时间: ";
                  CheckNickLog << FirstLine << strTime << std::endl;//写出操作时间到日志
                  GroupMessage.group.sendMessage("十秒后将执行不合规成员清理操作。\n如取消操作，请输入 .cleancancel。\n因线程限制，执行期间无法取消。");
                  Sleep(10000);
                  if(isCleanCancelled == 0)
                  {
                      GroupMessage.group.sendMessage("清理任务已启动。");
                      std::vector<unsigned long long>MemberList = Baozipu.getMemberList();
                      Logger::logger.info("[System]群成员清单拉取完毕，长度 " + std::to_string(MemberList.size()));
                      Sleep(300);
                      GroupMessage.group.sendMessage("获取群成员元素完毕，当前群成员数为 " + std::to_string(MemberList.size()));
                      Sleep(1500);
                      for(int i=0;i<=MemberList.size()-1;i++)
                      {
                          if(isCleanCancelled == 0)
                          {
                              MiraiCP::Member DetectedMember(MemberList.at(i),Baozipu.id(),GroupMessage.bot.id); //对检测目标构建对象
                              if(DetectedMember.nickOrNameCard().find("游戏ID:") == std::string::npos
                                 && DetectedMember.nickOrNameCard().find("游戏ID：") == std::string::npos
                                 && DetectedMember.nickOrNameCard().find("[无账号]") == std::string::npos
                                 && DetectedMember.nickOrNameCard().find("游戏id：") == std::string::npos
                                 && DetectedMember.nickOrNameCard().find("游戏id:") == std::string::npos)
                              {
                                  try { //踢出时捕获异常，以免崩端
                                      DetectedMember.kick("您的群名片不合乎格式要求。");
                                      std::string OutputResult = "检测到不合规的群名片，正在清理。位置：" + std::to_string(i) + " ,成员号: " + std::to_string(DetectedMember.id()) + " ,群名片为: " + DetectedMember.nickOrNameCard();
                                      CheckNickLog << OutputResult << std::endl;
                                      CleanedUser++;
                                      Sleep(3200);
                                  }
                                  catch (BotException &err){
                                      Logger::logger.error(err.what());
                                  }
                                  catch (MemberException &err){
                                      if(err.type == 1)
                                      {
                                          Logger::logger.error("[Error]执行时未找到目标群。");
                                      }
                                      if(err.type == 2)
                                      {
                                          std::string ErrorStr = "[Error]执行踢出成员 " + std::to_string(DetectedMember.id()) + " (执行位置: " + std::to_string(i) +")时未找到目标群成员。(可能已经提前退群?)";
                                          Logger::logger.error(ErrorStr);
                                          GroupMessage.group.sendMessage(ErrorStr);
                                          CheckNickLog << ErrorStr << std::endl;
                                      }
                                  }

                              }
                          }
                          else
                          {
                              GroupMessage.group.sendMessage("已取消群成员清理。操作数: " + std::to_string(CleanedUser));
                              CheckNickLog << "\n已取消群成员清理。已操作 " + std::to_string(CleanedUser) + "个不合规群成员，操作结束于执行位置 " + std::to_string(i) << std::endl;
                              isCleanCancelled = 0;
                              break;
                          }
                      }
                  }
                  else
                  {
                      GroupMessage.group.sendMessage("已检测到任务取消。");
                      CheckNickLog << "任务在开始执行前就被取消。" << std::endl;
                      isCleanCancelled = 0;
                  }
                  CheckNickLog << "\n不合规群名片成员清理任务已完成。清理了 " + std::to_string(CleanedUser) + "位群成员。"<< std::endl;
                  CheckNickLog << "此文件由包子铺群机器人系统自动生成。" << std::endl;
                  CheckNickLog.close();
                  RemoteFile tmp = GroupMessage.group.sendFile("/CleanNickLog-"+CurrentTime+".txt","C:\\Users\\Server\\Desktop\\CleanNickLog.txt");
                  Logger::logger.info("[Operation]文件发送操作：发送群名片清理日志");
                  Sleep(1000);
                  GroupMessage.group.sendMessage("清理完毕，清理了 "+ std::to_string(CleanedUser)+" 位群成员。\n结果已发送至上方群文件。");
              }
              if(GroupMessage.message == MessageChain(PlainText(".testMiraiCode")))
              {
                  GroupMessage.group.sendMessage("提示：此命令仅供开发用途。\n此条消息的 MiraiCode 格式为:\n" + GroupMessage.message.toMiraiCode());
              }
              if(GroupMessage.message.toMiraiCode().substr(0,10) == ".queryban ")
              {
                  std::string QueryMember = GroupMessage.message.toMiraiCode().erase(0, 10);
                  std::string ResultMsg = iniQuery(iniPath,"banlist", QueryMember);
                  GroupMessage.group.sendMessage("对 "+ QueryMember + " 的封禁查询：\n" + ResultMsg);
              }
              if(GroupMessage.message.toMiraiCode().substr(0,12) == ".queryleave ")
              {
                  std::string QueryMember = GroupMessage.message.toMiraiCode().erase(0, 12);
                  std::string ResultMsg = iniQuery(iniPath,"LeaveRecord", QueryMember);
                  GroupMessage.group.sendMessage("对 "+ QueryMember + " 的离群查询：\n" + ResultMsg);
              }
              if(GroupMessage.message.toMiraiCode().substr(0,5) == ".ban ")
              {
                  std::string TargetMember = GroupMessage.message.toMiraiCode().erase(0,5);
                  iniWrite(iniPath,"banlist",TargetMember,"True");
                  GroupMessage.group.sendMessage("已将 " + TargetMember + " 标记为入群封禁。");
              }
              if(GroupMessage.message.toMiraiCode().substr(0,7) == ".unban ")
              {
                  std::string TargetMember = GroupMessage.message.toMiraiCode().erase(0,7);
                  iniWrite(iniPath,"banlist",TargetMember,"False");
                  GroupMessage.group.sendMessage("已将 " + TargetMember + " 解除了入群封禁。");
              }
          }
          
      });
      Event::registerEvent<MemberJoinRequestEvent>([](MemberJoinRequestEvent JoinRequestEvent){ //成员申请入群的事件注册
          Group Baozipu(604890935,JoinRequestEvent.bot.id);
          Group Notify(181327275,JoinRequestEvent.bot.id);
          Group Lianhehui(1070074632,JoinRequestEvent.bot.id);
          if(JoinRequestEvent.group == Baozipu)
          {
              //入群申请监听。目前无可靠方法获取入群玩家的附言，暂时不设置转发。
              std::string RequestID = std::to_string(JoinRequestEvent.requesterId);
              std::string QueryBanResult = iniQuery(iniPath,"banlist", RequestID);
              std::string QueryLeaveRecord = iniQuery(iniPath,"LeaveRecord",RequestID);
              Logger::logger.info("[Info]入群申请事件: " + RequestID);
              if(QueryBanResult == "True")
              {
                  Logger::logger.info("[Info]检测的目标成员 " + RequestID + " 已被封禁，自动拒绝申请。");
                  Sleep(800);
                  JoinRequestEvent.reject("您已被管理员封禁，拒绝加群。");
                  Sleep(800);
                  Notify.sendMessage("检测到已被封禁的成员申请入群，已自动拒绝。\n申请者:" + RequestID + "\n离群记录:" + QueryLeaveRecord);
              }
              else
              {
                  Logger::logger.info("[Info]检测的目标成员 " + RequestID + " 无封禁信息。");
                  Sleep(800);
                  Notify.sendMessage("检测到包子铺入群申请，请及时处理!\n申请者:" + RequestID + "\n离群记录:" + QueryLeaveRecord);
              }
          }
      });
      Event::registerEvent<MemberLeaveEvent>([](MemberLeaveEvent LeaveEvent){ //群成员离开事件监听
          Group Baozipu(604890935,LeaveEvent.bot.id);
          Group Notify(181327275,LeaveEvent.bot.id);
          Group Lianhehui(1070074632,LeaveEvent.bot.id);
          auto now = std::chrono::system_clock::now();
          //通过不同精度获取相差的毫秒数
          time_t tt = std::chrono::system_clock::to_time_t(now);
          auto time_tm = localtime(&tt);
          char strTime[25] = { 0 };
          sprintf(strTime, "%d-%02d-%02d %02d:%02d:%02d", time_tm->tm_year + 1900,
                  time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_hour,
                  time_tm->tm_min, time_tm->tm_sec);
          std::string CurrentTime(strTime);
          if(LeaveEvent.group.id() == 604890935 && isKickIgnoreEnable == 0)
          {
              std::string MemberID = std::to_string(LeaveEvent.memberid);
              std::string LeaveText;
              std::string LeaveType;
              if(LeaveEvent.type == 1) // Type= 1 - 被踢；2 - 自己退群。
              {
                  LeaveText = " 被送了飞机票。";
                  LeaveType = "(被踢出)";
              }
              else
              {
                  LeaveText = " 离开了包子铺。";
                  LeaveType = "(主动离开)";
              }
              LeaveEvent.group.sendMessage(MemberID + LeaveText);
              std::string ResponseTmp = iniQuery(iniPath,"LeaveRecord",MemberID);
              if(ResponseTmp == "未找到")
              {
                  iniWrite(iniPath,"LeaveRecord", MemberID, CurrentTime + LeaveType);
              }
              else
              {
                  iniWrite(iniPath,"LeaveRecord",MemberID,ResponseTmp + "," + CurrentTime + LeaveType);
              }
              Logger::logger.info("[Info]离群事件记录: " + MemberID + LeaveType);
          }
      });
      Event::registerEvent<MemberJoinEvent>([](MemberJoinEvent JoinedEvent){
          if(JoinedEvent.group.id() == 604890935) // 只处理包子铺群事件
          {
              Sleep(650);
              JoinedEvent.group.sendMessage(iniQuery(iniPath,"Initialize","Announce"));
          }

      });

  }

  // 退出函数
  void onDisable() override {
    /*插件结束前执行*/
    Logger::logger.info("[Info]插件正在关闭。");
      iniWrite(iniPath,"Initialize","isShutdownNormally","1");
  }
};

// 绑定当前插件实例
void MiraiCP::enrollPlugin() {
  MiraiCP::enrollPlugin(new Main);
}
