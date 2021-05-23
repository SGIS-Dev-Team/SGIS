#include "simage.h"
#include <QFileInfo>
#include <modules/algorithm/sequalizehist.h>

SImage::SImage(const QString &_imagePath, bool _selected, QPointF center, const QString &_layerName, const QString &_layerDiscription, const QColor &_layerColor)
    : SObject(PaintObject::ImageBase, _selected, center, _layerName, _layerDiscription, _layerColor)
{
    mStrImagePath = _imagePath;
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

    QTransform oldTransform = painter.transform();

    //平移到中心点
    if(doTranslate)
        painter.translate(mPtCenter);

    //执行变换
    painter.setTransform(mTransform * painter.transform());
    //绘图
    if(mpImage)
        painter.drawImage(mImageRect, *mpImage, mpImage->rect());

    painter.setTransform(oldTransform);
}

QPolygonF SImage::boundingRect() const
{
    return QPolygonF(QVector<QPointF>(
    {mpBoundPt[0], mpBoundPt[1], mpBoundPt[2], mpBoundPt[3]} ))
    .translated(mPtCenter);
}

bool SImage::contains(const QPointF &pt, bool isInBoundRect) const
{
    return this->boundingRect().containsPoint(pt, Qt::WindingFill);
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

QRectF SImage::_originalRect()
{
    return mImageRect;
}

int SImage::getBandCount() const
{
    return mnBands;
}

void SImage::getBandIdices(int *pRGBIdx) const
{
    pRGBIdx[0] = mnRedBandIdx;
    pRGBIdx[1] = mnGreenBandIdx;
    pRGBIdx[2] = mnBlueBandIdx;
}

int SImage::blueBandIdx() const
{
    return mnBlueBandIdx;
}

bool SImage::isMultiBand() const
{
    return mnBands > 3;
}

void SImage::getHistEqFunc(std::shared_ptr<void> pEqFunc[])
{
    for(int i = 0; i < 3; ++i)
        pEqFunc[i] = mpEqualizeFunc[i];
}

void SImage::setBlueBandIdx(int value)
{
    mnBlueBandIdx = value;
}

void SImage::setBandIndices(int r, int g, int b)
{
    Q_ASSERT(r > 0 && g > 0 && b > 0);

    mnRedBandIdx = r;
    mnGreenBandIdx = g;
    mnBlueBandIdx = b;
}

void SImage::setDefaultedBands()
{
    mnRedBandIdx = 1;
    mnGreenBandIdx = 1;
    mnBlueBandIdx = 1;
}

void SImage::setHistEqFunc(std::shared_ptr<void> pEqFunc[])
{
    for(int i = 0; i < 3; ++i)
        mpEqualizeFunc[i] = pEqFunc[i];
}

int SImage::greenBandIdx() const
{
    return mnGreenBandIdx;
}

void SImage::setGreenBandIdx(int value)
{
    mnGreenBandIdx = value;
}

int SImage::redBandIdx() const
{
    return mnRedBandIdx;
}

void SImage::setRedBandIdx(int value)
{
    mnRedBandIdx = value;
}

void SImage::load(const QString &_imagePath)
{
    if(!_imagePath.isEmpty())
        mStrImagePath = _imagePath;
    if(mStrImagePath.isEmpty())
        return;

    releaseImage();

    //------加载图片------//

    //判断文件后缀是否为*.tif或*.tiff
    QFileInfo imgInfo(mStrImagePath);

    if(imgInfo.suffix() != "tiff" && imgInfo.suffix() != "tif")
        mpImage = new QImage(mStrImagePath);
    else
    {
        GDALAllRegister();

        QByteArray strImagePath_byte = mStrImagePath.toUtf8();
        GDALDataset* pDataSet = static_cast<GDALDataset*>(GDALOpen(strImagePath_byte.constData(), GA_ReadOnly));

        //获取数据集信息
        int nWidth = pDataSet->GetRasterXSize();
        int nHeight = pDataSet->GetRasterYSize();
        int nPixels = nWidth * nHeight;
        mnBands = pDataSet->GetRasterCount();
        GDALDataType nType = pDataSet->GetRasterBand(1)->GetRasterDataType();

        //分配内存、读入各波段、直方图均衡化，并拉伸为8位
        uchar* pBandData[3] {nullptr};
        uchar* pBand8bit[3] {nullptr};
        int pBandIdx[3];

        this->getBandIdices(pBandIdx);

        for(int i = 0; i < 3; ++i)
        {
            pBandData[i] = new uchar[nPixels * GDALGetDataTypeSizeBytes(nType)];
            pDataSet->GetRasterBand(pBandIdx[i])->RasterIO(GF_Read, 0, 0, nWidth, nHeight, pBandData[i], nWidth, nHeight, nType, 0, 0, 0);

            if(!mpEqualizeFunc[i])
                mpEqualizeFunc[i] = calcHistEqFunc(nType, pBandData[i], nPixels);
            histEqualize(nType, pBandData[i], nPixels, mpEqualizeFunc[i]);
            pBand8bit[i] = to8bit(nType, pBandData[i], nPixels);
            //释放原始波段数据内存
            delete [] pBandData[i];
        }

        //合成RGB影像
        uchar *pRGBData = new uchar[nPixels * 3 * sizeof (uchar)];
        for(int i = 0; i < nPixels; ++i)
            for(int band = 0; band < 3; ++band)
                pRGBData[i * 3 + band] = pBand8bit[band][i];

        for(int i = 0; i < 3; ++i)
            delete [] pBand8bit[i];

        //创建QPixmap用于显示
        mpImage = new QImage(pRGBData, nWidth, nHeight, nWidth * 3, QImage::Format_RGB888,
                             [](void* pData)->void{delete [] static_cast<uchar*>(pData); pData = nullptr;});

        GDALClose(pDataSet);
    }

//------显示区域初始化------//

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

bool SImage::save(const QString &_savePath)
{
    if(mpImage)
        return mpImage->save(_savePath);
    return false;
}

void SImage::_initializeWith(const SImage &theImage)
{
    SObject::_initializeWith(theImage);
    mStrImagePath = theImage.mStrImagePath;
    if(mpImage)
        delete mpImage;
    if(theImage.mpImage)
        mpImage = new QImage(*theImage.mpImage);
    mImageRect = theImage.mImageRect;
    for(int i = 0; i < 4; ++i)
        mpBoundPt[i] = theImage.mpBoundPt[i];
}

std::shared_ptr<void> SImage::calcHistEqFunc(GDALDataType type, void *pBandData, size_t count)
{
    std::shared_ptr<void> pFunc{nullptr};
    switch (type)
    {
    case GDT_Byte:
        pFunc = getEqualizeFunction<UINT8>(static_cast<UINT8*>(pBandData), count);
        break;
    case GDT_UInt16:
        pFunc = getEqualizeFunction<UINT16>(static_cast<UINT16*>(pBandData), count);
        break;
    case GDT_UInt32:
        pFunc = getEqualizeFunction<UINT32>(static_cast<UINT32*>(pBandData), count);
        break;
    default:
        Q_ASSERT(0);
        break;
    }
    return pFunc;
}

void SImage::histEqualize(GDALDataType type, void *pBandData, size_t count, std::shared_ptr<void> pEqFunc)
{
    switch (type)
    {
    case GDT_Byte:
        equalizeHist<UINT8>(static_cast<UINT8*>(pBandData), count, static_cast<UINT8*>(pEqFunc.get()));
        break;
    case GDT_UInt16:
        equalizeHist<UINT16>(static_cast<UINT16*>(pBandData), count, static_cast<UINT16*>(pEqFunc.get()));
        break;
    case GDT_UInt32:
        equalizeHist<UINT32>(static_cast<UINT32*>(pBandData), count, static_cast<UINT32*>(pEqFunc.get()));
        break;
    default:
        Q_ASSERT(0);
        break;
    }
}


template<typename T>
uchar* _to8bit(T* pBandData, size_t count, T divisor)
{
    uchar* p8bitData = new uchar[count];
    for(size_t i = 0; i < count; ++i)
        p8bitData[i] = pBandData[i] / divisor;
    return p8bitData;
}

uchar *SImage::to8bit(GDALDataType type, void *pBandData, size_t count)
{
    uchar* p8bitData{nullptr};

    switch (type)
    {
    case GDT_Byte:
    {
        p8bitData = new uchar[count];
        memcpy_s(p8bitData, count * sizeof(uchar), pBandData, count * sizeof(uchar));
        break;
    }
    case GDT_UInt16:
        p8bitData = _to8bit<UINT16>(static_cast<UINT16*>(pBandData), count, static_cast<UINT16>(257));
        break;
    case GDT_UInt32:
        p8bitData = _to8bit<UINT32>(static_cast<UINT32*>(pBandData), count, static_cast<UINT32>(16843009));
        break;
    default:
        Q_ASSERT(0);
        break;
    }

    return p8bitData;
}
