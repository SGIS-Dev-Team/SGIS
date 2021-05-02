#include "qcanvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <sshape.h>
#include <simage.h>

QCanvas::QCanvas(QWidget *parent, QSize canvasSize): QWidget(parent)
{
    //属性设置
    setBackgroundRole(QPalette::Base);
    setFixedSize(canvasSize);
    setMouseTracking(true);

    this->mSzLogical = this->mSzActual = canvasSize;
    mdScale = 1.0;
}

QCanvas::~QCanvas()
{

}

void QCanvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    /*-----画布背景绘图部分-----*/
    //若画布缩放比例大于800%，则显示像素网格
    if(mdScale >= 8.0)
    {
        QPen gridPen = painter.pen();
        gridPen.setColor(QColor(0, 0, 0));
        gridPen.setWidth(1);
        painter.setPen(gridPen);
        //绘制纵向网格线
        QPointF* pPtFPairsX = new QPointF[mSzLogical.width() * 2] {};
        double h_Act = mSzActual.height();
        for(int x_Log = 1; x_Log < mSzLogical.width(); ++x_Log)
        {
            double x_Act = x_Log * mdScale;
            pPtFPairsX[x_Log * 2] = QPointF(x_Act, 0);
            pPtFPairsX[x_Log * 2 + 1] = QPointF(x_Act, h_Act - 1);
        }
        painter.drawLines(pPtFPairsX, mSzLogical.width());
        delete[] pPtFPairsX;
        //绘制横向网格线
        QPointF* pPtFPairsY = new QPointF[mSzLogical.height() * 2];
        double w_Act = mSzActual.width();
        for(int y_Log = 1; y_Log < mSzLogical.height(); ++y_Log)
        {
            double y_Act = y_Log * mdScale;
            pPtFPairsY[y_Log * 2] = QPointF(0, y_Act);
            pPtFPairsY[y_Log * 2 + 1] = QPointF(w_Act - 1, y_Act);
        }
        painter.drawLines(pPtFPairsY, mSzLogical.width());
        delete[] pPtFPairsY;
    }

    /*-----画布内容绘制-----*/
    //缩放绘制器
    painter.scale(mdScale, mdScale);

    SShape rect(PaintObject::ShapeBase, true, QPoint(5000, 5000));
    rect.addVertex({QPointF(4900, 4900), QPointF(5100, 4900), QPointF(5100, 5100), QPointF(4900, 5100)});

    rect.paint(&painter);
}

void QCanvas::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    //------坐标显示-----//
    if(pos.x() < 0 || pos.x() >= mSzActual.width() || pos.y() < 0 || pos.y() >= mSzActual.height())
        return;
    //显示逻辑坐标
    //emit mouseMoved(AtoL(pos));
    //显示实际坐标
    emit mouseMoved(pos);
}

void QCanvas::mousePressEvent(QMouseEvent *event)
{
}

void QCanvas::mouseReleaseEvent(QMouseEvent *event)
{
}

void QCanvas::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() == Qt::KeyboardModifier::ControlModifier)
    {
        emit scaling(AtoL(event->pos()), event->delta());
        return;
    }
}

QSize QCanvas::logicalSize() const
{
    return mSzLogical;
}

QSize QCanvas::actualSize() const
{
    return mSzActual;
}

double QCanvas::scaleValue() const
{
    return mdScale;
}

bool QCanvas::isGridOn() const
{
    return mbGridOn;
}

bool QCanvas::isRefLineOn() const
{
    return mbRefLineOn;
}

bool QCanvas::setScaleValue(double value)
{
    bool bScaled{false};
    mdScale = value < ScaleLevelValue[0] ?  bScaled = false, ScaleLevelValue[0] :
              value > ScaleLevelValue[SCALE_LEVEL - 1] ? bScaled = false, ScaleLevelValue[SCALE_LEVEL - 1] : value, bScaled = true;
    mSzActual = QSize(mSzLogical.width() * value, mSzLogical.height() * value);
    this->setFixedSize(mSzActual);
    if(bScaled)
        emit scaled(mdScale);
    return bScaled;
}

bool QCanvas::setScaleLevel(int level)
{
    if(level < 0 || level > SCALE_LEVEL - 1)
        return false;
    return setScaleValue(ScaleLevelValue[level]);
}

bool QCanvas::setScaleLevelUp()
{
    //判断当前缩放等级
    int level = -1;
    while(level != SCALE_LEVEL - 1)
        if(mdScale < ScaleLevelValue[++level])
            break;
    return setScaleLevel(level);
}

bool QCanvas::setScaleLevelDown()
{
    //判断当前缩放等级
    int level = SCALE_LEVEL;
    while(level != 0)
        if(mdScale > ScaleLevelValue[--level])
            break;
    return setScaleLevel(level);
}
