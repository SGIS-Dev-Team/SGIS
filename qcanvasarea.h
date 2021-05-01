#ifndef QCANVASAREA_H
#define QCANVASAREA_H
#include "global.h"
#include <QScrollArea>
#include "qcanvas.h"

//  QCanvasArea
//  绘图区
//  由滚动区域和内置画布组成

class QCanvasArea : public QScrollArea
{
    Q_OBJECT

    /*-----构造函数与析构函数-----*/
public:
    //指定画布尺寸创建新绘图区
    explicit QCanvasArea(const QSize& CanvasSize, QWidget *parent = Q_NULLPTR);
    //指定文档路径创建新绘图区
    explicit QCanvasArea(const QString& templateFilePath, QWidget *parent = Q_NULLPTR);
    //使用默认尺寸创建新绘图区
    explicit QCanvasArea(QWidget *parent = Q_NULLPTR);
    ~QCanvasArea();

    /*-----虚函数重载-----*/
public:
    //画布区域快捷键事件
    void keyPressEvent(QKeyEvent* event)override;
    //画布区域鼠标滚轮事件
    void wheelEvent(QWheelEvent* event)override;
    //画布区域鼠标移动事件
    void mouseMoveEvent(QMouseEvent* event)override;

    /*-----信号-----*/
signals:

    /*-----槽函数-----*/
private slots:
    void onCanvasScaling(QPoint lgcPos, int delta);

    /*-----成员变量-----*/
private:
    QCanvas* mpCanvas{};

    /*-----成员函数-----*/
public:
    //[访问函数]

    QCanvas *canvas();

    //[修改函数]

    void setCanvas(QCanvas* newCanvas);
};

#endif // QCANVASAREA_H









