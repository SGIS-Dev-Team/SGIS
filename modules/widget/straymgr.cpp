#include "straymgr.h"

STrayMgr::STrayMgr()
{
    loadConfig("");
}

STrayMgr::STrayMgr(const QString &configDir)
{
    mStrConfigDir = configDir;
    loadConfig(configDir);
}

STrayMgr::~STrayMgr()
{
    saveConfig(mStrConfigDir);
    delete mpTrayIcon;
    delete mpTrayMenu;

    delete mpActionQuit;
    delete mpActionMainWnd;
    delete mpActionMaximize;
    delete mpActionStatus;
    delete mpActionNotify;
    delete mpActionSwitch;
}

void STrayMgr::onActionStatusTriggered()
{
    //更改状态
    mbActive = !mbActive;
    //更换图标和文字
    QIcon actionIcon = mbActive ? QIcon(":/TrayActionIcon/TrayActionIcon/sleep.png") : QIcon(":/TrayActionIcon/TrayActionIcon/activate.png");
    QString text = mbActive ? tr("Sleep") : tr("Activate");
    mpActionStatus->setIcon(actionIcon);
    mpActionStatus->setText(text);
    //更改托盘图标
    QIcon trayIcon = mbActive ? QIcon(":/TrayIcon/TrayIcon/active.png") : QIcon(":/TrayIcon/TrayIcon/sleeping.png");
    mpTrayIcon->setIcon(trayIcon);
}

void STrayMgr::onActionNotifyTriggered()
{
    //更改状态
    mbNotify = !mbNotify;
    //更换图标和文字
    QIcon icon = mbNotify ? QIcon(":/TrayActionIcon/TrayActionIcon/notification_off.png") : QIcon(":/TrayActionIcon/TrayActionIcon/notification_on.png");
    QString text = mbNotify ? tr("Do not Disturb") : tr("Turn on Notification");
    mpActionNotify->setIcon(icon);
    mpActionNotify->setText(text);
}

void STrayMgr::onActionMainWndTriggered()
{
    //更改状态
    mbShowMainWnd = !mbShowMainWnd;
    //更换图标和文字
    QIcon icon = mbShowMainWnd ? QIcon(":/TrayActionIcon/TrayActionIcon/hide.png") : QIcon(":/TrayActionIcon/TrayActionIcon/show.png");
    QString text = mbShowMainWnd ? tr("Hide Window") : tr("Show Window");
    mpActionMainWnd->setIcon(icon);
    mpActionMainWnd->setText(text);
    //发送通知
    if(mbNotify && !mbShowMainWnd)
        sendNotification(tr("S-GIS"), tr("The program will keep running in the backgroud."));
}

void STrayMgr::onActionMaximizeTriggered()

{
    if(!mbShowMainWnd)
        onActionMainWndTriggered();
}

void STrayMgr::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    //左键单击————显示主窗口
    //左键双击————激活/睡眠 然后 隐藏主窗口
    switch (reason)
    {
    case QSystemTrayIcon::ActivationReason::Trigger:
        emit mpActionMainWnd->triggered();
        break;
    case QSystemTrayIcon::ActivationReason::DoubleClick:
        emit mpActionStatus->triggered();
        emit mpActionMainWnd->triggered();
        break;
    default:
        break;
    }
}

void STrayMgr::initialize()
{
    //选项初始化
    mpActionQuit = new QAction(QIcon(":/TrayActionIcon/TrayActionIcon/quit.png"), tr("Quit"));

    if(mbShowMainWnd)
        mpActionMainWnd = new QAction(QIcon(":/TrayActionIcon/TrayActionIcon/hide.png"), tr("Hide Window"));
    else
        mpActionMainWnd = new QAction(QIcon(":/TrayActionIcon/TrayActionIcon/show.png"), tr("Show Window"));

    mpActionMaximize = new QAction(QIcon(":/TrayActionIcon/TrayActionIcon/maximize.png"), tr("Maximize"));

    if(mbActive)
        mpActionStatus = new QAction(QIcon(":/TrayActionIcon/TrayActionIcon/sleep.png"), tr("Sleep"));
    else
        mpActionStatus = new QAction(QIcon(":/TrayActionIcon/TrayActionIcon/activate.png"), tr("Activate"));

    mpActionSwitch = new QAction(tr("Switch"));

    if(mbNotify)
        mpActionNotify = new QAction(QIcon(":/TrayActionIcon/TrayActionIcon/notification_off.png"), tr("Do not Disturb"));
    else
        mpActionNotify = new QAction(QIcon(":/TrayActionIcon/TrayActionIcon/notification_on.png"), tr("Turn on Notification"));

    //选项事件响应启动
    connect(mpActionStatus, &QAction::triggered, this, &STrayMgr::onActionStatusTriggered);
    connect(mpActionNotify, &QAction::triggered, this, &STrayMgr::onActionNotifyTriggered);
    connect(mpActionMainWnd, &QAction::triggered, this, &STrayMgr::onActionMainWndTriggered);
    connect(mpActionMaximize, &QAction::triggered, this, &STrayMgr::onActionMaximizeTriggered);

    //加载菜单
    mpTrayMenu = new QMenu();
    mpTrayMenu->addAction(mpActionSwitch);
    mpTrayMenu->addAction(mpActionNotify);
    mpTrayMenu->addAction(mpActionStatus);
    mpTrayMenu->addSeparator();
    mpTrayMenu->addAction(mpActionMaximize);
    mpTrayMenu->addAction(mpActionMainWnd);
    mpTrayMenu->addSeparator();
    mpTrayMenu->addAction(mpActionQuit);
    //初始化托盘图标
    if(mbActive)
        mpTrayIcon = new QSystemTrayIcon(QIcon(":/TrayIcon/TrayIcon/active.png"));
    else
        mpTrayIcon = new QSystemTrayIcon(QIcon(":/TrayIcon/TrayIcon/sleeping.png"));

    mpTrayIcon->setContextMenu(mpTrayMenu);
    //托盘图标激活事件响应启动
    connect(mpTrayIcon, &QSystemTrayIcon::activated, this, &STrayMgr::onTrayIconActivated);
    mpTrayIcon->show();
}

void STrayMgr::loadConfig(const QString &dir)
{
    QFile cfg(dir + TRAY_MENU_CFG_NAME);
    if(!cfg.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        mbActive = true;
        mbNotify = true;
        mnStyle = 0;
    }
    else
    {
        QString cfgStr = cfg.readAll();
        cfg.close();
        QTextStream cfgStream(&cfgStr);
        cfgStream.readLine();
        cfgStream.seek(cfgStream.pos() + strlen("# version: "));
        double version;
        cfgStream >> version;
        cfgStream.readLine();   //读掉换行符
        //判断版本号，如果是版本不同则使用默认值
        if(version != VER)
        {
            mbActive = mbNotify = mbShowMainWnd = true;
            mnStyle = 0;
        }
        else
        {
            cfgStream.readLine();   //读掉"# Begin System Tray Menu Config File"

            int active, notify, showMainWnd;

            cfgStream.seek(cfgStream.pos() + strlen("Activated = "));
            cfgStream >> active;

            cfgStream.readLine();   //读掉换行符
            cfgStream.seek(cfgStream.pos() + strlen("Notification = "));
            cfgStream >> notify;

            cfgStream.readLine();   //读掉换行符
            cfgStream.seek(cfgStream.pos() + strlen("Show Main Window = "));
            cfgStream >> showMainWnd;

            mbActive = bool(active);
            mbNotify = bool(notify);
            mbShowMainWnd = bool(showMainWnd);

            mnStyle = 0;
        }
    }
    initialize();
}

void STrayMgr::saveConfig(const QString &dir) const
{
    //打开文件
    QFile cfg(dir + TRAY_MENU_CFG_NAME);
    if(!cfg.open(QIODevice::WriteOnly | QIODevice::Text))
        qDebug() << "Failed to write configuration file at " + dir;
    //保存数据
    QTextStream cfgStream(&cfg);
    cfgStream << CFG_HEAD
              << VER_STR
              << "# Begin System Tray Menu Config File\n"
              << "Activated = " << mbActive << '\n'
              << "Notification = " << mbNotify << '\n'
              << "Show Main Window = " << mbShowMainWnd << '\n'
              << "# End of System Tray Menu Config File\n";
    cfg.close();
}

bool STrayMgr::isShowingMainWnd() const
{
    return mbShowMainWnd;
}

QAction *STrayMgr::getActionPtr(TrayAction ta)
{
    switch (ta)
    {
    case Quit:
        return mpActionQuit;
        break;
    case MainWnd:
        return mpActionMainWnd;
        break;
    case Maximize:
        return mpActionMaximize;
        break;
    case Status:
        return mpActionStatus;
        break;
    case Switch:
        return mpActionSwitch;
        break;
    case Notify:
        return mpActionNotify;
        break;
    default:
        break;
    }
    return nullptr;
}

bool STrayMgr::isActive() const
{
    return mbActive;
}

bool STrayMgr::isNotifOn() const
{
    return mbNotify;
}

int STrayMgr::styleCount() const
{
    return mnStyle;
}

QIcon STrayMgr::currentIcon() const
{
    return mpTrayIcon->icon();
}

void STrayMgr::sendNotification(const QString &title, const QString &body)const
{
    mpTrayIcon->showMessage(title, body, mpTrayIcon->icon());
}

