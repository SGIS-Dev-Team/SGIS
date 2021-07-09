#include "sfeature.h"

SFeature::SFeature(PaintObject _type, bool _selected, QPointF _center, const QString& _layerName, const QString& _layerDiscription, const QColor& _layerColor)
    : SObject(_type, _selected, _center, _layerName, _layerDiscription, _layerColor)
{

}

SFeature::~SFeature()
{

}

QPolygonF SFeature::boundingRect() const
{
    double xMin{DBL_MAX}, yMin{DBL_MAX}, xMax{-DBL_MAX}, yMax{-DBL_MAX};
    for (const auto& ptVec : mItemPointsVecVec)
    {
        for (const auto& pt : ptVec)
        {
            if (xMin > pt.x())
                xMin = pt.x();
            if (xMax < pt.x())
                xMax = pt.x();
            if (yMin > pt.y())
                yMin = pt.y();
            if (yMax < pt.y())
                yMax = pt.y();
        }
    }

    return QPolygonF(QRectF(QPointF(xMin, yMin), QPointF(xMax, yMax)));
}

QIcon SFeature::icon() const
{
    return QIcon();
}

void SFeature::_applyTransform()
{
    //先进行地理到对象坐标的映射，再进行对象坐标的变换
    mItemPointsVecVec.clear();
    mItemPointsVecVec.reserve(mGeoPointsVecVec.size());
    for (decltype(mGeoPointsVecVec)::size_type i = 0; i < mGeoPointsVecVec.size(); ++i)
    {
        std::vector<QPointF> itemPtVec;
        itemPtVec.reserve(mGeoPointsVecVec[i].size());
        for (auto pt : mGeoPointsVecVec[i])
        {
            //地理到画布
            if (mpGeoRef)
                mpGeoRef->mapFromGeo(pt.x(), pt.y(), pt.rx(), pt.ry());
            //画布到对象
            pt -= mPtCenter;
            //对象坐标系内变换
            itemPtVec.push_back(mTransform.map(pt));
        }
        mItemPointsVecVec.push_back(std::move(itemPtVec));
    }
}

QRectF SFeature::_originalRect()
{
    return QRectF();
}
