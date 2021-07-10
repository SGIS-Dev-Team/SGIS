#include "qcanvasarea.h"
#include <QWheelEvent>
#include <QScrollBar>

QCanvasArea::QCanvasArea(const QSize &CanvasSize, QWidget *parent): QScrollArea(parent)
{
    mCanvasSize = CanvasSize;
    _initialize();
}

QCanvasArea::QCanvasArea(QWidget *parent): QCanvasArea(DEFAULT_CANVAS_SIZE, parent) {}

QCanvasArea::~QCanvasArea()
{
}

QCanvas * QCanvasArea::canvas()const
{
    return mpCanvas;
}

QRectF QCanvasArea::viewArea() const
{
    double scaleValue = mpCanvas->scaleValue();
    return QRectF(this->horizontalScrollBar()->value() / scaleValue,
                  this->verticalScrollBar()->value() / scaleValue,
                  this->horizontalScrollBar()->pageStep() / scaleValue,
                  this->verticalScrollBar()->pageStep() / scaleValue);
}

void QCanvasArea::_initialize()
{
    //创建画布
    mpCanvas = new QCanvas(this, mCanvasSize);
    //链接事件响应
    connect(this, &QCanvasArea::colorChanged, mpCanvas, &QCanvas::onDrawColorChanged);    //链接颜色改变信号
    connect(mpCanvas, &QCanvas::scaling, this, &QCanvasArea::onCanvasScaling);
    connect(this->horizontalScrollBar(), &QScrollBar::valueChanged, this, &QCanvasArea::onSliderValueChange);
    connect(this->verticalScrollBar(), &QScrollBar::valueChanged, this, &QCanvasArea::onSliderValueChange);
    //调整对齐
    setAlignment(Qt::Alignment(Qt::AlignHCenter | Qt::AlignVCenter));
    this->setWidget(mpCanvas);
    mpCanvas->setViewArea(viewArea());
}

void QCanvasArea::wheelEvent(QWheelEvent *event)
{
    //Ctrl+滚轮   缩放画布
    //滚轮        滚动条上下移动
    //Shift+滚轮  滚动条左右移动
    int delta = event->angleDelta().y();

    if(event->modifiers() == Qt::KeyboardModifier::ControlModifier)
    {
        return;
        bool isScaled{false};
        if(delta > 0)
            isScaled = mpCanvas->setScaleValue(mpCanvas->scaleValue() + mpCanvas->scaleValue() / 10.0);
        else
            isScaled = mpCanvas->setScaleValue(mpCanvas->scaleValue() - mpCanvas->scaleValue() / 10.0);
    }

    else if(event->modifiers() == Qt::KeyboardModifier::ShiftModifier)
        this->horizontalScrollBar()->setValue(this->horizontalScrollBar()->value() - delta);
    else if(event->modifiers() == Qt::KeyboardModifier::NoModifier)
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - delta);

}


void QCanvasArea::onCanvasScaling(QPointF lgcPos, int delta)
{
    QScrollBar *hor = this->horizontalScrollBar(), *ver = this->verticalScrollBar();
    //确定视图区左上角点在画布上的实际坐标
    QPointF ptViewportLTActual(hor->value(), ver->value());
    //计算鼠标点实际坐标
    QPointF ptCursorActual = mpCanvas->LtoA(lgcPos);
    //保存原先缩放值
    double dScale0 = mpCanvas->scaleValue();

    //进行缩放操作
    bool isScaled{false};

    if(delta > 0)
        isScaled = mpCanvas->setScaleValue(dScale0 * 1.10);
    else
        isScaled = mpCanvas->setScaleValue(dScale0 * 0.90);

    if(!isScaled)
        return;

    //获取当前缩放值
    double dScale = mpCanvas->scaleValue();
    double dRatio = dScale / dScale0;

    //计算缩放后鼠标点在画布实际坐标系下的位置
    QPointF ptCursorActual_Resized(ptCursorActual.x() * dRatio, ptCursorActual.y() * dRatio);

    //计算视图区偏移
    QPointF viewportShift = ptCursorActual_Resized - ptCursorActual;

    //视图区缩放后左上角点
    QPointF ptViewportLTActual_Resized = viewportShift + ptViewportLTActual;

    //将视图区移动到新左上角点
    hor->setValue(ptViewportLTActual_Resized.x());
    ver->setValue(ptViewportLTActual_Resized.y());

    //发送视图区域改变信号
    mpCanvas->setViewArea(viewArea());
}

void QCanvasArea::onSliderValueChange(int value)
{
    mpCanvas->setViewArea(viewArea());
}
