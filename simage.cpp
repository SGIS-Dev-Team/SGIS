#include "simage.h"

SImage::SImage(PaintObject _type, const QImage &image, bool _selected, QPoint center, const QString &_layerName, const QString &_layerDiscription, const QColor &_layerColor)
    : SShape(_type, _selected, center, _layerName, _layerDiscription, _layerColor)
{
    if(image.isNull())
        return;
    //图像初始化
    this->setImage(image);
    //显示区域初始化
    QPointF leftTop(-image.width() / 2, -image.height() / 2);
    QPointF rightTop(image.width() / 2, -image.height() / 2);
    QPointF rightBottom(image.width() / 2, image.height() / 2);
    QPointF leftBottom(-image.width() / 2, image.height() / 2);

    this->addVertex({this->CtoA(leftTop), this->CtoA(rightTop), this->CtoA(rightBottom), this->CtoA(leftBottom)});

    this->mImageRect = QRectF(leftTop, rightBottom);

    this->setUpdate(false);
    updatePath();
}

void SImage::updatePath()
{
    mPath.addRect(this->mImageRect);
    mPath.closeSubpath();
}

void SImage::paint(QPainter *painter) const
{
    //保存原来的样式
    const QPen& oldPen = painter->pen();
    //设置为本形状样式
    painter->setPen(mPen);
    //平移到中心点
    painter->translate(mPtCenter);
    //绘图
    painter->drawPath(mPath);
    painter->drawImage(mImageRect, this->mFillImage, mFillImage.rect());
    //返回原点
    painter->translate(-mPtCenter);
    //还原样式
    painter->setPen(oldPen);
}
