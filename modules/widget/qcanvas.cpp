﻿#include "qcanvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <modules/paint/sshape.h>
#include <modules/paint/simage.h>

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

    /*-----画布内容绘制-----*/
    //缩放绘制画笔
    painter.scale(mdScale, mdScale);

    if(mpDoc)
        mpDoc->paint(painter, mViewArea, mdScale);

    /*-----画布背景绘图部分-----*/
    painter.resetTransform();

#ifdef CANVAS_DRAW_CROSS_LINE
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
#endif
}

void QCanvas::mouseMoveEvent(QMouseEvent *event)
{
    QPointF actPos = event->pos();
    //------坐标显示------//
    if(actPos.x() < 0 || actPos.x() >= mSzActual.width() || actPos.y() < 0 || actPos.y() >= mSzActual.height())
        return;
    //显示逻辑坐标
    QPointF lgcPos = AtoL(actPos);
    emit mouseMoved(lgcPos);

    //------鼠标悬浮图标显示------//
    SObject* pObj = mpDoc->getLayerManager().getTopLayerOn(this->AtoL(event->pos()), true, true);
    Qt::CursorShape cursor_shape{};

    if(mbCursorOnLayer != static_cast<bool>(pObj))
    {
        mbCursorOnLayer = static_cast<bool>(pObj);
        cursor_shape = mbCursorOnLayer ? Qt::DragMoveCursor : Qt::CursorShape::ArrowCursor;
    }

    if(pObj)
    {
        //判断鼠标点在哪个点圆内
        QPointF nearest_pt;
        bool isFound{false};

        QPolygonF bound_rect = pObj->boundingRect();

        for(auto& corner : bound_rect)
        {
            if((this->LtoA(corner) - actPos).manhattanLength() <= BOUND_RECT_CORNER_RADIUS * 3)
            {
                nearest_pt = corner;
                isFound = true;
                break;
            }
        }

        //计算中点并判断是否在圆内
        if(!isFound)
            for(int i = 0; i < 4; ++i)
            {
                nearest_pt = (bound_rect[i] + bound_rect[(i + 1) % 4]) / 2;
                if((this->LtoA(nearest_pt) - actPos).manhattanLength() <= BOUND_RECT_CORNER_RADIUS * 2)
                {
                    isFound = true;
                    break;
                }
            }

        //选择最合适的鼠标图标
        if(isFound)
        {
            double x = nearest_pt.x() - pObj->centerPoint().x();
            double y = nearest_pt.y() - pObj->centerPoint().y();

            if(x == 0)
                cursor_shape = Qt::SizeVerCursor;
            else
            {
                const double tan_22_5 = 0.414213562373095;
                const double tan_67_5 = 2.414213562373095;
                double tan_sita = abs(y / x);
                if(tan_sita < tan_22_5)
                    cursor_shape = Qt::SizeHorCursor;
                else if(tan_sita < tan_67_5)
                    cursor_shape = x * y < 0 ? Qt::SizeBDiagCursor : Qt::SizeFDiagCursor;
                else
                    cursor_shape = Qt::SizeVerCursor;
            }
        }
    }

    this->setCursor(QCursor(cursor_shape));

    //------拖动选中图层------//
    if(mbPressed)
    {
        const std::list<list_iterator>& selectedIterList = mpDoc->getLayerManager().getSelectedLayerIterList();
        for(const list_iterator& iter : selectedIterList)
        {
            (*iter)->translate(lgcPos - mPtLastLogicalPos);
        }
        update(this->LtoA(viewArea()).toRect());
    }
    this->mPtLastLogicalPos = lgcPos;
}

void QCanvas::mousePressEvent(QMouseEvent * event)
{
    //判断鼠标点是否在某个已经选中的对象内
    QPointF lgcPos = AtoL(QPointF(event->pos()));
    SObject* pObj = mpDoc->getLayerManager().getTopLayerOn(lgcPos, true);
    if(pObj)
        mbPressed = true;
    //记录鼠标点位置
    this->mPtLogicalPressPos = lgcPos;
    this->mPtLastLogicalPos = lgcPos;
}
void QCanvas::mouseReleaseEvent(QMouseEvent * event)
{
    QPointF lgcPos = AtoL(QPointF(event->pos()));

    //判断是否有拖动发生
    int dragDistance = (this->LtoA(mPtLogicalPressPos) - event->pos()).manhattanLength();
    qDebug() << S3DBG(dragDistance, this->LtoA(mPtLogicalPressPos), event->pos());
    if(dragDistance < DRAG_TRIGGERING_DISTANCE)
        if(mpDoc)
        {
            //选中顶层对象
            SLayerManager& mgr = mpDoc->getLayerManager();

            bool doMultiSelect{false};
            //多重选择
            if(event->modifiers() == Qt::KeyboardModifier::ControlModifier)
                doMultiSelect = true;

            mgr.clickSelect(lgcPos, doMultiSelect);

            update(this->LtoA(mViewArea.toRect()));
        }
    mbPressed = false;
}

void QCanvas::wheelEvent(QWheelEvent * event)
{
    if(event->modifiers() == Qt::KeyboardModifier::ControlModifier)
    {
        emit scaling(AtoL(event->position()), event->angleDelta().y());
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

const QRectF &QCanvas::viewArea() const
{
    return mViewArea;
}

bool QCanvas::isGridOn() const
{
    return mbGridOn;
}

bool QCanvas::isRefLineOn() const
{
    return mbRefLineOn;
}

void QCanvas::setDocument(SDocument * pDoc)
{
    this->mpDoc = pDoc;
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

void QCanvas::setViewArea(const QRectF & rect)
{
    mViewArea = rect;
}