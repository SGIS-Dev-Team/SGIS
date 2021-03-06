#ifndef QCANVASAREA_H
#define QCANVASAREA_H
#include "modules/global.h"
#include "QThread"
#include <QScrollArea>
#include "modules/widget/qcanvas.h"

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
    //使用默认尺寸创建新绘图区
    explicit QCanvasArea(QWidget *parent = Q_NULLPTR);
    virtual ~QCanvasArea();

    /*-----虚函数重载-----*/
public:
    //画布区域鼠标滚轮事件
    void wheelEvent(QWheelEvent* event)override;

    /*-----信号-----*/
signals:
    //颜色改变信号
    void colorChanged(QColor color);
    /*-----槽函数-----*/
private slots:
    //画布缩放信号响应
    void onCanvasScaling(QPointF lgcPos, int delta);
    //滚动条位置改变信号响应
    void onSliderValueChange(int value);


    /*-----成员变量-----*/
private:
    //画布
    QCanvas *mpCanvas{};
    //画布尺寸
    QSize mCanvasSize{};

    /*-----成员函数-----*/
public:
    //[访问函数]
    QCanvas * canvas()const;
    //获取视图区逻辑坐标矩形
    QRectF viewArea()const;

    //[功能函数]
private:
    void _initialize();
};

#endif // QCANVASAREA_H









