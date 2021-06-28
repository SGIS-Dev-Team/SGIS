#include "simage.h"
#include <QFileInfo>
#include <modules/algorithm/sequalizehist.h>
#include "gdal.h"
#include "gdal_priv.h"
#include "gdal_alg.h"
SImage::SImage(const QString &_imagePath, bool _selected, QPointF center, const QString &_layerName, const QString &_layerDiscription, const QColor &_layerColor)
    : SObject(PaintObject::ImageBase, _selected, center, _layerName, _layerDiscription, _layerColor)
{
    mStrImagePath = _imagePath;
    for(auto &ptr : mpBandData)ptr = nullptr;
    for(auto &ptr : mpEqualizeFunc)ptr = nullptr;
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

void SImage::paint(QPainter &painter, bool doTranslate, const QRectF & viewLogicalArea, double scaleValue, PaintTrigger trigger)const
{
    Q_UNUSED(viewLogicalArea);
    Q_UNUSED(scaleValue);
    Q_UNUSED(trigger);

    if(!mpImage)
        return;

    QTransform oldTransform = painter.transform();

    //平移到中心点
    if(doTranslate)
        painter.translate(mPtCenter);

    //执行变换
    painter.setTransform(mTransform * painter.transform());
    //绘图
    const_cast<SImage*>(this)->mMutex.lock();
    painter.drawImage(mImageRect, *mpImage, mpImage->rect());
    const_cast<SImage*>(this)->mMutex.unlock();

    painter.setTransform(oldTransform);
}

const QImage &SImage::getImage() const
{
    return *mpImage;
}

const QString &SImage::getImagePath() const
{
    return mStrImagePath;
}

void SImage::setImagePath(const QString &imagePath)
{
    mStrImagePath = imagePath;
}

void SImage::setImage(const QImage &image)
{
    if(!image.isNull())
        mpImage = new QImage(image);
    mStrImagePath = nullptr;
}

void SImage::releaseImage()
{

    if(mpImage)
        delete mpImage;
    mpImage = nullptr;
}

bool SImage::isNull()const
{
    return mpImage == nullptr;
}

QPolygonF SImage::boundingRect() const
{
    return QPolygonF(QVector<QPointF>(
    {mpBoundPt[0], mpBoundPt[1], mpBoundPt[2], mpBoundPt[3]} ))
    .translated(mPtCenter);
}

bool SImage::contains(const QPointF &pt, bool isInBoundRect) const
{
    Q_UNUSED(isInBoundRect);
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

const QRect &SImage::getLoadRegionRect() const
{
    return mLoadRect;
}

const QSize &SImage::getLoadResampledSize() const
{
    return mImageSize;
}

void SImage::getHistEqFunc(std::shared_ptr<void> pEqFunc[])
{
    for(int i = 0; i < 3; ++i)
        pEqFunc[i] = mpEqualizeFunc[i];
}

void SImage::setHistEqFunc(std::shared_ptr<void> pEqFunc[])
{
    for(int i = 0; i < 3; ++i)
        mpEqualizeFunc[i] = pEqFunc[i];
}

void SImage::setLoadRegionRect(const QRect &rect)
{
    mLoadRect = rect;
}

void SImage::setLoadRegionResampledSize(const QSize &size)
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
	GDALRasterBand* poBand1 = poDataset->GetRasterBand(1);
	auto nX = poBand1->GetXSize();
	auto nY = poBand1->GetYSize();
	auto nBand = poBand1->GetBand();
	obj[QString::fromWCharArray(L"图像波段")]
		= QString("XSize:%0,YSize:%1,Band:%2").arg(nX).arg(nY).arg(nBand);
	//获取图像的尺寸 
	int nImgSizeX = poDataset->GetRasterXSize();
	int nImgSizeY = poDataset->GetRasterYSize();
	obj[QString::fromWCharArray(L"图像尺寸")]
		= QString("XSize:%0,YSize:%1").arg(nImgSizeX).arg(nImgSizeY);
	//获取坐标变换系数 
	double trans[6];
	CPLErr aaa = poDataset->GetGeoTransform(trans);
	obj[QString::fromWCharArray(L"坐标变换系数")]
		= QString("%0,%1,%2,%3,%4,%5")
		.arg(trans[0])
		.arg(trans[1])
		.arg(trans[2])
		.arg(trans[3])
		.arg(trans[4])
		.arg(trans[5]);
	//读取图像高程数据 
	QStringList lstValues;
	double Xgeo, Ygeo;
	auto pafScanblock1 = (unsigned char*)CPLMalloc(sizeof(char) * (nImgSizeX) * (nImgSizeY));
	poBand1->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY, pafScanblock1, nImgSizeX, nImgSizeY, GDALDataType(poBand1->GetRasterDataType()), 0, 0);
	for (int i = 0; i < (nImgSizeX - 5990); i++)
	{
		for (int j = 0; j < (nImgSizeY - 5990); j++)
		{
			auto elevation = *pafScanblock1;
			Xgeo = trans[0] + i * trans[1] + j * trans[2];
			Ygeo = trans[3] + i * trans[4] + j * trans[5];
			pafScanblock1++;

			lstValues<<QString("%0,%1,%2").arg(Xgeo).arg(Ygeo).arg(elevation);
		}
	}
	obj[QString::fromWCharArray(L"高程数据")] = lstValues.join("|");
	return obj;
}

std::unique_ptr<uchar> SImage::loadBand(int x_off, int y_off, int x_span, int y_span,
                                        int image_width, int image_height,
                                        const QString &imagePath, int bandIdx, GDALDataType dataType,
                                        std::shared_ptr<void> pEqFunc)
{
    GDALDataset *pDataSet = SImage::_getOpenDataSet(imagePath);
    std::unique_ptr<uchar> pBandData = loadBand(x_off, y_off, x_span, y_span,
                                       image_width, image_height,
                                       pDataSet,
                                       bandIdx,
                                       dataType,
                                       pEqFunc);
    GDALClose(pDataSet);
    return pBandData;
}

std::unique_ptr<uchar> SImage::loadBand(int x_off, int y_off, int x_span, int y_span,
                                        int image_width, int image_height,
                                        GDALDataset *pDataSet, int bandIdx, GDALDataType dataType,
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
	//todostd::unique_ptr<uchar> pBandData(new uchar[nPixels * GDALGetDataTypeSizeBytes(dataType)]{});
	std::unique_ptr<uchar> pBandData = nullptr;

    pDataSet->GetRasterBand(bandIdx)->RasterIO(
        GF_Read,
        x_off, y_off, x_span, y_span,
        pBandData.get(),
        image_width, image_height,
        dataType,
        0, 0, 0);

    if(pEqFunc)
        SImage::histEqualize(dataType, pBandData.get(), nPixels, pEqFunc.get());

    return pBandData;
}

std::unique_ptr<uchar> SImage::merge(const uchar *pBandData[], GDALDataType dataType, int pixelCount, int bandCount)
{
    //分配内存
    std::unique_ptr<uchar> pMergedData(new uchar [bandCount * pixelCount * sizeof(uchar)]);

    //合成逐像素存储图像数据
    for(int i = 0; i < bandCount; ++i)
    {
        uchar *pm = pMergedData.get() + i;
        std::unique_ptr<uchar> pBand8Bit = to8bit(dataType, pBandData[i], pixelCount);
        uchar *pb = pBand8Bit.get();
        uchar *pbEnd = pb + pixelCount;
        for(; pb != pbEnd; pm += bandCount, ++pb)
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
    if(reload)
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

void SImage::setBandIndices(int r, int g, int b, bool reload)
{
    Q_ASSERT(r > 0 && g > 0 && b > 0);
    setRedBandIdx(r, reload, nullptr);
    setGreenBandIdx(g, reload, nullptr);
    setBlueBandIdx(b, reload, nullptr);
}

void SImage::setBandIndices(int r, int g, int b, bool reload, std::shared_ptr<void> pHistEqFuncs[])
{
    Q_ASSERT(r > 0 && g > 0 && b > 0);
    setRedBandIdx(r, reload, pHistEqFuncs[0]);
    setGreenBandIdx(g, reload, pHistEqFuncs[1]);
    setBlueBandIdx(b, reload, pHistEqFuncs[2]);
}

void SImage::setDefaultedBands(bool reload)
{
    setBandIndices(1, 1, 1, reload);
}

void SImage::setGreenBandIdx(int value, bool reload, std::shared_ptr<void> pHistEqFunc)
{
    mnGreenBandIdx = value;
    if(reload)
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
    if(reload)
        _reloadChannel(0, value, pHistEqFunc);
    else
        mpEqualizeFunc[0].reset();
}

void SImage::load(const QString &imagePath)
{
	load(mLoadRect, imagePath);
}

void SImage::load(int x_off, int y_off, int x_span, int y_span, const QString &imagePath)
{
    Q_ASSERT(x_off >= 0 && y_off >= 0 && x_span >= 0 && y_span >= 0);
    load(x_off, y_off, x_span, y_span,
         mImageSize.width(), mImageSize.height(),
         imagePath);
}

void SImage::load(const QRect &rect, const QString &imagePath)
{
    load(rect.left(), rect.top(), rect.width(), rect.height(), imagePath);
}

void SImage::load(int x_off, int y_off, int x_span, int y_span, int image_width, int image_height, const QString &imagePath)
{
    if(!imagePath.isEmpty())
        mStrImagePath = imagePath;
    if(mStrImagePath.isEmpty())
        return;

    if(image_width == 0 || image_height == 0)
        image_width = x_span, image_height = y_span;

    GDALDataset *pDataSet = SImage::_getOpenDataSet(mStrImagePath);
    load(x_off, y_off, x_span, y_span, image_width, image_height, pDataSet);
    GDALClose(pDataSet);
}

void SImage::load(int x_off, int y_off, int x_span, int y_span, int image_width, int image_height, GDALDataset *pDataSet)
{
    releaseImage();
    //------加载图片------//

    //判断文件后缀是否为*.tif或*.tiff
    QFileInfo imgInfo(mStrImagePath);
    //TODO: 记得对jpg存储的金字塔单独处理
    if(imgInfo.suffix() != "tiff" && imgInfo.suffix() != "tif")
        mpImage = new QImage(mStrImagePath);
    else
    {
        //获取描述数据
        SImageMeta meta = getMetaOf(pDataSet);
        mnBands = meta.bandCount();
        int nPixels = image_width * image_height;
        mnDataType = meta.dataType();

        //读入选中的各波段数据
        int pBandIdx[3] = {mnRedBandIdx, mnGreenBandIdx, mnBlueBandIdx};
        for(int i = 0; i < 3; ++i)
        {
            mpBandData[i] = this->loadBand(x_off, y_off, x_span, y_span,
                                           image_width, image_height,
                                           pDataSet,
                                           pBandIdx[i],
                                           mnDataType);
            //直方图均衡化
            if(!mpEqualizeFunc[i])
                mpEqualizeFunc[i] = SImage::calcHistEqFunc(mnDataType, mpBandData[i].get(), nPixels);
            SImage::histEqualize(mnDataType, mpBandData[i].get(), nPixels, mpEqualizeFunc[i].get());
        }

        //融合图像，并创建QImage用于显示
        uchar *pBandData[3] = {mpBandData[0].get(), mpBandData[1].get(), mpBandData[2].get()};
        mpImageData = SImage::merge(const_cast<const uchar**>(pBandData), mnDataType, nPixels, 3);
        mpImage = new QImage(mpImageData.get(),
                             image_width, image_height,
                             image_width * 3,
                             QImage::Format_RGB888);
    }
    _updateImageRect();

    //应用变换
    _applyTransform();

    //更新读取图像区域
    mLoadRect.setRect(x_off, y_off, x_span, y_span);
    mImageSize.setWidth(image_width);
    mImageSize.setHeight(image_height);
}

void SImage::load(const QRect &rect, int down_sampling_ratio, const QString &imagePath)
{
    //计算降采样后的图像大小
    SImageMeta meta = SImage::getMetaOf(imagePath);
    int w = rect.width();
    int h = rect.height();
    if(w == 0)w = meta.width();
    if(h == 0)h = meta.height();

    load(rect.left(), rect.top(), w, h,
         w / down_sampling_ratio, h / down_sampling_ratio,
         imagePath);
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

    mImageSize = theImage.mImageSize;
    mLoadRect = theImage.mLoadRect;

    for(int i = 0; i < 3; ++i)
        mpEqualizeFunc[i] = theImage.mpEqualizeFunc[i];

    mnRedBandIdx = theImage.mnRedBandIdx;
    mnGreenBandIdx = theImage.mnGreenBandIdx;
    mnBlueBandIdx = theImage.mnBlueBandIdx;
    mnBands = theImage.mnBands;

    mnDataType = theImage.mnDataType;

    for(int i = 0; i < 3; ++i)
        if(theImage.mpBandData[i].get())
        {
            mpBandData[i].reset(new uchar[mImageSize.width()*mImageSize.height()]);

            memcpy_s(mpBandData[i].get(),
                     mImageSize.width() * mImageSize.height(),
                     theImage.mpBandData[i].get(),
                     theImage.mImageSize.width() * theImage.mImageSize.height());
        }

    if(theImage.mpImageData.get())
    {
        mpImageData.reset(new uchar[mImageSize.width()*mImageSize.height() * 3]);

        memcpy_s(mpImageData.get(),
                 mImageSize.width() * mImageSize.height(),
                 theImage.mpImageData.get(),
                 theImage.mImageSize.width() * theImage.mImageSize.height());
    }
}

GDALDataset *SImage::_getOpenDataSet(const QString &imagePath, GDALAccess access)
{
    if(imagePath.isEmpty())return nullptr;
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
    if(pHistEqFunc)
        this->mpEqualizeFunc[channel] = pHistEqFunc;
    else
        this->mpEqualizeFunc[channel] = SImage::calcHistEqFunc(mnDataType, mpBandData[channel].get(), nPixels);

    //均衡化
    SImage::histEqualize(mnDataType, mpBandData[channel].get(), nPixels, this->mpEqualizeFunc[channel].get());

    //拉伸
    std::unique_ptr<uchar> pBand8bit = to8bit(mnDataType, mpBandData[channel].get(), nPixels);

    //替换通道值
    uchar *pm = mpImageData.get() + channel;
    uchar *pb = pBand8bit.get();
    uchar *pbEnd = pb + nPixels;
    for(; pb != pbEnd; pm += 3, ++pb)
        * pm = *pb;
}

void SImage::_updateImageRect()
{
    this->mImageRect.setRect(-mpImage->width() / 2.0,
                             -mpImage->height() / 2.0,
                             mpImage->width(),
                             mpImage->height());
}

std::shared_ptr<void> SImage::calcHistEqFunc(GDALDataType type, const uchar *pBandData, size_t count)
{
    std::shared_ptr<void> pFunc{nullptr};
    switch (type)
    {
    case GDT_Byte:
        pFunc.reset(getEqualizeFunction<UINT8>(static_cast<const UINT8*>((const void*)pBandData), count));
        break;
    case GDT_UInt16:
        pFunc.reset(getEqualizeFunction<UINT16>(static_cast<const UINT16*>((const void*)pBandData), count));
        break;
    case GDT_UInt32:
        pFunc.reset(getEqualizeFunction<UINT32>(static_cast<const UINT32*>((const void*)pBandData), count));
        break;
    default:
        Q_ASSERT(0);
        break;
    }
    return pFunc;
}

void SImage::histEqualize(GDALDataType type, uchar *pBandData, size_t count, const void *pEqFunc)
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
std::unique_ptr<uchar> _to8bit(const T* pBandData, size_t count, T divisor)
{
    std::unique_ptr<uchar> p8bitData(new uchar[count]);
    uchar* pData = p8bitData.get();
    for(size_t i = 0; i < count; ++i)
        pData[i] = pBandData[i] / divisor;
    return p8bitData;
}

#define DIV_FACTOR_16TO8BIT 257
#define DIV_FACTOR_32TO8BIT 16843009
std::unique_ptr<uchar> SImage::to8bit(GDALDataType type, const uchar *pBandData, size_t count)
{
    std::unique_ptr<uchar> p8bitData{nullptr};

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
    if(imagePath.isEmpty())
        return SImageMeta{};

    GDALDataset *pDataSet = SImage::_getOpenDataSet(imagePath);
    SImageMeta meta = SImage::getMetaOf(pDataSet);
    GDALClose(pDataSet);
    return meta;
}

SImageMeta SImage::getMetaOf(GDALDataset *pDataSet)
{
    Q_ASSERT(pDataSet != nullptr);
    //获取数据集信息
    int nWidth = pDataSet->GetRasterXSize();
    int nHeight = pDataSet->GetRasterYSize();
    int nBandCount = pDataSet->GetRasterCount();
    GDALDataType nType = pDataSet->GetRasterBand(1)->GetRasterDataType();

    return SImageMeta(nWidth, nHeight, nBandCount, nType);
}
