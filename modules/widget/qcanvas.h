#ifndef QCANVAS_H
#define QCANVAS_H
#include <QWidget>
#include "modules/paint/simage.h"
#include <modules/global.h>
#include <modules/doc/sdocument.h>
#include "modules/paint/sobject.h"


class SDocument;
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
    //画布鼠标滚轮事件
    void wheelEvent(QWheelEvent* event)override;

    /*-----信号-----*/
signals:
    //鼠标移动
    void mouseMoved(QPointF pos);
    //缩放激活信号
    void scaling(QPointF pos, int delta);
    //缩放成功信号
    void scaled(double value);

    /*-----槽函数-----*/
public slots:
    void doRepaint();
    void doUpdate();

    //响应颜色选择变化
    void onDrawColorChanged(QColor color);
    /*-----控制标记-----*/
private:
    //鼠标点的位置
    QPointF mPtLgcLeftPressPos{};
    QPointF mPtLgcLastPos{};

    //左键是否按下
    bool mbLeftPressed{false};

    //鼠标是否在某些特殊位置
    bool mbCursorOnSelectedLayer{false};
    int mnCursorOnCornerCtrlPointIdx{-1};
    int mnCursorOnMiddleCtrlPointIdx{-1};
    bool mbCursorOnRotateIcon{false};

    //鼠标左键是否按在某些特殊位置而还没有释放
    bool mbLeftPressedOnSelectedLayer{false};
    int mnLeftPressedOnCornerCtrlPointIdx{-1};
    int mnLeftPressedOnMiddleCtrlPointIdx{-1};
    bool mbLeftPressedOnRotateIcon{false};

    //控制目标
    SObject* mpDstObj{nullptr};
    //控制目标在鼠标左键按下时的变换参数
    double mdOriginalRotateAngle{0};
    double mdOriginalScaleX{1}, mdOriginalScaleY{1};


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
    //当前绘制区域，由画布区域对象维护
    QRectF mViewArea{};

    /*-----成员变量-----*/
private:
    //文档
    SDocument* mpDoc{nullptr};
    //重绘诱因
    SObject::PaintTrigger mTrigger{SObject::User_Trigger};

    /*-----成员函数-----*/
    QColor mpDrawColor;
public:
    //[访问函数]

    //获取文档
    SDocument* getDocument();

    //画布逻辑尺寸
    QSize logicalSize()const;
    //画布实际尺寸
    QSize actualSize()const;
    //缩放值
    double scaleValue()const;
    //绘制区域
    const QRectF& viewArea()const;
    //网格是否开启
    bool isGridOn()const;
    //参考线是否开启
    bool isRefLineOn()const;

    SObject::PaintTrigger currentPaintTrigger()const;

    //[更改函数]

    void setDocument(SDocument* pDoc);

    void setPaintTrigger(SObject::PaintTrigger trigger);
    //重置绘制诱因为User
    void resetPaintTrigger();

    //按数值缩放画布实际尺寸
    bool setScaleValue(double value);
    //按级别缩放画布实际尺寸
    bool setScaleLevel(int level);
    //缩放级别增加
    bool setScaleLevelUp();
    //缩放级别减少
    bool setScaleLevelDown();

    //设置绘制区域
    void setViewArea(const QRectF& rect);

    void updateViewArea();

    //[功能函数]

    //逻辑坐标转实际坐标
    inline QPoint LtoA(const QPoint& pt_Logical)const;
    inline QPointF LtoA(const QPointF& ptF_Logical)const;

    inline QRect LtoA(const QRect& rect_Logical)const;
    inline QRectF LtoA(const QRectF& rectF_Logical)const;

    //实际坐标转逻辑坐标
    inline QPoint AtoL(const QPoint& pt_Actual)const;
    inline QPointF AtoL(const QPointF& ptF_Actual)const;

    inline QRect AtoL(const QRect& rect_Actual)const;
    inline QRectF AtoL(const QRectF& rectF_Actual)const;

};

QPoint QCanvas::LtoA(const QPoint& pt_Logical) const
{
    return QPoint(pt_Logical.x() * mdScale, pt_Logical.y() * mdScale);
}

QPointF QCanvas::LtoA(const QPointF& ptF_Logical) const
{
    return QPointF(ptF_Logical.x() * mdScale, ptF_Logical.y() * mdScale);
}

QRect QCanvas::LtoA(const QRect &rect_Logical) const
{
    return QRect(rect_Logical.left() * mdScale, rect_Logical.top() * mdScale, rect_Logical.width() * mdScale, rect_Logical.height() * mdScale);
}

QRectF QCanvas::LtoA(const QRectF &rectF_Logical) const
{
    return QRectF(rectF_Logical.left() * mdScale, rectF_Logical.top() * mdScale, rectF_Logical.width() * mdScale, rectF_Logical.height() * mdScale);
}

QPoint QCanvas::AtoL(const QPoint& pt_Actual) const
{
    return QPoint(pt_Actual.x() / mdScale, pt_Actual.y() / mdScale);
}

QPointF QCanvas::AtoL(const QPointF& ptF_Actual) const
{
    return QPointF(ptF_Actual.x() / mdScale, ptF_Actual.y() / mdScale);
}

QRect QCanvas::AtoL(const QRect &rect_Actual) const
{
    return QRect(rect_Actual.left() / mdScale, rect_Actual.top() / mdScale, rect_Actual.width() / mdScale, rect_Actual.height() / mdScale);
}

QRectF QCanvas::AtoL(const QRectF &rectF_Actual) const
{
    return QRectF(rectF_Actual.left() / mdScale, rectF_Actual.top() / mdScale, rectF_Actual.width() / mdScale, rectF_Actual.height() / mdScale);
}

#endif // QCANVAS_H
