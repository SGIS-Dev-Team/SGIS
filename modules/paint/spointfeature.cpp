#include "spointfeature.h"

SPointFeature::SPointFeature(bool _selected, QPointF point, const QString &_layerName, const QString &_layerDiscription, const QColor &_layerColor)
    :SFeature(PaintObject::PointFeature, _selected, point, _layerName, _layerDiscription, _layerColor)
{

}

void SPointFeature::addPoint(QPointF point)
{
    if (mItemPointsVecVec.size() == 0)
    {
        mItemPointsVecVec.push_back(std::vector<QPointF>());
    }
    mItemPointsVecVec[0].push_back(point);
}

void SPointFeature::paint(QPainter &painter, bool doTranslate, const QRectF &viewLogicalArea, double scaleValue, SObject::PaintTrigger trigger) const
{
    Q_UNUSED(viewLogicalArea);
    Q_UNUSED(scaleValue);
    Q_UNUSED(trigger);
    if (mItemPointsVecVec.size() == 0)
    {
        return;
    }
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
    painter.drawPoints(mItemPointsVecVec[0].data(), mItemPointsVecVec[0].size());
    //绘图前后都不用改

    //还原样式
    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
    painter.setTransform(oldTransform);
}

bool SPointFeature::contains(const QPointF& pt, bool isInBoundRect) const
{
    //线状要素直接返回false
    return false;
}

void SPointFeature::writeBinaryData(QDataStream& stream) const
{

}

void SPointFeature::readBinaryData(QDataStream& stream)
{

}
