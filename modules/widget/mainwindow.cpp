#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCloseEvent>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _initialize();
}

MainWindow::~MainWindow()
{
    //释放托盘图标管理器
    delete mpTrayIconMgr;
    delete mEditor;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    //隐藏主窗口
    emit mpTrayIconMgr->getActionPtr(STrayManager::MainWnd)->triggered();
    event->ignore();
}

void MainWindow::setVisible(bool visible)
{
    //在窗口显示状态改变时改变托盘菜单选项可用性
    mpTrayIconMgr->getActionPtr(STrayManager::Maximize)->setEnabled(!isMaximized() || !visible);

    QWidget::setVisible(visible);
}

void MainWindow::changeEvent(QEvent* event)
{
    //判断窗口是否最大化
    if (event->type() == QEvent::WindowStateChange)
        mpTrayIconMgr->getActionPtr(STrayManager::Maximize)->setEnabled(!isMaximized());
}

void MainWindow::onTrayMenuActionMainWndTriggered()
{
    setVisible(mpTrayIconMgr->isShowingMainWnd());
}

void MainWindow::onButtonEditorClicked()
{
    //初始化编辑器类
    if (!mEditor)
    {
        mEditor = new SEditor(nullptr);
        _connectEditor();
    }

    mEditor->show();

    this->hide();
    //停用托盘菜单
    mpTrayIconMgr->getActionPtr(STrayManager::MainWnd)->setEnabled(false);
    mpTrayIconMgr->getActionPtr(STrayManager::Maximize)->setEnabled(false);
    mpTrayIconMgr->getActionPtr(STrayManager::Switch)->setEnabled(false);
}

void MainWindow::onEditorClosed()
{
    //显示主窗口
    this->show();
    //启用托盘菜单
    mpTrayIconMgr->getActionPtr(STrayManager::MainWnd)->setEnabled(true);
    mpTrayIconMgr->getActionPtr(STrayManager::Maximize)->setEnabled(true);
    mpTrayIconMgr->getActionPtr(STrayManager::Switch)->setEnabled(true);
}

void MainWindow::onEditorInitComplete()
{
    this->hide();
}

void MainWindow::_initialize()
{
    /*-----初始化主界面-----*/
    //设置为无边框窗口
    //this->setWindowFlags(Qt::ToolTip);

    /*-----初始化托盘图标-----*/
    mStrTrayConfigDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/sgif";
    mpTrayIconMgr = new STrayManager(mStrTrayConfigDir);

    //启动完成，打开主窗口或通知用户
    if (mpTrayIconMgr->isShowingMainWnd())
        this->show();
    else if (mpTrayIconMgr->isNotifOn())
        mpTrayIconMgr->sendNotification(tr("S-GIS"), tr("S-GIS initialization complete. Click the tray icon to show the splash screen."));

    /*-----测试用-----*/
    //打开编辑器，隐藏主窗口
    //onButtonEditorClicked();

    _initializeConnections();
}

void MainWindow::_initializeConnections()
{
    //连接编辑器
    if (mEditor) _connectEditor();
    //链接托盘菜单响应事件
    connect(mpTrayIconMgr->getActionPtr(STrayManager::Quit), &QAction::triggered, qApp, &QApplication::quit);
    connect(mpTrayIconMgr->getActionPtr(STrayManager::MainWnd), &QAction::triggered, this, &MainWindow::onTrayMenuActionMainWndTriggered);
    connect(mpTrayIconMgr->getActionPtr(STrayManager::Maximize), &QAction::triggered, this, &QWidget::showMaximized);
    //链接界面按钮事件响应
    connect(ui->mButtonEditor, &QPushButton::clicked, this, &MainWindow::onButtonEditorClicked);
}

void MainWindow::_connectEditor()
{
    connect(mEditor, &SEditor::initComplete, this, &MainWindow::onEditorInitComplete);
    connect(mEditor, &SEditor::closed, this, &MainWindow::onEditorClosed);
}

