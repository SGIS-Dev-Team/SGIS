#ifndef SEDITOR_H
#define SEDITOR_H

#include <QMainWindow>
#include "qcanvasarea.h"
#include"slogger.h"
#include"qcanvas.h"
#include<QLabel>

namespace Ui
{
class SEditor;
}

class SEditor : public QMainWindow
{
    Q_OBJECT
    /*-----构造函数与析构函数-----*/
public:
    explicit SEditor(SLogger* _logger, QWidget *parent = nullptr);
    ~SEditor();

    /*-----信号-----*/
signals:
    void closed();

    /*-----槽函数-----*/
private slots:
    //[菜单选项响应]
    void onActionZoominTriggered();
    void onActionZoomoutTriggered();
    //[切换绘图区响应]
    void onTabSwitched();
    //[画布事件响应]
    //鼠标移动
    void onCanvasMouseMoved(QPoint Log_pos);
    //缩放
    void onCanvasScaled(double value);

    /*-----虚函数重载-----*/
public:
    void closeEvent(QCloseEvent* event)override;


    /*-----成员变量-----*/
private:
    //日志记录器
    SLogger* gbLogger{};
    //[Me]
    QString Me = QString("SEditor");
    //[当前绘图区]
    QCanvasArea* mpCurCanvasArea{};
    //[已打开并加载的绘图区]
    std::vector<QCanvasArea*> mpCanvasAreaVec{};

    /*-----成员函数-----*/
public:
    //初始化函数
    void initialize();
    //创建新的绘图区
    void createTemplate(const QSize& CanvasSize = DEFAULT_CANVAS_SIZE);
    void createTemplate(const QString& templateFilePath);

    /*-----UI与控件-----*/
private:
    Ui::SEditor * ui;
    //[状态栏控件]
    QLabel* mpStatLblCursorPos;
    QLabel* mpStatLblCanvasScale;
};

#endif // SEditor_H
