#include "simage.h"
#include <QMutex>

SImage::SImage(const QString &_imagePath, bool _selected, QPointF center, const QString &_layerName, const QString &_layerDiscription, const QColor &_layerColor)
    : SObject(PaintObject::ImageBase, _selected, center, _layerName, _layerDiscription, _layerColor)
{
    if(_imagePath.isEmpty())
        return;
    load(mStrImagePath);

}

SImage::SImage(const SImage &theImage): SObject(PaintObject::ImageBase)
{
    _initializeWith(theImage);
}

SImage::~SImage()
{
    releaseImage();
}

SImage &SImage::operator=(const SImage &theImage)
{
    if(this == &theImage)
        return *this;
    _initializeWith(theImage);
    return *this;
}

void SImage::paint(QPainter &painter, bool doTranslate, QRectF viewLogicalArea, double scaleValue)const
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


void SImage::load(const QString &_imagePath)
{
    if(!_imagePath.isEmpty())
        mStrImagePath = _imagePath;
    if(!mStrImagePath.isEmpty())
        mpImage = new QPixmap(mStrImagePath);
    //显示区域初始化
    QPointF topLeft(-mpImage->width() / 2, -mpImage->height() / 2);
    QPointF topRight(mpImage->width() / 2, -mpImage->height() / 2);
    QPointF bottomLeft(-mpImage->width() / 2, mpImage->height() / 2);
    QPointF bottomRight(mpImage->width() / 2, mpImage->height() / 2);
    //确定外围矩形
    this->mImageRect.setTopLeft(topLeft);
    this->mImageRect.setBottomRight(bottomRight);
    //应用变换
    _applyTransform();
}

void SImage::_initializeWith(const SImage &theImage)
{
    SObject::_initializeWith(theImage);
    mStrImagePath = theImage.mStrImagePath;
    if(mpImage)
        delete mpImage;
    if(theImage.mpImage)
        mpImage = new QPixmap(*theImage.mpImage);
    mImageRect = theImage.mImageRect;
    for(int i = 0; i < 4; ++i)
        mpBoundPt[i] = theImage.mpBoundPt[i];
}
