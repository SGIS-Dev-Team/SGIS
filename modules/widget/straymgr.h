#ifndef STrayMgr_H
#define STrayMgr_H

//  托盘管理类
//  STrayMgr
//  用于创建托盘图标、创建托盘菜单、管理托盘菜单

#include <QSystemTrayIcon>
#include <QMenu>
#include <QStandardPaths>
#include "modules/slogger.h"
#include "modules/global.h"

class STrayManager:
    public QObject
{
    Q_OBJECT

    /*-----常量定义-----*/
public:
    //菜单选项
    enum TrayAction
    {
        Quit,           //退出
        MainWnd,       //显示/隐藏主窗口
        Maximize,       //最大化
        Status,         //激活状态

        Switch,         //切换样式
        Notify          //通知设置
    };

    /*-----构造函数与析构函数-----*/
public:
    STrayManager();
    STrayManager(const QString& configDir);
    virtual ~STrayManager();

    /*-----槽函数-----*/
private slots:
    //选项菜单触发响应
    void onActionStatusTriggered();
    void onActionNotifyTriggered();
    void onActionMainWndTriggered();
    void onActionMaximizeTriggered();
    //托盘图标激活响应
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

    /*-----信号-----*/
signals:


    /*-----成员变量-----*/
private:
    //系统托盘图标
    QSystemTrayIcon* mpTrayIcon{nullptr};
    //系统托盘菜单
    QMenu* mpTrayMenu{nullptr};

    //[托盘菜单选项]

    //基本菜单项
    QAction* mpActionQuit{nullptr};
    QAction* mpActionMainWnd{nullptr};
    QAction* mpActionMaximize{nullptr};
    QAction* mpActionStatus{nullptr};

    //扩展菜单项
    QAction* mpActionSwitch{nullptr};
    QAction* mpActionNotify{nullptr};

    //[计数变量与状态变量]
    //样式数
    int mnStyle{};
    //显示主窗口
    bool mbShowMainWnd{};
    //托盘状态
    bool mbActive{};
    //通知模式
    bool mbNotify{};

    //[配置文件路径]
    QString mStrConfigDir{};

    //[Me]
    QString Me = "Tray_Icon_Manager";


    /*-----成员函数-----*/
public:
    /*文件IO与初始化*/
    void initialize();
    //配置文件读取
    void loadConfig(const QString& dir);
    //配置文件写入
    void saveConfig(const QString& dir)const;

    /*访问函数*/

    //是否显示主窗口
    bool isShowingMainWnd()const;
    //是否处于激活状态
    bool isActive()const;
    //是否发送通知
    bool isNotifOn()const;
    //已添加的样式个数
    int styleCount()const;
    //获取当前图标
    QIcon currentIcon()const;

    /*控制函数*/

    //菜单选项访问函数
    QAction* getActionPtr(TrayAction ta);
    //桌面通知
    void sendNotification(const QString& title, const QString& body)const;
    //改变激活状态
    void setActive(bool active);
    //更改配置文件路径
    void setConfigPath(const QString& configFilePath);
};

#endif // STrayMgr_H
