#include "simagestreammeta.h"
#include "modules/algorithm/soverviewbuilder.h"
#include <QApplication>

void SImageStreamMeta::loadPreviewImage(int r, int g, int b)
{
    _loadPreviewImage(r, g, b);
    moveBackToMainThread();
    emit previewImageLoaded();
}

void SImageStreamMeta::moveBackToMainThread()
{
    this->moveToThread(QApplication::instance()->thread());
}

void SImageStreamMeta::setOriginalImagePath(const QString &path)
{
    clear();
    mStrOriImagePath = path;
}

const SImageMeta &SImageStreamMeta::imageMeta() const
{
    const_cast<SImageStreamMeta*>(this)->_loadImageMeta();
    return mImageMeta;
}

void SImageStreamMeta::setBuildingProgress(int value)
{
    mnBuildingProgress = value;
    if(mnBuildingProgress == 100)
        emit overviewsBuilt(mStrPyramidDir);
}

void SImageStreamMeta::_loadImageMeta()
{
    if(!mImageMeta.isEmpty() || mStrOriImagePath.isEmpty())
        return;
    mImageMeta = SImage::getMetaOf(mStrOriImagePath);
}

void SImageStreamMeta::_loadPreviewImage(int r, int g, int b)
{
    if(mStrOriImagePath.isEmpty())
        return;

    _loadImageMeta();

    Q_ASSERT(r >= 0 && g >= 0 && b >= 0 && r <= mImageMeta.bandCount() && g <= mImageMeta.bandCount() && b <= mImageMeta.bandCount());

    //计算降采样倍率
    double resamplingRatio = 1.0 / pow(2, SOverviewBuilder::calcPyramidLevelCount(mImageMeta) - 1);

    //设置波段
    if(r == 0 || g == 0 || b == 0)
    {
        if(mImageMeta.bandCount() < 3)
            mPreviewImage.presetBandIndices(1, 1, 1);
        else
            mPreviewImage.presetBandIndices(1, 2, 3);
    }
    else
        mPreviewImage.presetBandIndices(r, g, b);

    //加载缩略图
    mPreviewImage.load(0, 0, mImageMeta.width(), mImageMeta.height(), resamplingRatio, mStrOriImagePath);
}

void SImageStreamMeta::_initializeWith(const SImageStreamMeta &smeta)
{
    this->mImageMeta = smeta.mImageMeta;

}

bool SImageStreamMeta::checkPyramidExistence()
{
    if(mStrOriImagePath.isEmpty() || mStrPyramidDir.isEmpty())
        Q_ASSERT(0);
    return SOverviewBuilder::varifyPyramid(mStrOriImagePath, mStrPyramidDir);
}

void SImageStreamMeta::getBandIdices(int pBandIndices[]) const
{
    pBandIndices[0] = getRedBandIndex();
    pBandIndices[1] = getGreenBandIndex();
    pBandIndices[2] = getBlueBandIndex();
}

void SImageStreamMeta::getHistEqFunc(std::shared_ptr<void> pHistEqFunc[]) const
{
    if(mPreviewImage.isNull())
        const_cast<SImageStreamMeta*>(this)->_loadPreviewImage();
    mPreviewImage.getHistEqFunc(pHistEqFunc);
}

void SImageStreamMeta::clear()
{
    mStrOriImagePath.clear();
    mStrPyramidDir.clear();
    mImageMeta.clear();
    mnBuildingProgress = 0;
    mPreviewImage.releaseMemory();
}
