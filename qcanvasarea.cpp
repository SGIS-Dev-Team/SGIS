#include "qcanvasarea.h"
#include <QWheelEvent>
#include <QScrollBar>

QCanvasArea::QCanvasArea(const QSize &CanvasSize, QWidget *parent): QScrollArea(parent)
{
    mpCanvas = new QCanvas(this, CanvasSize);

    //设置滚动条范围为画布逻辑尺寸像素范围
    this->horizontalScrollBar()->setMinimum(0);
    this->horizontalScrollBar()->setMaximum(mpCanvas->width() - 1);
    this->verticalScrollBar()->setMinimum(0);
    this->verticalScrollBar()->setMaximum(mpCanvas->height() - 1);

    //调整对齐
    setAlignment(Qt::Alignment(Qt::AlignHCenter | Qt::AlignVCenter));
    this->setWidget(mpCanvas);
}

QCanvasArea::QCanvasArea(const QString &templateFilePath, QWidget *parent)
{

}

QCanvasArea::QCanvasArea(QWidget *parent): QCanvasArea(DEFAULT_CANVAS_SIZE, parent) {}

QCanvasArea::~QCanvasArea()
{
    delete mpCanvas;
}

QCanvas *QCanvasArea::canvas()
{
    return mpCanvas;
}

void QCanvasArea::setCanvas(QCanvas * newCanvas)
{
    mpCanvas = newCanvas;
}

void QCanvasArea::keyPressEvent(QKeyEvent *event)
{

}

void QCanvasArea::wheelEvent(QWheelEvent *event)
{
    //Ctrl+滚轮   缩放画布
    //滚轮        滚动条上下移动
    //Shift+滚轮  滚动条左右移动

    if(event->modifiers() == Qt::KeyboardModifier::ControlModifier && event->delta() != 0)
    {
        //------为鼠标所在点预计算所在窗口位置------//
        //计算窗口左上角点在画布中的逻辑坐标
        QPointF ptLeftTop(this->horizontalScrollBar()->value(), this->verticalScrollBar()->value());

        bool isScaled{false};
        if(event->delta() > 0)
            isScaled = mpCanvas->setScaleValue(mpCanvas->scaleValue() + mpCanvas->scaleValue() / 10.0);
        else
            isScaled = mpCanvas->setScaleValue(mpCanvas->scaleValue() - mpCanvas->scaleValue() / 10.0);

        if(isScaled)
        {

        }
    }
    else if(event->modifiers() == Qt::KeyboardModifier::ShiftModifier)
        this->horizontalScrollBar()->setValue(this->horizontalScrollBar()->value() - event->delta());
    else if(event->modifiers() == Qt::KeyboardModifier::NoModifier)
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - event->delta());

}
