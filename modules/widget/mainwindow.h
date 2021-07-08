#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "modules/global.h"
#include "modules/widget/straymgr.h"
#include "modules/widget/seditor.h"
#include "modules/widget/globe/sglobe.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE


//

class MainWindow : public QMainWindow
{
    Q_OBJECT

    /*-----构造与析构函数-----*/
public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

    /*-----虚函数重载-----*/
public:
    //窗口关闭事件重载
    void closeEvent(QCloseEvent* event)override;
    //窗口显示状态变化事件重载
    void setVisible(bool visible)override;
    //窗口状态变化事件重载
    void changeEvent(QEvent* event)override;

    /*-----信号-----*/

    /*-----槽函数-----*/
private slots:
    //托盘菜单切换主窗口显示状态选项触发响应
    void onTrayMenuActionMainWndTriggered();
    //编辑器按钮单击响应
    void onButtonEditorClicked();
    //数字地球按钮单击响应
    void onButtonGlobeClicked();
    //编辑器关闭事件响应
    void onEditorClosed();
    //编辑器初始化完毕
    void onEditorInitComplete();
    //数字地球关闭事件响应
    void onGlobeClosed();
    //数字地球初始化完毕
    void onGlobeInitComplete();

    /*-----成员变量-----*/
private:
    //托盘配置文件路径
    QString mStrTrayConfigDir;
    //托盘图标
    STrayManager* mpTrayIconMgr;
    //[Me]
    QString Me = "Main_Window";

    /*-----主窗口管理的窗口-----*/
private:
    SEditor* mpEditor{nullptr};
    SGlobe* mpGlobe{nullptr};

private:

    /*-----成员函数-----*/
private:
    //[界面与托盘初始化]
    void _initialize();
    void _initializeConnections();
    void _connectEditor();
    void _connectGlobe();

    //UI界面
private:
    Ui::MainWindow* ui;
};


#endif // MAINWINDOW_H
