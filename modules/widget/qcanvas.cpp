#include "qcanvas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <modules/paint/simage.h>
#include <modules/paint/slinestringfeature.h>
#include <modules/paint/spointfeature.h>
#include <modules/paint/spolygonfeature.h>
QCanvas::QCanvas(QWidget* parent, QSize canvasSize): QWidget(parent)
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

void QCanvas::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    /*-----画布内容绘制-----*/
    //缩放绘制画笔

    painter.scale(mdScale, mdScale);

    if (mpDoc)
        mpDoc->paint(painter, mViewArea, mdScale, mTrigger);
    this->resetPaintTrigger();


    //-----测试要素绘图开始-----//

    //地理坐标系设置为画布坐标系等同
    std::shared_ptr<SCoordinate> pGeoRef = std::make_shared<SCoordinate>(0, 0, 1, 1);

    //构建要素
    std::shared_ptr<SLineStringFeature> pLineString(new SLineStringFeature(true));
    pLineString->setGeoReference(pGeoRef);

    //添加顶点
    pLineString->beginModifyVertex();

    //第一条线
    pLineString->getGeoPoints().push_back(std::vector<QPointF>({{500, 500}, {1000, 800}, {1500, 900}}));

    //第二条线
    pLineString->getGeoPoints().push_back(std::vector<QPointF>({{3000, 500}, {3000, 900}, {2000, 900}}));

    //结束添加
    pLineString->endModifyVertex();

    //设置要素样式
    pLineString->rPen().setWidth(20);
    pLineString->rPen().setStyle(Qt::PenStyle::DashLine);
    pLineString->rPen().setCapStyle(Qt::PenCapStyle::RoundCap);
    pLineString->rPen().setColor(mpDrawColor);
    pLineString->rPen().setJoinStyle(Qt::PenJoinStyle::RoundJoin);

    //绘图
    pLineString->paint(painter, true, mViewArea, mdScale, mTrigger);


    std::shared_ptr<SPointFeature>  pPointFeature(new SPointFeature(true));
    pPointFeature->setGeoReference(pGeoRef);
    pPointFeature->addPoint(QPointF(qrand() %200, 200));
    pPointFeature->addPoint(QPointF(qrand() %220, 400));
    pPointFeature->addPoint(QPointF(qrand() %230, 300));
    //设置要素样式
    pPointFeature->rPen().setWidth(20);
    pPointFeature->rPen().setStyle(Qt::PenStyle::DashLine);
    pPointFeature->rPen().setCapStyle(Qt::PenCapStyle::RoundCap);
    pPointFeature->rPen().setColor(mpDrawColor);
    pPointFeature->rPen().setJoinStyle(Qt::PenJoinStyle::RoundJoin);
    pPointFeature->paint(painter, true, mViewArea, mdScale, mTrigger);

    //
    auto  pPolygonFeature = std::make_shared<SPolygonFeature>(true);
    pPolygonFeature->setGeoReference(pGeoRef);
    pPolygonFeature->addPoint(QPointF(qrand() %200, 200));
    pPolygonFeature->addPoint(QPointF(qrand() %300, qrand() % 400));
    pPolygonFeature->addPoint(QPointF(500, 150));
    //设置要素样式
    pPolygonFeature->rPen().setWidth(20);
    pPolygonFeature->rPen().setStyle(Qt::PenStyle::DashLine);
    pPolygonFeature->rPen().setCapStyle(Qt::PenCapStyle::RoundCap);
    pPolygonFeature->rPen().setColor(mpDrawColor);
    pPolygonFeature->rPen().setJoinStyle(Qt::PenJoinStyle::RoundJoin);
    pPolygonFeature->paint(painter, true, mViewArea, mdScale, mTrigger);
    //-----测试要素绘图结束-----//


    /*-----画布背景绘图部分-----*/
    painter.resetTransform();

#ifdef CANVAS_DRAW_CROSS_LINE
    //若画布缩放比例大于800%，则显示像素网格
    if (mdScale >= 8.0)
    {
        QPen gridPen = painter.pen();
        gridPen.setColor(QColor(0, 0, 0));
        gridPen.setWidth(1);
        painter.setPen(gridPen);
        //绘制纵向网格线
        QPointF* pPtFPairsX = new QPointF[mSzLogical.width() * 2] {};
        double h_Act = mSzActual.height();
        for (int x_Log = 1; x_Log < mSzLogical.width(); ++x_Log)
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
        for (int y_Log = 1; y_Log < mSzLogical.height(); ++y_Log)
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

void QCanvas::mouseMoveEvent(QMouseEvent* event)
{
    QPointF actPos = event->pos();
    //------坐标显示------//
    if (actPos.x() < 0 || actPos.x() >= mSzActual.width() || actPos.y() < 0 || actPos.y() >= mSzActual.height())
        return;
    //显示逻辑坐标
    QPointF lgcPos = AtoL(actPos);
    emit mouseMoved(lgcPos);

    mnCursorOnCornerCtrlPointIdx = mnCursorOnMiddleCtrlPointIdx = -1;
    mbCursorOnRotateIcon = mbCursorOnSelectedLayer = false;

    if (!mbLeftPressed)
    {
        //------鼠标位置判断与鼠标悬浮图标显示------//
        Qt::CursorShape cursor_shape{};

        //对每个已选中图层的外接矩形进行判断
        for (const auto& iter : mpDoc->getLayerManager().getSelectedLayerIterList())
        {
            SObject* pObj = mpDstObj = *iter;
            bool isCursorOnCtrlPt{false};

            //判断鼠标点在外接矩形角点中哪个点圆内
            QPointF nearest_pt;
            QPolygonF bound_rect = pObj->boundingRect();

            for (int i = 0; i < bound_rect.size(); ++i)
                if ((this->LtoA(bound_rect[i]) - actPos).manhattanLength() <= BOUND_RECT_CORNER_RADIUS * 2)
                {
                    nearest_pt = bound_rect[i];
                    isCursorOnCtrlPt = true;
                    mnCursorOnCornerCtrlPointIdx = i;
                    break;
                }

            //计算外接矩形边中点并判断是否在点圆内
            QPointF ptMid[4];
            if (!isCursorOnCtrlPt)
                for (int i = 0; i < 4; ++i)
                {
                    ptMid[i] = (bound_rect[i] + bound_rect[(i + 1) % 4]) / 2.0;
                    if ((this->LtoA(ptMid[i]) - actPos).manhattanLength() <= BOUND_RECT_CORNER_RADIUS * 2)
                    {
                        nearest_pt = ptMid[i];
                        isCursorOnCtrlPt = true;
                        mnCursorOnMiddleCtrlPointIdx = i;
                        break;
                    }
                }

            //判断鼠标点是否在旋转图标上
            if (!isCursorOnCtrlPt)
            {
                //设置变换:我打赌Qt写这个类的人用了Lazy Evaluation
                QTransform transform;
                QPointF ptMid0_act = this->LtoA(ptMid[0]);
                transform.translate(ptMid0_act.x(), ptMid0_act.y());
                transform.rotate(pObj->rotateAngle());
                if (pObj->scaleFactorY() > 0)
                    transform.translate(0, ROTATE_ICON_CENTER_Y);
                else
                    transform.translate(0, -ROTATE_ICON_CENTER_Y);

                if (transform.inverted().map(actPos).manhattanLength() < ROTATE_ICON_RADIUS * 2)
                {
                    mbCursorOnRotateIcon = true;
                    cursor_shape = Qt::OpenHandCursor;
                    break;
                }
            }

            //选择最合适的鼠标图标
            if (isCursorOnCtrlPt)
            {
                double x = nearest_pt.x() - pObj->centerPoint().x();
                double y = nearest_pt.y() - pObj->centerPoint().y();

                if (x == 0)
                    cursor_shape = Qt::SizeVerCursor;
                else
                {
                    const double tan_22_5 = 0.414213562373095;
                    const double tan_67_5 = 2.414213562373095;
                    double tan_sita = abs(y / x);
                    if (tan_sita < tan_22_5)
                        cursor_shape = Qt::SizeHorCursor;
                    else if (tan_sita < tan_67_5)
                        cursor_shape = x * y < 0 ? Qt::SizeBDiagCursor : Qt::SizeFDiagCursor;
                    else
                        cursor_shape = Qt::SizeVerCursor;
                }
                break;
            }

            //最后再判断鼠标点是否在已选中的图层上
            mbCursorOnSelectedLayer = pObj->contains(lgcPos);
            if (mbCursorOnSelectedLayer)
                cursor_shape = Qt::DragMoveCursor;
        }

        this->setCursor(QCursor(cursor_shape));
        if (!(mnCursorOnCornerCtrlPointIdx != -1 || mnCursorOnMiddleCtrlPointIdx != -1 || mbCursorOnSelectedLayer || mbCursorOnRotateIcon))
            mpDstObj = nullptr;
    }
    else
    {
        if (mpDstObj)
        {
            //鼠标左键按下时事件
            QPointF ptTranslated(0, 0);
            double angleRotated{0};
            double xScaled{1}, yScaled{1};

            if (mbLeftPressedOnSelectedLayer)
            {
                //拖动选中图层
                ptTranslated = lgcPos - mPtLgcLastPos;
            }
            else if (mnLeftPressedOnCornerCtrlPointIdx != -1 || mnLeftPressedOnMiddleCtrlPointIdx != -1)
            {
                switch (event->modifiers())
                {
                //中心缩放
                case Qt::KeyboardModifier::ControlModifier:
                {
                    const QPointF& centerPt = this->LtoA(mpDstObj->centerPoint());
                    //变换到mpDstObj的旋转后坐标系再计算缩放
                    QTransform rotater;
                    rotater.translate(centerPt.x(), centerPt.y());
                    rotater.rotate(mdOriginalRotateAngle);
                    rotater = rotater.inverted();
                    //执行变换
                    QPointF curPt = rotater.map(actPos);
                    QPointF startPt = rotater.map(mPtLgcLeftPressPos * mdScale);
                    //计算旋转后坐标系的缩放量
                    xScaled = curPt.x() / startPt.x();
                    yScaled = curPt.y() / startPt.y();
                    //对控制点为中点的情况单独考虑
                    if (mnLeftPressedOnMiddleCtrlPointIdx == 0 || mnLeftPressedOnMiddleCtrlPointIdx == 2)
                        xScaled = 1;
                    if (mnLeftPressedOnMiddleCtrlPointIdx == 1 || mnLeftPressedOnMiddleCtrlPointIdx == 3)
                        yScaled = 1;

                    break;
                }
                //矩形缩放，还需要几个变量确定鼠标按在哪个控制点上面了
                case Qt::KeyboardModifier::NoModifier:
                {
                    break;
                }
                default:
                    break;
                }
            }
            else if (mbLeftPressedOnRotateIcon)
            {
                //计算偏角
                const QPointF& centerPt = this->LtoA(mpDstObj->centerPoint());
                double x1 = mPtLgcLastPos.x() * mdScale - centerPt.x(), x2 = actPos.x() - centerPt.x();
                double y1 = mPtLgcLastPos.y() * mdScale - centerPt.y(), y2 = actPos.y() - centerPt.y();
                angleRotated = acos((x1 * x2 + y1 * y2) / sqrt((x1 * x1 + y1 * y1) * (x2 * x2 + y2 * y2))) / PI * 180;
                if (x1 * y2 - x2 * y1 < 0)
                    angleRotated = -angleRotated;
            }

            //应用变换
            const std::list<list_iterator>& selectedIterList = mpDoc->getLayerManager().getSelectedLayerIterList();
            for (const list_iterator& iter : selectedIterList)
            {
                SObject* pObj = *iter;
                pObj->translate(ptTranslated);
                pObj->rotate(angleRotated, false);
                pObj->setScaleFactor(mdOriginalScaleX * xScaled, mdOriginalScaleY * yScaled, false);
                pObj->reCalcTransfrom();
            }

            //更新视图区
            update(this->LtoA(viewArea()).toRect());
        }
    }
    this->mPtLgcLastPos = lgcPos;
}

void QCanvas::mousePressEvent(QMouseEvent* event)
{
    QPointF lgcPos = AtoL(QPointF(event->pos()));

    switch (event->button())
    {
    //判断鼠标点是否在某个已经选中的对象内
    case Qt::LeftButton:
    {
        mbLeftPressed = true;
        //根据按下的位置改变图标
        Qt::CursorShape cursor_shape{};

        if (mbCursorOnSelectedLayer)
        {
            cursor_shape = Qt::SizeAllCursor;
            mbLeftPressedOnSelectedLayer = true;
        }
        else if (mnCursorOnCornerCtrlPointIdx != -1)
        {
            cursor_shape = Qt::CrossCursor;
            mnLeftPressedOnCornerCtrlPointIdx = mnCursorOnCornerCtrlPointIdx;
        }
        else if (mnCursorOnMiddleCtrlPointIdx != -1)
        {
            cursor_shape = Qt::CrossCursor;
            mnLeftPressedOnMiddleCtrlPointIdx = mnCursorOnMiddleCtrlPointIdx;
        }
        else if (mbCursorOnRotateIcon)
        {
            cursor_shape = Qt::ClosedHandCursor;
            mbLeftPressedOnRotateIcon = true;
        }

        //记录变换前参数
        if (mpDstObj)
        {
            mdOriginalScaleX = mpDstObj->scaleFactorX();
            mdOriginalScaleY = mpDstObj->scaleFactorY();
            mdOriginalRotateAngle = mpDstObj->rotateAngle();
        }
        //改变鼠标图标
        this->setCursor(QCursor(cursor_shape));

        break;
    }
    case Qt::RightButton:
    {

        break;
    }
    default:
        break;
    }

    //记录鼠标点位置
    this->mPtLgcLeftPressPos = lgcPos;
}
void QCanvas::mouseReleaseEvent(QMouseEvent* event)
{
    switch (event->button())
    {
    case Qt::LeftButton:
    {
        //判断是否有拖动发生
        QPointF lgcPos = AtoL(QPointF(event->pos()));
        int dragDistance = (this->LtoA(mPtLgcLeftPressPos) - event->pos()).manhattanLength();

        if (dragDistance < DRAG_TRIGGERING_DISTANCE)
            if (mpDoc)
            {
                //选中顶层对象
                SLayerManager& mgr = mpDoc->getLayerManager();

                bool doMultiSelect{false};
                //多重选择
                if (event->modifiers() == Qt::KeyboardModifier::ControlModifier)
                    doMultiSelect = true;

                mgr.clickSelect(lgcPos, doMultiSelect);

                updateViewArea();
            }

        //清空控制标记
        mbLeftPressed = mbLeftPressedOnRotateIcon = mbLeftPressedOnSelectedLayer = false;
        mnLeftPressedOnCornerCtrlPointIdx = mnLeftPressedOnMiddleCtrlPointIdx = -1;
        mdOriginalRotateAngle = 0, mdOriginalScaleX = mdOriginalScaleY = 1;
        break;
    }
    case Qt::RightButton:

        break;
    default:
        break;
    }

    this->setCursor(QCursor());
}

void QCanvas::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() == Qt::KeyboardModifier::ControlModifier)
    {
        emit scaling(AtoL(event->position()), event->angleDelta().y());
        return;
    }
}

void QCanvas::doRepaint()
{
    repaint(mViewArea.toRect());
}

void QCanvas::doUpdate()
{
    updateViewArea();
}

void QCanvas::onDrawColorChanged(QColor color)
{
    //设置当前选中颜色
    mpDrawColor  = color;
    //刷新
    update();
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

const QRectF& QCanvas::viewArea() const
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

SObject::PaintTrigger QCanvas::currentPaintTrigger() const
{
    return mTrigger;
}

void QCanvas::setDocument(SDocument* pDoc)
{
    this->mpDoc = pDoc;
}

void QCanvas::setPaintTrigger(SObject::PaintTrigger trigger)
{
    mTrigger = trigger;
}

void QCanvas::resetPaintTrigger()
{
    mTrigger = SObject::User_Trigger;
}

bool QCanvas::setScaleValue(double value)
{
    bool bScaled{false};
    mdScale = value < ScaleLevelValue[0] ?  bScaled = false, ScaleLevelValue[0] :
                                                      value > ScaleLevelValue[SCALE_LEVEL - 1] ? bScaled = false, ScaleLevelValue[SCALE_LEVEL - 1] : value, bScaled = true;
    mSzActual = QSize(mSzLogical.width() * value, mSzLogical.height() * value);
    this->setFixedSize(mSzActual);
    if (bScaled)
        emit scaled(mdScale);
    return bScaled;
}

bool QCanvas::setScaleLevel(int level)
{
    if (level < 0 || level > SCALE_LEVEL - 1)
        return false;
    return setScaleValue(ScaleLevelValue[level]);
}

bool QCanvas::setScaleLevelUp()
{
    //判断当前缩放等级
    int level = -1;
    while (level != SCALE_LEVEL - 1)
        if (mdScale < ScaleLevelValue[++level])
            break;
    return setScaleLevel(level);
}

bool QCanvas::setScaleLevelDown()
{
    //判断当前缩放等级
    int level = SCALE_LEVEL;
    while (level != 0)
        if (mdScale > ScaleLevelValue[--level])
            break;
    return setScaleLevel(level);
}

void QCanvas::setViewArea(const QRectF& rect)
{
    mViewArea = rect;
}

void QCanvas::updateViewArea()
{
    update(this->LtoA(mViewArea.toRect()));
}

