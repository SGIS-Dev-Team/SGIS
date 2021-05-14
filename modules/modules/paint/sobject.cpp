#include "sobject.h"


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
    for(int i = 0; i < 4; ++i)
    {
        QPointF center = (bound_polygon[i] + bound_polygon[(i + 1) % 4]) / 2.0;
        painter.drawEllipse(center, radius / scaleValue, radius / scaleValue);
    }

    //------------绘图-------------//

    //还原样式
    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
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

void SObject::rotate(double angle)
{
    mdRotateAngle += angle;
    _reCalcTransfrom();
    _applyTransform();
}

void SObject::scale(double sx, double sy)
{
    Q_ASSERT(sx > 0 && sy > 0);
    mdSx *= sx;
    mdSy *= sy;
    _reCalcTransfrom();
    _applyTransform();
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

bool SObject::rotateAngle() const
{
    return mdRotateAngle;
}

void SObject::scaleFactor(double &sx, double &sy) const
{
    sx = this->mdSx;
    sy = this->mdSy;
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

void SObject::setRotateAngle(double angle)
{
    this->rotate(angle - mdRotateAngle);
}

void SObject::setScaleFactor(double sx, double sy)
{
    this->scale(sx / mdSx, sy / mdSy);
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

void SObject::_reCalcTransfrom()
{
    mTransform.reset();
    mTransform.rotate(mdRotateAngle);
    mTransform.scale(mdSx, mdSy);
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
