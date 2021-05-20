#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initialize();
}

MainWindow::~MainWindow()
{
    //释放托盘图标管理器
    delete mpTrayIconMgr;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //隐藏主窗口
    emit mpTrayIconMgr->getActionPtr(STrayMgr::MainWnd)->triggered();
    event->ignore();
}

void MainWindow::setVisible(bool visible)
{
    //在窗口显示状态改变时改变托盘菜单选项可用性
    mpTrayIconMgr->getActionPtr(STrayMgr::Maximize)->setEnabled(!isMaximized() || !visible);

    QWidget::setVisible(visible);
}

void MainWindow::changeEvent(QEvent *event)
{
    //判断窗口是否最大化
    if(event->type() == QEvent::WindowStateChange)
        mpTrayIconMgr->getActionPtr(STrayMgr::Maximize)->setEnabled(!isMaximized());
}

void MainWindow::onTrayMenuActionMainWndTriggered()
{
    setVisible(mpTrayIconMgr->isShowingMainWnd());
}

void MainWindow::onButtonEditorClicked()
{
    //初始化编辑器类
    if(!mWndEditor)
    {
        mWndEditor = new SEditor(nullptr);
        connect(mWndEditor, &SEditor::closed, this, &MainWindow::onEditorClosed);
    }

    mWndEditor->show();

    this->hide();
    //停用托盘菜单
    mpTrayIconMgr->getActionPtr(STrayMgr::MainWnd)->setEnabled(false);
    mpTrayIconMgr->getActionPtr(STrayMgr::Maximize)->setEnabled(false);
    mpTrayIconMgr->getActionPtr(STrayMgr::Switch)->setEnabled(false);
}

void MainWindow::onEditorClosed()
{
    //显示主窗口
    this->show();
    //启用托盘菜单
    mpTrayIconMgr->getActionPtr(STrayMgr::MainWnd)->setEnabled(true);
    mpTrayIconMgr->getActionPtr(STrayMgr::Maximize)->setEnabled(true);
    mpTrayIconMgr->getActionPtr(STrayMgr::Switch)->setEnabled(true);
}

void MainWindow::initialize()
{
    /*-----初始化主界面-----*/
    //连接编辑器
    connect(ui->mButtonEditor, &QPushButton::clicked, this, &MainWindow::onButtonEditorClicked);

    /*-----初始化托盘图标-----*/
    mStrTrayConfigDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/sgif";
    mpTrayIconMgr = new STrayMgr(mStrTrayConfigDir);
    //链接响应事件
    connect(mpTrayIconMgr->getActionPtr(STrayMgr::Quit), &QAction::triggered, qApp, &QApplication::quit);
    connect(mpTrayIconMgr->getActionPtr(STrayMgr::MainWnd), &QAction::triggered, this, &MainWindow::onTrayMenuActionMainWndTriggered);
    connect(mpTrayIconMgr->getActionPtr(STrayMgr::Maximize), &QAction::triggered, this, &QWidget::showMaximized);
    //启动完成，打开主窗口或通知用户
    if(mpTrayIconMgr->isShowingMainWnd())
        this->show();
    else if(mpTrayIconMgr->isNotifOn())
        mpTrayIconMgr->sendNotification(tr("S-GIS"), tr("S-GIS initialization complete. Click the tray icon to show the main window."));

    /*-----测试用-----*/
    //打开编辑器，隐藏主窗口
    onButtonEditorClicked();
}

