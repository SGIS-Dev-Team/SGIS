#include "simage.h"
#include <QFileInfo>
#include <modules/algorithm/sequalizehist.h>

#include "gdal.h"
#include "gdal_priv.h"
#include "gdal_alg.h"
#include <QJsonArray>
SImage::SImage(const QString& _imagePath, bool _selected, QPointF center, const QString& _layerName, const QString& _layerDiscription, const QColor& _layerColor)
    : SObject(PaintObject::ImageBase, _selected, center, _layerName, _layerDiscription, _layerColor)
{
    mStrImagePath = _imagePath;
}

SImage::SImage(const SImage& theImage): SObject(PaintObject::ImageBase)
{
    _initializeWith(theImage);
}

SImage::~SImage()
{
    releaseImage();
}

SImage& SImage::operator=(const SImage& theImage)
{
    if (this == &theImage)
        return *this;
    _initializeWith(theImage);
    return *this;
}

void SImage::paint(QPainter& painter, bool doTranslate, const QRectF& viewLogicalArea, double scaleValue, PaintTrigger trigger)const
{
    Q_UNUSED(viewLogicalArea);
    Q_UNUSED(scaleValue);
    Q_UNUSED(trigger);

    if (!mpImage)
        return;

    QTransform oldTransform = painter.transform();

    //平移到中心点
    if (doTranslate)
        painter.translate(mPtCenter);

    //执行变换
    painter.setTransform(mTransform * painter.transform());

    //绘图
    const_cast<SImage*>(this)->mMutex.lock();
    painter.drawPixmap(mImageRect, *mpImage, mpImage->rect());
    const_cast<SImage*>(this)->mMutex.unlock();

    painter.setTransform(oldTransform);

}

const QPixmap& SImage::getImage() const
{
    return *mpImage;
}

const QString& SImage::getImagePath() const
{
    return mStrImagePath;
}

void SImage::setImagePath(const QString& imagePath)
{
    mStrImagePath = imagePath;
}

void SImage::setImage(const QPixmap& image)
{
    if (!image.isNull())
        mpImage = new QPixmap(image);
    mStrImagePath = nullptr;
}

void SImage::releaseImage()
{
    mMutex.lock();

    if (mpImage)
        delete mpImage;
    mpImage = nullptr;

    mMutex.unlock();
}

void SImage::releaseMemory()
{
    for (auto& ptr : mpBandData)
        ptr.reset();
    mpImageData.reset();
    releaseImage();
}

bool SImage::isNull()const
{
    return mpImage == nullptr;
}

QPolygonF SImage::boundingRect() const
{
    return QPolygonF(QVector<QPointF>(
    {mpBoundPt[0], mpBoundPt[1], mpBoundPt[2], mpBoundPt[3]}))
    .translated(mPtCenter);
}

bool SImage::contains(const QPointF& pt, bool isInBoundRect) const
{
    Q_UNUSED(isInBoundRect);
    return this->boundingRect().containsPoint(pt, Qt::WindingFill);
}

void SImage::writeBinaryData(QDataStream& stream) const
{

}

void SImage::readBinaryData(QDataStream& stream)
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

void SImage::getBandIdices(int* pRGBIdx) const
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

const QRect& SImage::getLoadRegionRect() const
{
    return mLoadRect;
}

const QSize& SImage::getLoadResampledSize() const
{
    return mImageSize;
}

void SImage::getHistEqFunc(std::shared_ptr<void> pEqFunc[])const
{
    for (int i = 0; i < 3; ++i)
        pEqFunc[i] = mpEqualizeFunc[i];
}

void SImage::setHistEqFunc(std::shared_ptr<void> pEqFunc[])
{
    for (int i = 0; i < 3; ++i)
        mpEqualizeFunc[i] = pEqFunc[i];
}

void SImage::setLoadRegionRect(const QRect& rect)
{
    mLoadRect = rect;
}

void SImage::setLoadRegionResampledSize(const QSize& size)
{
    mImageSize = size;
}

QJsonObject SImage::getRawMetaInfo()
{
    GDALDataset* poDataset = SImage::_getOpenDataSet(mStrImagePath);
    if (poDataset == NULL)
    {
        return QJsonObject();
    }
    QJsonObject obj;
    //获取图像波段
    //我不知道这些是啥意义
    GDALRasterBand* poBand1 = poDataset->GetRasterBand(1);
    auto nX = poBand1->GetXSize();
    auto nY = poBand1->GetYSize();
    auto nBand = poBand1->GetBand();
    //
    obj["Band"] = nBand;
    obj["BandXSize"] = nX;
    obj["BandYSize"] = nY;

    //获取图像的尺寸
    int nImgSizeX = poDataset->GetRasterXSize();
    int nImgSizeY = poDataset->GetRasterYSize();

    //
    obj["RasterXSize"] = nImgSizeX;
    obj["RasterYSize"] = nImgSizeY;
    //
    //获取坐标变换系数
    double trans[6];
    CPLErr aaa = poDataset->GetGeoTransform(trans);
    QJsonArray transArray;
    for (auto i = 0; i < sizeof(trans) / sizeof(trans[0]); i++)
    {
        transArray.append(trans[i]);
    }
    obj["GeoTransform"] = transArray;

    //读取图像高程数据
    QJsonArray geoArray;
    auto pafScanblock1 = (unsigned char*)CPLMalloc(sizeof(char) * (nImgSizeX) * (nImgSizeY));
    poBand1->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY, pafScanblock1, nImgSizeX, nImgSizeY, GDALDataType(poBand1->GetRasterDataType()), 0, 0);
    for (int i = 0; i < (nImgSizeX - 5990); i++)
    {
        for (int j = 0; j < (nImgSizeY - 5990); j++)
        {
            auto elevation = *pafScanblock1;
            auto Xgeo = trans[0] + i * trans[1] + j * trans[2];
            auto Ygeo = trans[3] + i * trans[4] + j * trans[5];
            pafScanblock1++;
            QJsonObject geo;
            geo["XGeo"] = Xgeo;
            geo["YGeo"] = Ygeo;
            geo["Elevation"] = elevation;
            geoArray.append(Xgeo);
        }
    }
    obj["Geo"] = geoArray;
    return obj;
}

std::unique_ptr<uchar[], std::default_delete<uchar[]>> SImage::loadBand(int x_off, int y_off, int x_span, int y_span,
                                                                        int image_width, int image_height,
                                                                        const QString& imagePath, int bandIdx, GDALDataType dataType,
                                                                        std::shared_ptr<void> pEqFunc)
{
    GDALDataset* pDataSet = SImage::_getOpenDataSet(imagePath);
    std::unique_ptr<uchar[], std::default_delete<uchar[]>> pBandData = loadBand(x_off, y_off, x_span, y_span,
                                                                                image_width, image_height,
                                                                                pDataSet,
                                                                                bandIdx,
                                                                                dataType,
                                                                                pEqFunc);
    GDALClose(pDataSet);
    return pBandData;
}

std::unique_ptr<uchar[], std::default_delete<uchar[]>> SImage::loadBand(int x_off, int y_off, int x_span, int y_span,
                                                                        int image_width, int image_height,
                                                                        GDALDataset* pDataSet, int bandIdx, GDALDataType dataType,
                                                                        std::shared_ptr<void> pEqFunc)
{
    //执行越界检查
#ifndef QT_NO_DEBUG
    SImageMeta meta = SImage::getMetaOf(pDataSet);
    Q_ASSERT(image_width >= 0 && image_height >= 0);

    Q_ASSERT(x_off >= 0 && y_off >= 0);
    Q_ASSERT(x_span >= 0 && y_span >= 0);
    Q_ASSERT(x_off + x_span <= meta.width() && y_off + y_span <= meta.height());
    Q_ASSERT(bandIdx >= 1 && bandIdx <= pDataSet->GetRasterCount());
#endif

    int nPixels = image_width * image_height;

    //分配内存、读入波段、直方图均衡化
    std::unique_ptr<uchar[], std::default_delete<uchar[]>> pBandData(new uchar[nPixels * GDALGetDataTypeSizeBytes(dataType)] {});

    pDataSet->GetRasterBand(bandIdx)->RasterIO(
        GF_Read,
        x_off, y_off, x_span, y_span,
        pBandData.get(),
        image_width, image_height,
        dataType,
        0, 0, 0);

    if (pEqFunc)
        SImage::histEqualize(dataType, pBandData.get(), nPixels, pEqFunc.get());

    return pBandData;
}

std::unique_ptr<uchar[], std::default_delete<uchar[]>> SImage::merge(const uchar* pBandData[], GDALDataType dataType, int pixelCount, int bandCount)
{
    //分配内存
    std::unique_ptr<uchar[], std::default_delete<uchar[]>> pMergedData(new uchar [bandCount * pixelCount * sizeof(uchar)]);

    //合成逐像素存储图像数据
    for (int i = 0; i < bandCount; ++i)
    {
        uchar* pm = pMergedData.get() + i;
        std::unique_ptr<uchar[], std::default_delete<uchar[]>> pBand8Bit = to8bit(dataType, pBandData[i], pixelCount);
        uchar* pb = pBand8Bit.get();
        uchar* pbEnd = pb + pixelCount;
        for (; pb != pbEnd; pm += bandCount, ++pb)
            * pm = *pb;
    }

    return pMergedData;
}
int SImage::greenBandIdx() const
{
    return mnGreenBandIdx;
}

void SImage::setBlueBandIdx(int value, bool reload, std::shared_ptr<void> pHistEqFunc)
{
    mnBlueBandIdx = value;
    if (reload)
        _reloadChannel(2, value, pHistEqFunc);
    else
        mpEqualizeFunc[2].reset();
}

void SImage::setBandIndex(int channel, int bandIdx, bool reload, std::shared_ptr<void> pHistEqFunc)
{
    switch (channel)
    {
    case 0:
        setRedBandIdx(bandIdx, reload, pHistEqFunc);
        break;
    case 1:
        setGreenBandIdx(bandIdx, reload, pHistEqFunc);
        break;
    case 2:
        setBlueBandIdx(bandIdx, reload, pHistEqFunc);
        break;
    default:
        Q_ASSERT(0);
        break;
    }
}

void SImage::presetBandIndices(int r, int g, int b)
{
    std::shared_ptr<void> pHistEqFuncs[3] = {nullptr, nullptr, nullptr};
    presetBandIndices(r, g, b, pHistEqFuncs);
}

void SImage::presetBandIndices(int r, int g, int b, std::shared_ptr<void> pHistEqFuncs[])
{
    Q_ASSERT(r > 0 && g > 0 && b > 0);
    setRedBandIdx(r, false, pHistEqFuncs[0]);
    setGreenBandIdx(g, false, pHistEqFuncs[1]);
    setBlueBandIdx(b, false, pHistEqFuncs[2]);
}

void SImage::setBandIndices(int r, int g, int b)
{
    std::shared_ptr<void> pHistEqFuncs[3] = {nullptr, nullptr, nullptr};
    setBandIndices(r, g, b, pHistEqFuncs);
}

void SImage::setBandIndices(int r, int g, int b, std::shared_ptr<void> pHistEqFuncs[])
{
    Q_ASSERT(r > 0 && g > 0 && b > 0);
    Q_ASSERT(r <= mnBands && g <= mnBands && b <= mnBands);
    if (r != mnRedBandIdx)
        setRedBandIdx(r, true, pHistEqFuncs[0]);
    if (g != mnGreenBandIdx)
        setGreenBandIdx(g, true, pHistEqFuncs[1]);
    if (b != mnBlueBandIdx)
        setBlueBandIdx(b, true, pHistEqFuncs[2]);
}

void SImage::setDefaultedBands()
{
    if (mnBands >= 3)
        setBandIndices(1, 2, 3);
    else
        setBandIndices(1, 1, 1);
}

void SImage::presetDefaultedBands()
{
    presetBandIndices(1, 1, 1);
}

void SImage::setGreenBandIdx(int value, bool reload, std::shared_ptr<void> pHistEqFunc)
{
    mnGreenBandIdx = value;
    if (reload)
        _reloadChannel(1, value, pHistEqFunc);
    else
        mpEqualizeFunc[1].reset();
}

int SImage::redBandIdx() const
{
    return mnRedBandIdx;
}

void SImage::setRedBandIdx(int value, bool reload, std::shared_ptr<void> pHistEqFunc)
{
    mnRedBandIdx = value;
    if (reload)
        _reloadChannel(0, value, pHistEqFunc);
    else
        mpEqualizeFunc[0].reset();
}

void SImage::load(const QString& imagePath)
{
    load(mLoadRect, imagePath);
}

void SImage::load(int x_off, int y_off, int x_span, int y_span, const QString& imagePath)
{
    Q_ASSERT(x_off >= 0 && y_off >= 0 && x_span >= 0 && y_span >= 0);
    load(x_off, y_off, x_span, y_span,
         mImageSize.width(), mImageSize.height(),
         imagePath);
}

void SImage::load(const QRect& rect, const QString& imagePath)
{
    load(rect.left(), rect.top(), rect.width(), rect.height(), imagePath);
}

void SImage::load(int x_off, int y_off, int x_span, int y_span, int image_width, int image_height, const QString& imagePath)
{
    if (!imagePath.isEmpty())
        mStrImagePath = imagePath;
    if (mStrImagePath.isEmpty())
        return;

    GDALDataset* pDataSet = SImage::_getOpenDataSet(mStrImagePath);
    load(x_off, y_off, x_span, y_span, image_width, image_height, pDataSet);
    GDALClose(pDataSet);
}

void SImage::load(int x_off, int y_off, int x_span, int y_span, int image_width, int image_height, GDALDataset* pDataSet)
{
    SImageMeta meta = getMetaOf(pDataSet);
    mnBands = meta.bandCount();
    int nPixels = image_width * image_height;
    mnDataType = meta.dataType();

    if (x_span == 0)
        x_span = meta.width() - x_off;
    if (y_span == 0)
        y_span = meta.height() - y_off;

    if (image_width == 0)
        image_width = x_span;
    if (image_height == 0)
        image_height = y_span;

    Q_ASSERT(x_span > 0);
    Q_ASSERT(y_span > 0);
    Q_ASSERT(image_width > 0);
    Q_ASSERT(image_height > 0);

    //更新读取图像区域
    mLoadRect.setRect(x_off, y_off, x_span, y_span);
    mImageSize.setWidth(image_width);
    mImageSize.setHeight(image_height);

    releaseImage();
    //------加载图片------//

    //获取描述数据

    //读入选中的各波段数据
    if (mnRedBandIdx == 0 || mnGreenBandIdx == 0 || mnBlueBandIdx == 0)
        this->presetDefaultedBands();
    int pBandIdx[3] = {mnRedBandIdx, mnGreenBandIdx, mnBlueBandIdx};
    for (int i = 0; i < 3; ++i)
    {
        mpBandData[i] = this->loadBand(x_off, y_off, x_span, y_span,
                                       image_width, image_height,
                                       pDataSet,
                                       pBandIdx[i],
                                       mnDataType);
        //直方图均衡化
        if (!mpEqualizeFunc[i])
            mpEqualizeFunc[i] = SImage::calcHistEqFunc(mnDataType, mpBandData[i].get(), nPixels);
        SImage::histEqualize(mnDataType, mpBandData[i].get(), nPixels, mpEqualizeFunc[i].get());
    }

    //融合图像，并创建QImage用于显示
    const uchar* pBandData[3] = {mpBandData[0].get(), mpBandData[1].get(), mpBandData[2].get()};
    mpImageData = SImage::merge(pBandData, mnDataType, nPixels, 3);

    _updateImage();
    _updateImageRect();

    //应用变换
    _applyTransform();
}

void SImage::load(const QRect& rect, double resampling_ratio, const QString& imagePath)
{
    load(rect.left(), rect.top(), rect.width(), rect.height(),
         resampling_ratio,
         imagePath);
}

void SImage::load(int x_off, int y_off, int x_span, int y_span, double resampling_ratio, const QString& imagePath)
{
    Q_ASSERT(resampling_ratio > 0);

    if (x_span == 0 || y_span == 0)
    {
        SImageMeta meta = getMetaOf(imagePath);
        if (x_span == 0)
            x_span = meta.width() - x_off;
        if (y_span == 0)
            y_span = meta.height() - y_off;
    }

    load(x_off, y_off, x_span, y_span,
         x_span * resampling_ratio, y_span * resampling_ratio,
         imagePath);
}

bool SImage::save(const QString& _savePath)
{
    if (mpImage)
        return mpImage->save(_savePath);
    return false;
}

void SImage::_initializeWith(const SImage& theImage)
{
    SObject::_initializeWith(theImage);
    mStrImagePath = theImage.mStrImagePath;
    if (mpImage)
        delete mpImage;
    if (theImage.mpImage)
        mpImage = new QPixmap(*theImage.mpImage);

    mImageRect = theImage.mImageRect;
    for (int i = 0; i < 4; ++i)
        mpBoundPt[i] = theImage.mpBoundPt[i];

    mImageSize = theImage.mImageSize;
    mLoadRect = theImage.mLoadRect;

    for (int i = 0; i < 3; ++i)
        mpEqualizeFunc[i] = theImage.mpEqualizeFunc[i];

    mnRedBandIdx = theImage.mnRedBandIdx;
    mnGreenBandIdx = theImage.mnGreenBandIdx;
    mnBlueBandIdx = theImage.mnBlueBandIdx;
    mnBands = theImage.mnBands;

    mnDataType = theImage.mnDataType;

    for (int i = 0; i < 3; ++i)
        if (theImage.mpBandData[i].get())
        {
            mpBandData[i].reset(new uchar[mImageSize.width() * mImageSize.height()]);

            memcpy_s(mpBandData[i].get(),
                     mImageSize.width() * mImageSize.height(),
                     theImage.mpBandData[i].get(),
                     theImage.mImageSize.width() * theImage.mImageSize.height());
        }

    if (theImage.mpImageData.get())
    {
        mpImageData.reset(new uchar[mImageSize.width()*mImageSize.height() * 3]);

        memcpy_s(mpImageData.get(),
                 mImageSize.width() * mImageSize.height(),
                 theImage.mpImageData.get(),
                 theImage.mImageSize.width() * theImage.mImageSize.height());
    }
}

GDALDataset* SImage::_getOpenDataSet(const QString& imagePath, GDALAccess access)
{
    if (imagePath.isEmpty())
        return nullptr;
    QByteArray strImagePath_byte = imagePath.toUtf8();
    return static_cast<GDALDataset*>(GDALOpen(strImagePath_byte.constData(), access));
}

void SImage::_reloadChannel(int channel, int newBandIdx, std::shared_ptr<void> pHistEqFunc)
{
    Q_ASSERT(channel == 0 || channel == 1 || channel == 2);
    Q_ASSERT(newBandIdx >= 1 && newBandIdx <= mnBands);

    int nPixels = mImageSize.width() * mImageSize.height();

    mpBandData[channel] = loadBand(mLoadRect.left(), mLoadRect.top(), mLoadRect.width(), mLoadRect.height(),
                                   mImageSize.width(), mImageSize.height(),
                                   mStrImagePath,
                                   newBandIdx,
                                   mnDataType);

    //计算均衡化函数
    if (pHistEqFunc)
        this->mpEqualizeFunc[channel] = pHistEqFunc;
    else
        this->mpEqualizeFunc[channel] = SImage::calcHistEqFunc(mnDataType, mpBandData[channel].get(), nPixels);

    //均衡化
    SImage::histEqualize(mnDataType, mpBandData[channel].get(), nPixels, this->mpEqualizeFunc[channel].get());

    //拉伸
    std::unique_ptr<uchar[], std::default_delete<uchar[]>> pBand8bit = to8bit(mnDataType, mpBandData[channel].get(), nPixels);

    //替换通道值
    uchar* pm = mpImageData.get() + channel;
    uchar* pb = pBand8bit.get();
    uchar* pbEnd = pb + nPixels;
    for (; pb != pbEnd; pm += 3, ++pb)
        * pm = *pb;

    //更新图像
    _updateImage();
}

void SImage::_updateImageRect()
{
    this->mImageRect.setRect(-mpImage->width() / 2.0,
                             -mpImage->height() / 2.0,
                             mpImage->width(),
                             mpImage->height());
}

void SImage::_updateImage()
{
    mpImage = new QPixmap();
    *mpImage = QPixmap::fromImage(QImage(mpImageData.get(),
                                         mImageSize.width(), mImageSize.height(),
                                         mImageSize.width() * 3,
                                         QImage::Format_RGB888,
    [](void* pData) {delete[]static_cast<uchar*>(pData);}));
}

std::shared_ptr<void> SImage::calcHistEqFunc(GDALDataType type, const uchar* pBandData, size_t count)
{
    std::shared_ptr<void> pFunc{nullptr};
    switch (type)
    {
    case GDT_Byte:
        pFunc = getEqualizeFunction<UINT8>(static_cast<const UINT8*>(pBandData), count);
        break;
    case GDT_UInt16:
        pFunc = getEqualizeFunction<UINT16>(static_cast<const UINT16*>((const void*)pBandData), count);
        break;
    case GDT_UInt32:
        pFunc = getEqualizeFunction<UINT32>(static_cast<const UINT32*>((const void*)pBandData), count);
        break;
    default:
        Q_ASSERT(0);
        break;
    }
    return pFunc;
}

void SImage::histEqualize(GDALDataType type, uchar* pBandData, size_t count, const void* pEqFunc)
{
    switch (type)
    {
    case GDT_Byte:
        equalizeHist<UINT8>(static_cast<UINT8*>((void*)pBandData), count, static_cast<const UINT8*>(pEqFunc));
        break;
    case GDT_UInt16:
        equalizeHist<UINT16>(static_cast<UINT16*>((void*)pBandData), count, static_cast<const UINT16*>(pEqFunc));
        break;
    case GDT_UInt32:
        equalizeHist<UINT32>(static_cast<UINT32*>((void*)pBandData), count, static_cast<const UINT32*>(pEqFunc));
        break;
    default:
        Q_ASSERT(0);
        break;
    }

}

template<typename T>
std::unique_ptr<uchar[], std::default_delete<uchar[]>> _to8bit(const T* pBandData, size_t count, T divisor)
{
    std::unique_ptr<uchar[], std::default_delete<uchar[]>> p8bitData(new uchar[count]);
    uchar* pData = p8bitData.get();
    for (size_t i = 0; i < count; ++i)
        pData[i] = pBandData[i] / divisor;
    return p8bitData;
}

#define DIV_FACTOR_16TO8BIT 257
#define DIV_FACTOR_32TO8BIT 16843009
std::unique_ptr<uchar[], std::default_delete<uchar[]>> SImage::to8bit(GDALDataType type, const uchar* pBandData, size_t count)
{
    std::unique_ptr<uchar[], std::default_delete<uchar[]>> p8bitData{nullptr};

    switch (type)
    {
    case GDT_Byte:
    {
        p8bitData.reset(new uchar[count]);
        memcpy_s(p8bitData.get(), count * sizeof(uchar), pBandData, count * sizeof(uchar));
        break;
    }
    case GDT_UInt16:
        p8bitData = _to8bit<UINT16>(static_cast<const UINT16*>((const void*)pBandData), count, static_cast<UINT16>(DIV_FACTOR_16TO8BIT));
        break;
    case GDT_UInt32:
        p8bitData = _to8bit<UINT32>(static_cast<const UINT32*>((const void*)pBandData), count, static_cast<UINT32>(DIV_FACTOR_32TO8BIT));
        break;
    default:
        Q_ASSERT(0);
        break;
    }

    return p8bitData;
}

SImageMeta SImage::getMetaOf(QString imagePath)
{
    if (imagePath.isEmpty())
        return SImageMeta{};

    GDALDataset* pDataSet = SImage::_getOpenDataSet(imagePath);
    SImageMeta meta = SImage::getMetaOf(pDataSet);
    GDALClose(pDataSet);
    return meta;
}

SImageMeta SImage::getMetaOf(GDALDataset* pDataSet)
{
    Q_ASSERT(pDataSet != nullptr);
    //获取数据集信息
    int nWidth = pDataSet->GetRasterXSize();
    int nHeight = pDataSet->GetRasterYSize();
    int nBandCount = pDataSet->GetRasterCount();
    double dGeoFeoTransform[6] = {0};
    pDataSet->GetGeoTransform(dGeoFeoTransform);
    GDALDataType nType = pDataSet->GetRasterBand(1)->GetRasterDataType();

    //加上参数pDataSet->GetProjectionRef()
    return SImageMeta(nWidth, nHeight, nBandCount, dGeoFeoTransform, nType, pDataSet->GetProjectionRef());
}
