#include "simage.h"

SImage::SImage(PaintObject _type, QPixmap *image, bool _selected, QPoint center, const QString &_layerName, const QString &_layerDiscription, const QColor &_layerColor)
    : SObject(_type, _selected, center, _layerName, _layerDiscription, _layerColor)
{
    if(image->isNull())
        return;
    this->mpImage = image;
    //显示区域初始化
    QPointF topLeft(-image->width() / 2, -image->height() / 2);
    QPointF topRight(image->width() / 2, -image->height() / 2);
    QPointF bottomLeft(-image->width() / 2, image->height() / 2);
    QPointF bottomRight(image->width() / 2, image->height() / 2);
    //确定包围控制点
    mpBoundPt[0] = topLeft;
    mpBoundPt[1] = topRight;
    mpBoundPt[2] = bottomRight;
    mpBoundPt[3] = bottomLeft;
    //确定外围矩形
    this->mImageRect.setTopLeft(topLeft);
    this->mImageRect.setBottomRight(bottomRight);
}

SImage::~SImage()
{
    if(mpImage)
        delete mpImage;
    mpImage = nullptr;
}

void SImage::paint(QPainter &painter, bool doTranslate) const
{
    if(!mpImage)
        return;

    //平移到中心点
    if(doTranslate)
        painter.translate(mPtCenter);

    //执行变换
    painter.setTransform(mTransform * painter.transform());
    //绘图
    painter.drawPixmap(mImageRect, *mpImage, mpImage->rect());
    //执行逆变换
    painter.setTransform(mTransform.inverted() * painter.transform());

    //返回原点
    if(doTranslate)
        painter.translate(-mPtCenter);
}

QPolygonF SImage::boundingRect() const
{
    return QPolygonF(QVector<QPointF>(
    {mpBoundPt[0], mpBoundPt[1], mpBoundPt[2], mpBoundPt[3]} ))
    .translated(mPtCenter);
}

bool SImage::contains(const QPointF &pt) const
{
    QPainterPath path;
    path.moveTo(mpBoundPt[0]);
    path.lineTo(mpBoundPt[1]);
    path.lineTo(mpBoundPt[2]);
    path.lineTo(mpBoundPt[3]);
    path.closeSubpath();

    return path.contains(this->AtoC(pt));
}

void SImage::writeBinaryData(QDataStream &stream) const
{

}

void SImage::readBinaryData(QDataStream &stream)
{

}

void SImage::_applyTransform()
{
    mpBoundPt[0] = mTransform.map(mImageRect.topLeft());
    mpBoundPt[1] = mTransform.map(mImageRect.topRight());
    mpBoundPt[2] = mTransform.map(mImageRect.bottomRight());
    mpBoundPt[3] = mTransform.map(mImageRect.bottomLeft());
}
