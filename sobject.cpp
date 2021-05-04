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
