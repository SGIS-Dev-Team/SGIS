#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "modules/global.h"
#include "modules/widget/straymgr.h"
#include "modules/widget/seditor.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

    /*-----构造与析构函数-----*/
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /*-----虚函数重载-----*/
public:
    //窗口关闭事件重载
    void closeEvent(QCloseEvent *event)override;
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
    //编辑器关闭事件响应
    void onEditorClosed();

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
    SEditor* mWndEditor{nullptr};


private:

    /*-----成员函数-----*/
private:
    //[界面与托盘初始化]
    void initialize();

    //UI界面
private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
