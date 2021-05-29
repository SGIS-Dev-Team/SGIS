#include "soverviewbuilder.h"
#include <qthread.h>
#include <QFileInfo>
#include <QDir>

SOverviewBuilder::SOverviewBuilder(QObject *parent) : QObject(parent)
{

}

void SOverviewBuilder::buildOverviews(QString imagePath, QString savePath, SOverviewBuilder::Format format)
{
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
    QDir dir;
    if(dir.exists(savePath))
        SLogger::getLogger()->addEntry(Me, SLogger::RunningStatus, "Overview dir exists.");
    else
        dir.mkdir(savePath);
    dir.cd(savePath);

    QFileInfo fileInfo(imagePath);
    QString oriImageName = fileInfo.completeBaseName();

    emit progressUpdated(1, "Builder initialization complete.");

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
    SImageMeta meta = SImage::getMetaOf(imagePath);
    int levelCount = SOverviewBuilder::calcPyramidLevelCount(meta);

    //写入金字塔元数据
    QFile metaFile(dir.path() + '/' + oriImageName + "_Meta.txt");
    metaFile.open(QFile::WriteOnly);
    metaFile.write(generateMetaString(imagePath, QSize(meta.width(), meta.height()), dir.path(), levelCount).toUtf8());
    metaFile.close();

    //获取写入驱动
    GDALDriver *pDriver = GetGDALDriverManager()->GetDriverByName("GTiff");

    //------构建金字塔-----//
    QByteArray lastOutImagePath = imagePath.toUtf8();
    for(int i = 1; i < levelCount; ++i)
    {
        //更新进度条
        QString infoStr = tr("Building Overviews : ") + QString::number(i - 1) + '/' + QString::number(levelCount);
        emit progressUpdated(static_cast<double>(i - 1) / levelCount * 100.0, infoStr);

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
                pResampledData = new uchar[nResampledWidth * nResampledHeight * GDALGetDataTypeSizeBytes(meta.dataType()) * meta.bandCount()];
            }
            catch (std::bad_alloc)
            {
                QThread::msleep(nRetryFreq);
            }

            if(pResampledData)
                break;
        }
        if(nRetryTimes == 0)
            Q_ASSERT(0);

        //采样读入
        GDALDataset *pInDataSet = static_cast<GDALDataset*>(GDALOpen(lastOutImagePath.constData(), GA_ReadOnly));
        pInDataSet->RasterIO(GF_Read,
                             0, 0, pInDataSet->GetRasterXSize(), pInDataSet->GetRasterYSize(),
                             pResampledData,
                             nResampledWidth, nResampledHeight,
                             meta.dataType(),
                             meta.bandCount(), nullptr,
                             0, 0, 0);
        GDALClose(pInDataSet);

        //输出数据
        QString outPath = dir.path() + '/' + oriImageName + "_" + QString::number(i) + ".tif";
        QByteArray outPath_byte(outPath.toUtf8());

        GDALDataset *pOutDataSet = pDriver->Create(
                                       outPath_byte.constData(),
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

        GDALClose(pOutDataSet);
        delete []pResampledData;
        lastOutImagePath = outPath_byte;
    }
    emit progressUpdated(100, tr("Overviews built."));
    emit overviewsBuilt(dir.path());
}

int SOverviewBuilder::calcPyramidLevelCount(const SImageMeta &meta)
{
    int maxWH = meta.width() > meta.height() ? meta.width() : meta.height();

    int levelCount{0};
    while(maxWH > DEFAULT_LOGICAL_FRAGMENT_SIZE)
        ++levelCount, maxWH = maxWH / 2;
    return levelCount + 1;
}

QString SOverviewBuilder::generateMetaString(const QString &oriImgPath, const QSize &oriImgSize, const QString &pyramidDirPath, int levelCount)
{
    QFileInfo fileInfo(oriImgPath);
    QString oriImgName = fileInfo.completeBaseName();

    QString metaString;
    QTextStream stream(&metaString);

    stream << "# SGIS Image Pyramid Meta File\n";
    stream << VER_STR;

    stream << "LEVEL_COUNT\t" << levelCount << '\n';

    stream << "BASE_WIDTH\t" << oriImgSize.width() << '\n';
    stream << "BASE_HEIGHT\t" << oriImgSize.height() << '\n';

    stream << oriImgPath << '\n';

    for(int i = 1; i < levelCount; ++i)
        stream << pyramidDirPath + '/' + oriImgName + "_" + QString::number(i) + ".tif\n";

    return metaString;
}

bool SOverviewBuilder::varifyPyramid(const QString &oriImgPath, const QString &pyramidDirPath)
{
    //找到元数据文件
    QString oriImgName = QFileInfo(oriImgPath).completeBaseName();

    QString metaPath = pyramidDirPath + '/' + oriImgName + "_Meta.txt";

    //检查元数据文件是否存在
    QFile metaFile(metaPath);
    if(!metaFile.open(QFile::ReadOnly))
        return false;

    //比对元数据字符串是否一致
    SImageMeta meta = SImage::getMetaOf(oriImgPath);
    int levelCount = calcPyramidLevelCount(meta);

    QString metaString = generateMetaString(oriImgPath, QSize(meta.width(), meta.height()), pyramidDirPath, levelCount);
    if(metaFile.readAll() != metaString)
    {
        metaFile.close();
        return false;
    }

    //处理字符串获得图像文件路径并比对
    QString buf;
    QTextStream stream(&metaString);
    for(int i = 0; i < 5; ++i)  //当前元数据文件有2行注释和3行前置信息
        stream.readLine();

    for(int i = 0; i < levelCount; ++i)
    {
        QString pyramidImagePath;
        stream >> pyramidImagePath;
        if(!QFile(pyramidDirPath).exists())
            return false;
    }

    return true;
}














