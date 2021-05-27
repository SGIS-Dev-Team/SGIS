#include "sslice.h"
#include <QDebug>
#include <QThread>

SSlice::SSlice()
{
}
SSlice::~SSlice()
{
}

GDALDataset* SSlice::GetDataset(QString qstrInPath)
{
    qDebug() << qstrInPath << " OPENED by SImage on Thread " << QThread::currentThreadId();

    //含中文QString->char*
    QByteArray qbarr = qstrInPath.toUtf8();// 不能两句合起来写
    const char* ch = qbarr.constData();
    return  static_cast<GDALDataset*>(GDALOpen(ch, GA_Update));  // 打开给定的数据文件
}
GDALDataset* SSlice::CreateDataset(GDALDriver* pDriver, QString qstrInPath, int nXSize, int nYSize, int nBands,
                                   GDALDataType eType, char** papszOptions)
{
    //含中文QString->char*
    QByteArray qbarr = qstrInPath.toUtf8();// 不能两句合起来写
    const char* ch = qbarr.constData();
    return pDriver->Create(ch, nXSize, nYSize, nBands, eType, papszOptions);
}

QString SSlice::getOverviewsSlice(QString qstrInPath)
{
    /*--------准备工作--------*/
    //顶层图像的最大尺寸
    int iSliceMaxSize = 2 * SLICE_SIZE;
    //获取图像路径，设置相应的输出路径
    QFileInfo fileInfo(qstrInPath);
    //注册所有支持的数据格式驱动
    GDALAllRegister();
    //读图像
    GDALDataset* pDataset = GetDataset(qstrInPath);
    //打开失败
    if (!pDataset)
    {
        emit progressUpdated(100, tr("failed to open the image"));
        emit overviewsBuilt("");
        return "";
    }
    //波段数
    int iBandCount = pDataset->GetRasterCount();
    //原图大小X,Y(width,height)
    int iOriX = pDataset->GetRasterXSize();
    int iOriY = pDataset->GetRasterYSize();
    //数据类型和位深度
    GDALDataType gdalType = pDataset->GetRasterBand(1)->GetRasterDataType();
    int iDepth = GDALGetDataTypeSize(gdalType) / 8;

    //金字塔级数
    int iPyCount = 0;
    int iOriMax = MAX(iOriX, iOriY);
    if (iOriMax > iSliceMaxSize)
    {
        iPyCount = ceil(qLn(iOriMax / double(iSliceMaxSize)) / qLn(2)) + 1;
    }
    else
    {
        iPyCount = 1;
    }

    //构建元数据文本文件
    //qstrCompleteBaseName是影像的无后缀文件名
    QString qstrCompleteBaseName = fileInfo.completeBaseName();
    //存放在我的文档sgis文件夹
    QString mStrLogDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/sgis";
    //检查sgif文件夹是否存在
    QDir dir;
    if (!dir.exists(mStrLogDir))
        dir.mkdir(mStrLogDir);
    //输出文件夹的上级目录
    QDir qdirOutFile(mStrLogDir);
    if (!qdirOutFile.exists(qstrCompleteBaseName))
    {
        qdirOutFile.mkdir(qstrCompleteBaseName);
    }
    //这是输出文件夹路径
    QString qstrOutPath = qdirOutFile.filePath(qstrCompleteBaseName);
    //元数据
    QString temp = qstrOutPath + "/" + qstrCompleteBaseName + "_Meta.txt";
    QFile qfMeta(temp);
    qfMeta.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&qfMeta);
    //层数与原图大小
    out << QStringLiteral("PYRAMID_COUNT= %1\n").arg(iPyCount)
        << QStringLiteral("ORIGINAL_WIDTH= %1\n").arg(iOriX)
        << QStringLiteral("ORIGINAL_HEIGHT= %1\n\n").arg(iOriY);
    GDALClose(pDataset);

    /*--------逐层构建---------*/
    emit progressUpdated(0, tr("building the overviews"));
    QDir qdirLevelFile(qstrOutPath);
    //分块大小和数量
    int iFrgW = SLICE_SIZE, iFrgH = SLICE_SIZE;
    int iPreColCount = 1, iPreRowCount = 1; //上一级
    int iCurColCount = 1, iCurRowCount = 1; //本级
    int iXOff = 0, iYOff = 0; //偏移量
    int iX = 0, iY = 0; //暂时存储一个图像的宽高
    int iPreX = 0, iPreY = 0; //记录上一级拼图像的大小
    //相应路径
    QString qstrPreLevel = "";
    QString qstrPreSlice = "";
    QString qstrCurLevel = "";
    QString qstrCurSlice = "";
    //内存分配(根据数据类型要乘上位深度)
    unsigned char* pBuf = new unsigned char[iSliceMaxSize * iSliceMaxSize * iBandCount * iDepth];
    //写图像的相关指针
    GDALDriver* pDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset* pDsWrite = nullptr;

    //当前金字塔级数下的尺寸(x--width--col,y--height--col)
    int iCurX = 0, iCurY = 0, iCurMax;
    for (int i = 1; i <= iPyCount; ++i)
    {
        emit progressUpdated(i / double(iPyCount) * 100, tr("building the overviews") + "(" + QString::number(i) + "/" + QString::number(iPyCount) + ")");

        //计算大小和分块
        iCurMax = iOriMax / pow(2, i - 1);
        iCurX = iOriX / pow(2, i - 1);
        iCurY = iOriY / pow(2, i - 1);
        iFrgW = SLICE_SIZE;
        iFrgH = SLICE_SIZE;
        //达到尺寸要求结束
        if (iCurMax <= iSliceMaxSize)
        {
            iPreColCount = iCurColCount;
            iPreRowCount = iCurRowCount;
            iCurColCount = 1;
            iCurRowCount = 1;
            iFrgW = iCurX;
            iFrgH = iCurY;

            //当前级的文件夹路径
            qstrCurLevel = qdirLevelFile.filePath(QString::number(pow(2, i - 1)));
            //当前级的创建切片文件路径
            qstrCurSlice = qstrCurLevel + "/" + qstrCompleteBaseName + "_1.tif";
            //上一级文件夹
            qstrPreLevel = qdirLevelFile.filePath(QString::number(pow(2, i - 2)));
            //创建当前级的文件夹
            if (!qdirLevelFile.exists(QString::number(pow(2, i - 1))))
            {
                qdirLevelFile.mkdir(QString::number(pow(2, i - 1)));
            }
            pDsWrite = CreateDataset(pDriver, qstrCurSlice, iCurX, iCurY, iBandCount, gdalType, NULL);

            //将上一级切片的全部合入
            //说明不是原图
            if (iPyCount > 1)
            {
                //根据切片的原理要合并3*3=9 ~ 4*4=16张图像
                //默认切片是对齐的，这很重要（只要是这个函数生成的切片，就不会有没对齐的问题）
                iYOff = 0;
                for (int y = 0; y < iPreRowCount; ++y)
                {
                    iXOff = 0;
                    for (int x = 0; x < iPreColCount; ++x)
                    {
                        //TODO:这个地方直接用.tif后缀感觉有些不太保险,以后可以考虑改一下
                        qstrPreSlice = qstrPreLevel + "/" + qstrCompleteBaseName + "_" + QString::number(iPreColCount * y + x + 1) + ".tif";

                        //读图像
                        pDataset = GetDataset(qstrPreSlice);
                        if (!pDataset)
                        {
                            continue;
                        }
                        iX = pDataset->GetRasterXSize();
                        iY = pDataset->GetRasterYSize();
                        pDataset->RasterIO(GF_Read, 0, 0, iX, iY, pBuf, iX, iY, gdalType, iBandCount, NULL, 0, 0, 0);
                        GDALClose(pDataset);

                        pDsWrite->RasterIO(GF_Write, iXOff, iYOff, iX / 2, iY / 2, pBuf, iX, iY, gdalType, iBandCount, NULL, 0, 0, 0);
                        iXOff += iX / 2;
                    }
                    iYOff += iY / 2;
                }
            }
            else //直接复制原图
            {
                //读图像
                pDataset = GetDataset(qstrInPath);
                if (!pDataset)
                {
                    //处理结束
                    GDALClose(pDsWrite);
                    delete[] pBuf;
                    emit progressUpdated(100, tr("failed to open the image"));
                    emit overviewsBuilt("");
                    return "";
                }
                iX = pDataset->GetRasterXSize();
                iY = pDataset->GetRasterYSize();
                pDataset->RasterIO(GF_Read, 0, 0, iX, iY, pBuf, iX, iY, gdalType, iBandCount, NULL, 0, 0, 0);
                GDALClose(pDataset);

                pDsWrite->RasterIO(GF_Write, 0, 0, iCurX, iCurY, pBuf, iX, iY, gdalType, iBandCount, NULL, 0, 0, 0);
            }
            //处理结束
            GDALClose(pDsWrite);

        }
        else if (i == 1)//若是第一级直接分割
        {
            //从原图开始分割
            //读图像
            pDataset = GetDataset(qstrInPath);
            if (!pDataset)
            {
                delete[] pBuf;
                emit progressUpdated(100, tr("failed to open the image"));
                emit overviewsBuilt("");
                return "";
            }
            iCurColCount = ceil(iCurX / double(SLICE_SIZE));
            iCurRowCount = ceil(iCurY / double(SLICE_SIZE));

            int iCount = 1;
            for (int y = 0; y < iCurRowCount; ++y)
            {
                if (y == iCurRowCount - 1)
                {
                    iY = iCurY - SLICE_SIZE * (iCurRowCount - 1);
                }
                else
                {
                    iY = SLICE_SIZE;
                }
                iYOff = y * SLICE_SIZE;
                for (int x = 0; x < iCurColCount; ++x)
                {
                    if (x == iCurColCount - 1)
                    {
                        iX = iCurX - SLICE_SIZE * (iCurColCount - 1);
                    }
                    else
                    {
                        iX = SLICE_SIZE;
                    }
                    iXOff = x * SLICE_SIZE;
                    pDataset->RasterIO(GF_Read, iXOff, iYOff, iX, iY, pBuf, iX, iY, gdalType, iBandCount, NULL, 0, 0, 0);

                    //创建当前级的文件夹
                    if (!qdirLevelFile.exists(QString::number(pow(2, i - 1))))
                    {
                        qdirLevelFile.mkdir(QString::number(pow(2, i - 1)));
                    }
                    //当前级的文件夹路径
                    qstrCurLevel = qdirLevelFile.filePath(QString::number(pow(2, i - 1)));
                    //当前级的创建切片文件路径
                    qstrCurSlice = qstrCurLevel + "/" + qstrCompleteBaseName + "_" + QString::number(iCount) + ".tif";
                    //TODO:进度条以后可能要用
                    pDsWrite = CreateDataset(pDriver, qstrCurSlice, iX, iY, iBandCount, gdalType, NULL);
                    pDsWrite -> RasterIO(GF_Write, 0, 0, iX, iY, pBuf, iX, iY, gdalType, iBandCount, NULL, 0, 0, 0);

                    GDALClose(pDsWrite);
                    ++iCount;
                }
            }
            GDALClose(pDataset);
        }
        else  //其他情况从上一级文件夹中读取
        {
            iPreColCount = iCurColCount;
            iCurColCount = ceil(iCurX / double(SLICE_SIZE));
            iPreRowCount = iCurRowCount;
            iCurRowCount = ceil(iCurY / double(SLICE_SIZE));


            //创建当前级的文件夹
            if (!qdirLevelFile.exists(QString::number(pow(2, i - 1))))
            {
                qdirLevelFile.mkdir(QString::number(pow(2, i - 1)));
            }
            //当前级的文件夹路径
            qstrCurLevel = qdirLevelFile.filePath(QString::number(pow(2, i - 1)));
            //上一级的文件夹路径
            qstrPreLevel = qdirLevelFile.filePath(QString::number(pow(2, i - 2)));

            int iCount = 1;

            for (int y = 0; y < iCurRowCount; ++y)
            {
                if (y == iCurRowCount - 1)
                {
                    iY = iCurY - SLICE_SIZE * (iCurRowCount - 1);
                }
                else
                {
                    iY = SLICE_SIZE;
                }
                for (int x = 0; x < iCurColCount; ++x)
                {
                    if (x == iCurColCount - 1)
                    {
                        iX = iCurX - SLICE_SIZE * (iCurColCount - 1);
                    }
                    else
                    {
                        iX = SLICE_SIZE;
                    }
                    //写图像
                    //当前级的创建切片文件路径
                    qstrCurSlice = qstrCurLevel + "/" + qstrCompleteBaseName + "_" + QString::number(iCount) + ".tif";
                    pDsWrite = CreateDataset(pDriver, qstrCurSlice, iX, iY, iBandCount, gdalType, NULL);

                    iYOff = 0;
                    for (int m = 0, py = 2 * y; py < iPreRowCount && m < 2; ++m, ++py)
                    {
                        iXOff = 0;
                        for (int n = 0, px = 2 * x; px < iPreColCount && n < 2; ++n, ++px)
                        {
                            //TODO:这个地方直接用.tif后缀感觉有些不太保险,以后可以考虑改一下
                            qstrPreSlice = qstrPreLevel + "/" + qstrCompleteBaseName + "_" + QString::number(iPreColCount * py + px + 1) + ".tif";


                            //读图像
                            pDataset = GetDataset(qstrPreSlice);
                            if (!pDataset)
                            {
                                continue;
                            }
                            iPreX = pDataset->GetRasterXSize();
                            iPreY = pDataset->GetRasterYSize();
                            pDataset->RasterIO(GF_Read, 0, 0, iPreX, iPreY, pBuf, iPreX, iPreY, gdalType, iBandCount, NULL, 0, 0, 0);
                            GDALClose(pDataset);

                            pDsWrite -> RasterIO(GF_Write, iXOff, iYOff, iPreX / 2, iPreY / 2, pBuf, iPreX, iPreY, gdalType, iBandCount, NULL, 0, 0, 0);
                            iXOff += iPreX / 2;
                        }
                        iYOff += iPreY / 2;
                    }
                    //写完一张图像
                    GDALClose(pDsWrite);
                    ++iCount;
                }
            }
        }
        //保存元文件数据
        out << QStringLiteral("PYRAMID_LEVEL= %1\n").arg(pow(2, i - 1))
            << QStringLiteral("LEVEL_WIDTH= %1\n").arg(iCurX)
            << QStringLiteral("LEVEL_HEIGHT= %1\n").arg(iCurY)
            << QStringLiteral("FRAG_WIDTH= %1\n").arg(iFrgW)
            << QStringLiteral("FRAG_HEIGHT= %1\n").arg(iFrgH)
            << QStringLiteral("FRAG_ROWS= %1\n").arg(iCurRowCount)
            << QStringLiteral("FRAG_COLS= %1\n\n").arg(iCurColCount);

    }
    delete[] pBuf;
    emit progressUpdated(100, tr("finished"));
    emit overviewsBuilt(qstrOutPath);
    return qstrOutPath;
}

//备注：GDAL图像数据类型对应
/*
GDT_Unknown : 未知数据类型
GDT_Byte : 8bit正整型 (unsigned char)
GDT_UInt16 : 16bit正整型 ( unsigned short)
GDT_Int16 : 16bit整型 ( short 或 short int)
GDT_UInt32 : 32bit 正整型 (unsigned long)
GDT_Int32 : 32bit整型 (int 或 long 或 long int)
GDT_Float32 : 32bit 浮点型 (float)
GDT_Float64 : 64bit 浮点型 (double)
GDT_CInt16 : 16bit复整型
GDT_CInt32 : 32bit复整型
GDT_CFloat32 : 32bit复浮点型
GDT_CFloat64 : 64bit复浮点型
*/
