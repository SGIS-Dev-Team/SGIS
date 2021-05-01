#ifndef QCANVAS_H
#define QCANVAS_H
#include <QWidget>
#include "simage.h"
#include <global.h>

class QCanvas : public QWidget
{
    Q_OBJECT

    /*常量定义*/
private:
#define SCALE_LEVEL 24
    //画布缩放级别，从0-23级，第10级为100%
    const double ScaleLevelValue[SCALE_LEVEL] = {0.3e-2, 0.5e-2, 0.8e-2, 1.0e-2, 2.0e-2, 5.0e-2, 10.0e-2, 15.0e-2, 25.0e-2, 50.0e-2, 75.0e-2, 1.0,
                                                 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 12.0, 16.0, 32.0, 64.0, 128.0
                                                };
public:
    /*-----构造函数与析构函数-----*/
public:
    explicit QCanvas(QWidget *parent = nullptr, QSize canvasSize = QSize(800, 600));
    virtual ~QCanvas();

    /*-----虚函数重载-----*/
public:
    //画布绘图事件
    void paintEvent(QPaintEvent* event)override;
    //画布鼠标移动事件
    void mouseMoveEvent(QMouseEvent* event)override;
    //画布鼠标左键按下事件
    void mousePressEvent(QMouseEvent* event)override;
    //画布鼠标左键松开事件
    void mouseReleaseEvent(QMouseEvent* event)override;

    /*-----信号-----*/
signals:
    //鼠标移动
    void mouseMoved(QPoint pos);
    //缩放信号
    void scaled(double value);

    /*-----槽函数-----*/
private slots:


    /*-----控制标记-----*/
private:
    //是否处于选择模式
    bool mbSelectMode{false};
    //选中的绘图对象
    SObject *mObjSelected{};
    //拖动起始点
    QPoint mDragStartPt{};


    /*-----属性-----*/
private:
    //开启网格
    bool mbGridOn{true};
    //开启参考线显示
    bool mbRefLineOn{};
    //画布逻辑尺寸
    QSize mSzLogical{};
    //画布实际尺寸
    QSize mSzActual{};
    //缩放比例
    double mdScale{1.0};

    /*-----成员变量-----*/
private:

    /*-----成员函数-----*/
public:
    //[访问函数]

    //画布逻辑尺寸
    QSize logicalSize()const;
    //画布实际尺寸
    QSize actualSize()const;
    //缩放值
    double scaleValue()const;
    //网格是否开启
    bool isGridOn()const;
    //参考线是否开启
    bool isRefLineOn()const;

    //[更改函数]

    //按数值缩放画布实际尺寸
    bool setScaleValue(double value);
    //按级别缩放画布实际尺寸
    bool setScaleLevel(int level);
    //缩放级别增加
    bool setScaleLevelUp();
    //缩放级别减少
    bool setScaleLevelDown();

    //[功能函数]

    //逻辑坐标转实际坐标
    inline QPoint LtoA(QPoint pt_Logical)const;
    inline QPointF LtoA(QPointF ptF_Logical)const;
    //实际坐标转逻辑坐标
    inline QPoint AtoL(QPoint pt_Actual)const;
    inline QPointF AtoL(QPointF ptF_Actual)const;




    /*----------用于测试的变量---------*/
public:
    QImage* img;
    SImage* imgObj;

};

#endif // QCANVAS_H
