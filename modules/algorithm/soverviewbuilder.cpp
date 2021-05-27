#include "soverviewbuilder.h"
#include <qthread.h>
#include <QFileInfo>
#include <QDir>

SOverviewBuilder::SOverviewBuilder(QObject *parent) : QObject(parent)
{

}

void SOverviewBuilder::buildOverviews(QString imagePath, QString savePath, SOverviewBuilder::Format format)
{
    //-----获取文件元数据-----//
    QFileInfo fileInfo(imagePath);
    //只接受TIFF影像
    Q_ASSERT(fileInfo.suffix() == "tiff" || fileInfo.suffix() == "tif");
    //打开文件

    switch (format)
    {
    case TIFF:
    {
        _build_overviews_tiff(imagePath, savePath);

        break;
    }
    case JPEG:
    {
        break;
    }
    default:
        Q_ASSERT(0);
    }
}

void SOverviewBuilder::_build_overviews_tiff(const QString &imagePath, const QString &savePath)
{
    this->mStrImagePath = imagePath;
    this->mStrSavePath = savePath;
    //-----创建路径-----//
    QDir dir(savePath);
    Q_ASSERT(dir.exists());

    QFileInfo fileInfo(imagePath);
    QString oriImageName = fileInfo.baseName();

    if(dir.exists(oriImageName))
    {
        QDir overviewDir(dir);
        overviewDir.cd(oriImageName);
        bool success = overviewDir.removeRecursively();
        Q_ASSERT(success);
        SLogger::getLogger()->addEntry(Me, SLogger::LocalError, "Overview folder already exists. Cannot overwrite.");
    }

    dir.mkdir(oriImageName);
    dir.cd(oriImageName);

    //-----获取原始图像元数据-----//
    QByteArray input_path = imagePath.toUtf8();
    GDALDataset *pInDataSet = static_cast<GDALDataset *>(GDALOpen(input_path.constData(), GA_ReadOnly));

    SImageMeta meta = SImage::getMetaOf(pInDataSet);
    int levelCount = SOverviewBuilder::calcPyramidLevelCount(meta);

    //写入金字塔元数据
    QFile metaFile(dir.path() + '/' + oriImageName + "_Meta.txt");
    metaFile.open(QFile::WriteOnly);
    metaFile.write(_generateMetaString(imagePath, savePath, levelCount).toUtf8());

    //------构建金字塔-----//
    for(int i = 1; i < levelCount; ++i)
    {
        //计算降采样宽高
        int nResampledWidth = meta.width() / pow(2, i);
        int nResampledHeight = meta.height() / pow(2, i);

        //尝试分配内存
        uchar *pResampledData{nullptr};
        int nRetryTimes{100};   //重试次数
        int nRetryFreq{200};    //重试时间间隔
        while(--nRetryTimes)
        {
            try
            {
                pResampledData = new uchar[nResampledWidth * nResampledHeight];
            }
            catch (std::bad_alloc)
            {
                QThread::msleep(nRetryFreq);
            }

            if(pResampledData)
                break;
        }

        //采样读入
        pInDataSet->RasterIO(GF_Read,
                             0, 0, meta.width(), meta.height(),
                             pResampledData,
                             nResampledWidth, nResampledHeight,
                             meta.dataType(),
                             meta.bandCount(), nullptr,
                             0, 0, 0);

        //输出数据
        GDALDriver *pDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
        QByteArray outPath = (dir.path() + '/' + oriImageName + "_" + QString::number(i)).toUtf8();
        GDALDataset *pOutDataSet = pDriver->Create(
                                       outPath.constData(),
                                       nResampledWidth, nResampledHeight,
                                       meta.bandCount(),
                                       meta.dataType(),
                                       nullptr);
        pOutDataSet->RasterIO(GF_Write,
                              0, 0, nResampledWidth, nResampledHeight,
                              pResampledData,
                              nResampledWidth, nResampledHeight,
                              meta.dataType(),
                              meta.bandCount(), nullptr,
                              0, 0, 0);

        //更新进度条
        QString infoStr = tr("Building Overviews : ") + QString::number(i) + '/' + QString::number(levelCount);
        emit progressUpdated(static_cast<double>(i) / levelCount * 100.0, infoStr);
    }
    emit overviewsBuilt(savePath);
}

int SOverviewBuilder::calcPyramidLevelCount(const SImageMeta &meta)
{
    int maxWH = meta.width() > meta.height() ? meta.width() : meta.height();

    int levelCount{0};
    while(maxWH > TOP_PYRAMID_SIZE)
        ++levelCount, maxWH = maxWH / 2;
    return levelCount + 1;
}

QString SOverviewBuilder::_generateMetaString(const QString &oriImgPath, const QString &savePath, int levelCount)
{
    QFileInfo fileInfo(oriImgPath);
    QString oriImgName = fileInfo.baseName();

    QString metaString;
    QTextStream stream(&metaString);

    stream << "# SGIS Image Pyramid Meta File\n";
    stream << VER_STR;

    stream << "LEVEL_COUNT\t" << levelCount;

    stream << oriImgPath << '\n';

    for(int i = 1; i < levelCount; ++i)
        stream << savePath + '/' + oriImgName + '/' + oriImgName + "_" + QString::number(i) + '\n';

    return metaString;
}














