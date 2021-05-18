#include "sobject.h"
#include <QSvgRenderer>

SObject::SObject(PaintObject _type, bool _selected, QPointF _center,
                 const QString &_layerName,
                 const QString &_layerDiscription,
                 const QColor &_layerColor)
    :   type(_type), mbSelected(_selected), mPtCenter(_center),
        mStrLayerName(_layerName),
        mStrLayerDiscription(_layerDiscription),
        mLayerColor(_layerColor) {}

SObject::~SObject() {}

bool SObject::intersect(const QRectF &rect) const
{
    return boundingRect().intersects(rect);
}

void SObject::paintBoundRect(QPainter &painter, double scaleValue, int lineWidth, int radius)
{
    //保存原来的样式
    const QPen& oldPen = painter.pen();
    const QBrush& oldBrush = painter.brush();
    QTransform oldTransform = painter.transform();
    //定义外界矩形样式
    QPen pen;
    QBrush brush;
    pen.setColor(QColor(127, 127, 127));
    pen.setWidth(lineWidth / scaleValue);
    brush.setStyle(Qt::NoBrush);
    //设置为外界矩形样式
    painter.setPen(pen);
    painter.setBrush(brush);

    QPolygonF bound_polygon = this->boundingRect();

    //------------绘图-------------//

    //绘制矩形
    painter.drawPolygon(bound_polygon);
    //四个角点
    brush.setColor(QColor(255, 255, 255));
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);

    for(auto& pt : bound_polygon)
        painter.drawEllipse(pt, radius / scaleValue, radius / scaleValue);
    //四个边中点
    QPointF ptMid[4] {};
    for(int i = 0; i < 4; ++i)
    {
        ptMid[i] = (bound_polygon[i] + bound_polygon[(i + 1) % 4]) / 2.0;
        painter.drawEllipse(ptMid[i], radius / scaleValue, radius / scaleValue);
    }
    //绘制旋转符号
    painter.translate(ptMid[0]);
    painter.rotate(mdRotateAngle);
    painter.scale(1.0 / scaleValue, 1.0 / scaleValue);

    //保证旋转符号在包围矩形的外侧，且方向合理
    if(mdSy > 0)
        painter.translate(0, ROTATE_ICON_CENTER_Y);
    else
        painter.translate(0, -ROTATE_ICON_CENTER_Y);

    if(mdSx < 0)
        painter.scale(-1, 1);


    QSvgRenderer rotate_icon(QString(":/WidgetIcon/WidgetIcon/rotate.svg"));

    rotate_icon.render(&painter, ROTATE_ICON_RECT);

    //绘制一条链接线（上边中点到旋转符号）
    pen.setWidth(BOUND_RECT_PEN_WIDTH);
    painter.setPen(pen);
    if(mdSy > 0)
        painter.drawLine(0, ROTATE_ICON_RADIUS, 0, - ROTATE_ICON_CENTER_Y - radius);
    else
        painter.drawLine(0, -ROTATE_ICON_RADIUS, 0, + ROTATE_ICON_CENTER_Y + radius);
    //------------绘图-------------//

    //还原样式
    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
    painter.setTransform(oldTransform);
}


void SObject::translate(double dx, double dy)
{
    mPtCenter.rx() += dx;
    mPtCenter.ry() += dy;
}

void SObject::translate(const QPointF &pt)
{
    mPtCenter += pt;
}

void SObject::rotate(double angle, bool doReCalc)
{
    mdRotateAngle = fmod(mdRotateAngle + angle, 360.0);
    if(doReCalc) reCalcTransfrom();
}

void SObject::scale(double sx, double sy, bool doReCalc)
{
    Q_ASSERT(sx != 0 && sy != 0);
    mdSx *= sx;
    mdSy *= sy;
    if(doReCalc) reCalcTransfrom();
}

QIcon SObject::icon()const
{
    QPixmap iconPixmap(LAYER_ICON_SIZE);
    iconPixmap.fill(Qt::transparent);
    QPainter iconPainter(&iconPixmap);

    //确定形状的外接矩形尺寸
    QRectF bound_rect = this->boundingRect().boundingRect();
    //平移外界矩形中心点到画布中心
    iconPainter.translate(iconPixmap.rect().center());
    //使形状填充画布
    iconPainter.scale(iconPixmap.width() / bound_rect.width(), iconPixmap.height() / bound_rect.height());

    this->paint(iconPainter, false);

    return QIcon(iconPixmap);
}

bool SObject::isVisible() const
{
    return mbVisible;
}

bool SObject::isLocked() const
{
    return mbLocked;
}

bool SObject::isSelected() const
{
    return mbSelected;
}

double SObject::rotateAngle() const
{
    return mdRotateAngle;
}

double SObject::scaleFactorX() const
{
    return mdSx;
}

double SObject::scaleFactorY() const
{
    return mdSy;
}

const QPointF SObject::centerPoint() const
{
    return mPtCenter;
}

const QString &SObject::layerName() const
{
    return mStrLayerName;
}

const QString &SObject::layerDiscription() const
{
    return mStrLayerDiscription;
}

const QColor &SObject::layerColor() const
{
    return mLayerColor;
}

PaintObject SObject::getType()
{
    return type;
}

const QTransform &SObject::transform() const
{
    return mTransform;
}

void SObject::setVisible(bool visible)
{
    this->mbVisible = visible;
}

void SObject::setLocked(bool lock)
{
    this->mbLocked = lock;
}

void SObject::setSelected(bool select)
{
    mbSelected = select;
}

void SObject::setRotateAngle(double angle, bool doReCalc)
{
    mdRotateAngle = fmod(angle, 360.0);
    if(doReCalc) reCalcTransfrom();
}

void SObject::setScaleFactor(double sx, double sy, bool doReCalc)
{
    mdSx = sx;
    mdSy = sy;
    if(doReCalc) reCalcTransfrom();
}

void SObject::setCenterPoint(const QPointF &newCenterPt)
{
    mPtCenter = newCenterPt;
}

void SObject::setLayerName(const QString &name)
{
    mStrLayerName = name;
}

void SObject::setLayerDiscription(const QString &discription)
{
    mStrLayerDiscription = discription;
}

void SObject::setLayerColor(const QColor &color)
{
    mLayerColor = color;
}

void SObject::reCalcTransfrom()
{
    mTransform.reset();
    mTransform.rotate(mdRotateAngle);
    mTransform.scale(mdSx, mdSy);
    _applyTransform();
}

void SObject::_initializeWith(const SObject &theObj)
{
    mbVisible = theObj.mbVisible;
    mbLocked = theObj.mbLocked;
    mbSelected = theObj.mbSelected;
    mTransform = theObj.mTransform;
    mdRotateAngle = theObj.mdRotateAngle;
    mdSx = theObj.mdSx;
    mdSy = theObj.mdSy;
    mPtCenter = theObj.mPtCenter;
    mStrLayerName = theObj.mStrLayerName;
    mStrLayerDiscription = theObj.mStrLayerDiscription;
    mLayerColor = theObj.mLayerColor;
}
