#include "slinestringfeature.h"

SLineStringFeature::SLineStringFeature(bool _selected, QPointF _center, const QString& _layerName, const QString& _layerDiscription, const QColor& _layerColor)
    : SFeature(PaintObject::LineStringFeature, _selected, _center, _layerName, _layerDiscription, _layerColor)
{

}

void SLineStringFeature::paint(QPainter& painter, bool doTranslate, const QRectF& viewLogicalArea, double scaleValue, PaintTrigger trigger) const
{
    Q_UNUSED(viewLogicalArea);
    Q_UNUSED(scaleValue);
    Q_UNUSED(trigger);
    //保存原来的样式
    QPen oldPen = painter.pen();
    QBrush oldBrush = painter.brush();
    QTransform oldTransform = painter.transform();
    //设置为本形状样式
    painter.setPen(mPen);
    painter.setBrush(mBrush);
    //平移到中心点
    if (doTranslate)
        painter.translate(mPtCenter);

    //绘图核心部分
    for (const std::vector<QPointF>& ptVec : mItemPointsVecVec)
    {
        for (std::vector<QPointF>::size_type i = 0; i < ptVec.size() - 1; ++i)
        {
            painter.drawLine(ptVec[i], ptVec[i + 1]);
        }
    }
    //绘图前后都不用改

    //还原样式
    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
    painter.setTransform(oldTransform);
}

bool SLineStringFeature::contains(const QPointF& pt, bool isInBoundRect) const
{
    //线状要素直接返回false
    return false;
}

void SLineStringFeature::writeBinaryData(QDataStream& stream) const
{

}

void SLineStringFeature::readBinaryData(QDataStream& stream)
{

}
