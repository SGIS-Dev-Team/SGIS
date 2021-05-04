#include "sobject.h"


SObject::SObject(PaintObject _type, bool _selected, QPoint center,
                 const QString &_layerName,
                 const QString &_layerDiscription,
                 const QColor &_layerColor)
    :   type(_type), mbSelected(_selected), mPtCenter(center),
        mStrLayerName(_layerName),
        mStrLayerDiscription(_layerDiscription),
        mLayerColor(_layerColor) {}

SObject::~SObject() {}

void SObject::paintBoundRect(QPainter &painter, bool doTranslate)
{
    //保存原来的样式
    const QPen& oldPen = painter.pen();
    const QBrush& oldBrush = painter.brush();
    QPainter::CompositionMode oldCompoMode = painter.compositionMode();
    //定义外界矩形样式
    QPen pen;
    pen.setWidth(BOUND_RECT_PEN_WIDTH);
    QBrush brush;
    brush.setColor(QColor(255, 255, 255, 200));
    //设置为外界矩形样式
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setCompositionMode(oldCompoMode);

    QRectF bound_rect = this->rect();

    //------------绘图-------------//

    //绘制矩形
    painter.drawRect(bound_rect);
    //绘制操作点
    pen.setWidth(1);
    //四个角点
    painter.drawEllipse(bound_rect.topLeft(), 2 * BOUND_RECT_PEN_WIDTH, 2 * BOUND_RECT_PEN_WIDTH);
    painter.drawEllipse(bound_rect.topRight(), 2 * BOUND_RECT_PEN_WIDTH, 2 * BOUND_RECT_PEN_WIDTH);
    painter.drawEllipse(bound_rect.bottomLeft(), 2 * BOUND_RECT_PEN_WIDTH, 2 * BOUND_RECT_PEN_WIDTH);
    painter.drawEllipse(bound_rect.bottomRight(), 2 * BOUND_RECT_PEN_WIDTH, 2 * BOUND_RECT_PEN_WIDTH);
    //四个边中点
    painter.drawEllipse((bound_rect.topLeft() + bound_rect.topRight()) / 2, 2 * BOUND_RECT_PEN_WIDTH, 2 * BOUND_RECT_PEN_WIDTH);
    painter.drawEllipse((bound_rect.topLeft() + bound_rect.bottomLeft()) / 2, 2 * BOUND_RECT_PEN_WIDTH, 2 * BOUND_RECT_PEN_WIDTH);
    painter.drawEllipse((bound_rect.bottomRight() + bound_rect.topRight()) / 2, 2 * BOUND_RECT_PEN_WIDTH, 2 * BOUND_RECT_PEN_WIDTH);
    painter.drawEllipse((bound_rect.bottomRight() + bound_rect.bottomLeft()) / 2, 2 * BOUND_RECT_PEN_WIDTH, 2 * BOUND_RECT_PEN_WIDTH);

    //------------绘图-------------//

    //还原样式
    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
    painter.setCompositionMode(oldCompoMode);
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
    mdRotateAngle = angle;
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
