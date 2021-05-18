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
    QPoint pos = event->pos();
    //------坐标显示-----//
    if(pos.x() < 0 || pos.x() >= mSzActual.width() || pos.y() < 0 || pos.y() >= mSzActual.height())
        return;
    //显示逻辑坐标
    QPoint lgcPos = AtoL(pos);
    emit mouseMoved(lgcPos);

    //------拖动选中图层------//
    if(mbDragging)
    {
        const std::list<list_iterator>& selectedIterList = mpDoc->getLayerManager().getSelectedLayerIterList();
        for(const list_iterator& iter : selectedIterList)
        {
            (*iter)->translate(lgcPos - mPtLastLogicalPos);
        }
        update();
    }
    this->mPtLastLogicalPos = lgcPos;
}

void QCanvas::mousePressEvent(QMouseEvent * event)
{
    //判断鼠标点是否在某个已经选中的对象内
    QPoint lgcPos = AtoL(event->pos());
    const std::list<list_iterator>& selectedIterList = mpDoc->getLayerManager().getSelectedLayerIterList();
    for(const list_iterator& iter : selectedIterList)
    {
        if((*iter)->contains(lgcPos))
        {
            mbDragging = true;
            break;
        }
    }
    //记录鼠标点位置
    this->mPtLogicalPressPos = lgcPos;
    this->mPtLastLogicalPos = lgcPos;
}
void QCanvas::mouseReleaseEvent(QMouseEvent * event)
{
    QPoint lgcPos = AtoL(event->pos());
    //判断是否有拖动发生
    int dragDistance = abs((this->LtoA(mPtLogicalPressPos) - event->pos()).manhattanLength());
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
    mbDragging = false;
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

void QCanvas::setViewArea(const QRectF &rect)
{
    mViewArea = rect;
}

void QCanvas::changeSelectLayerInList(QString order)
{
    //将画布移动四种选择写入QString链表
    QStringList options;
    options<<"BringForward"<<"SendBackward"<<"BringToFront"<<"SendToBack";
    //得到画布链表
    layer_list layerlist = mpDoc->getLayerManager().getLayerList();
    std::list<SObject*>::const_iterator iter;
    for(iter=layerlist.begin();iter!=layerlist.end();iter++){
        SObject* obj= *iter;
        if(obj->isSelected()==true)
        {
            break;
        }
    }

    std::list<SObject*>::const_iterator iter1;
    std::list<SObject*>::const_iterator iter2;

    //根据输入指令order对画布链表进行不同操作
    switch(options.indexOf(order))
    {
    case 0:
        if(iter==layerlist.end())
        {
            break;
        }
        iter1=iter;
        iter2=iter++;
        std::swap(iter1,iter2);
        break;
    case 1:
        if(iter==layerlist.begin())
        {
            break;
        }
        iter1=iter;
        iter2=iter--;
        std::swap(iter1,iter2);
        break;
    case 2:
        iter1=iter;
        iter2=layerlist.end();
        std::swap(iter1,iter2);
        break;
    case 3:
        iter1=iter;
        iter2=layerlist.begin();
        std::swap(iter1,iter2);
        break;
    }
}

void QCanvas::moveSelectObject(QString order)
{
    //将绘图对象六种操作选择写入QString链表
    QStringList options;
    options<<"AlignLeft"<<"AlignMiddle"<<"AlignRight"<<"AlignTop"<<"AlignCenter"<<"AlignBottom";
    layer_list layerlist = mpDoc->getLayerManager().getLayerList();
    std::list<SObject*>::iterator iter;
    int index=0;
    for(iter=layerlist.begin();iter!=layerlist.end();iter++,index++){
        SObject* obj= *iter;
        if(obj->isSelected()==true)
        {
            break;
        }
    }

    //得到外接矩形的坐标信息
    SObject* obj=*iter;
    QPointF centerPoint=obj->centerPoint();
    QPolygonF boundPolygon=obj->boundingRect();
    QPointF* topLeftPoint=boundPolygon.begin();
    qreal dx=centerPoint.rx()-topLeftPoint->rx();
    qreal dy=centerPoint.ry()-topLeftPoint->ry();

    //根据输入指令order的不同类型对绘图对象进行不同操作
    switch(options.indexOf(order))
    {
    case 0:
        (*iter)->translate(dx-centerPoint.rx(),0);
        break;

    case 1:
        (*iter)->translate(DEFAULT_CANVAS_SIZE.rwidth()/2-centerPoint.rx(),0);
        break;

    case 2:
        (*iter)->translate(DEFAULT_CANVAS_SIZE.rheight()-dx,0);
        break;

    case 3:
        (*iter)->translate(0,dy-centerPoint.ry());
        break;

    case 4:
        (*iter)->translate(DEFAULT_CANVAS_SIZE.rheight()-dx,DEFAULT_CANVAS_SIZE.rwidth()-dy);
        break;

    case 5:
        (*iter)->translate(0,DEFAULT_CANVAS_SIZE.rwidth()-dy);
        break;
    }

}
