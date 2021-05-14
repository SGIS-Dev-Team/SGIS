#include "sslice.h"


SSlice::SSlice()
{
}

SSlice::~SSlice()
{
}

QString SSlice::getOverviewsSlice(QString qstrInPath)
{   
    int iSliceMaxSize=2*SLICE_SIZE;

    //获取图像路径，设置相应的输出路径
    QFileInfo fileInfo(qstrInPath);
    //注册所有支持的数据格式驱动
    GDALAllRegister();
    //含中文QString->char*
    QByteArray qbarr = qstrInPath.toUtf8();// 不能两句合起来写
    const char* ch= qbarr.constData();
    GDALDataset* pDataset = static_cast<GDALDataset*>(GDALOpen(ch, GA_Update));  // 打开给定的数据文件
    //打开失败
    if (!pDataset)
    {
        return "";
    }
    //波段数
    int iBandCount = pDataset->GetRasterCount();
    //原图大小X,Y(width,height)
    int iOriX = pDataset->GetRasterXSize();
    int iOriY = pDataset->GetRasterYSize();
    //金字塔级数
    int iPyCount = 0;
    int iOriMax = MAX(iOriX,iOriY);
    iPyCount = ceil(qLn(iOriMax/double(iSliceMaxSize))/qLn(2))+1;


    //构建元数据文本文件
    //qstrBaseName是影像的无后缀文件名
    QString qstrBaseName=fileInfo.baseName();
    QDir qdirOutFile(fileInfo.path());
    if(!qdirOutFile.exists(qstrBaseName)){
        qdirOutFile.mkdir(qstrBaseName);
    }
    //这是输出文件夹路径
    QString qstrOutPath = qdirOutFile.filePath(qstrBaseName);
    //元数据
    QString temp=qstrOutPath+"/"+qstrBaseName+"_Meta.txt";
    QFile qfMeta(temp);
    qfMeta.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&qfMeta);
    //层数与原图大小
    out << QStringLiteral("PYRAMID_COUNT= %1\n").arg(iPyCount)
        << QStringLiteral("ORIGINAL_WIDTH= %1\n").arg(iOriX)
        << QStringLiteral("ORIGINAL_HEIGHT= %1\n\n").arg(iOriY);


    //注意
    GDALClose(pDataset);


    //逐层构建
    QDir qdirLevelFile(qstrOutPath);
    //分块大小和数量
    int iFrgW= SLICE_SIZE,iFrgH= SLICE_SIZE;
    int iPreColCount=1,iPreRowCount=1;//上一级
    int iCurColCount=1,iCurRowCount=1;//本级
    int iXOff=0,iYOff=0;//偏移量
    int iX=0,iY=0;//暂时存储一个图像的宽高
    int iPreX=0,iPreY=0;//记录上一级拼图像的大小

    //相应路径
    QString qstrPreLevel = "";
    QString qstrPreSlice = "";
    QString qstrCurLevel = "";
    QString qstrCurSlice = "";
    //内存分配
    unsigned char* pBuf = new unsigned char[4 * iSliceMaxSize * iSliceMaxSize * iBandCount];
    //写图像的相关指针
    GDALDriver* pDriver= GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset* pDsWrite=nullptr;

    //当前金字塔级数下的尺寸(x--width--col,y--height--col)
    int iCurX=0,iCurY=0,iCurMax;
    for(int i=1;i<=iPyCount;++i)
    {
        //计算大小和分块
        iCurMax=iOriMax/pow(2,i-1);
        iCurX=iOriX/pow(2,i-1);
        iCurY=iOriY/pow(2,i-1);
        iFrgW=SLICE_SIZE;
        iFrgH=SLICE_SIZE;

        /*-------------------------------*/
        //达到尺寸要求结束
        if(iCurMax<=SLICE_SIZE*2)
        {
            iCurColCount=1;
            iCurRowCount=1;
            iFrgW=iCurX;
            iFrgH=iCurY;

            //将上一级切片的全部合入
            //说明不是原图
            if(iPyCount>1)
            {
                //上一级文件夹
                qstrPreLevel = qdirLevelFile.filePath(QString::number(pow(2,i-2)));
                //创建当前级的文件夹
                if(!qdirLevelFile.exists(QString::number(pow(2,i-1)))){
                    qdirLevelFile.mkdir(QString::number(pow(2,i-1)));
                }
                //当前级的文件夹路径
                qstrCurLevel = qdirLevelFile.filePath(QString::number(pow(2,i-1)));
                //当前级的创建切片文件路径
                qstrCurSlice = qstrCurLevel+ "/"+qstrBaseName+"_1.tif";


                //QString->char*
                qbarr = qstrCurSlice.toUtf8();// 不能两句合起来写
                ch= qbarr.constData();
                pDsWrite = pDriver->Create(ch, iCurX, iCurY, iBandCount, GDT_Byte, NULL);
                //根据切片的原理要合并3*3=9 ~ 4*4=16张图像
                //默认切片是对齐的，这很重要
                for(int y=0;y<iPreRowCount;++y)
                {
                    iXOff=0;
                    for(int x=0;x<iPreColCount;++x)
                    {
                        //TODO:这个地方直接用.tif后缀感觉有些不太保险,以后可以考虑改一下
                        qstrPreSlice=qstrPreLevel+"/"+qstrBaseName+"_"+QString::number(iPreColCount*y+x+1)+".tif";

                        //读图像
                        //含中文QString->char*
                        qbarr = qstrPreSlice.toUtf8();// 不能两句合起来写
                        ch= qbarr.constData();
                        pDataset = static_cast<GDALDataset*>(GDALOpen(ch, GA_Update));
                        if (!pDataset)
                        {
                            continue;
                        }
                        iX=pDataset->GetRasterXSize();
                        iY=pDataset->GetRasterYSize();
                        pDataset->RasterIO(GF_Read, 0, 0, iX, iY, pBuf, iX, iY , GDT_Byte, iBandCount, NULL, iBandCount, iX * iBandCount, 1);
                        GDALClose(pDataset);


                        pDsWrite->RasterIO(GF_Write, iXOff, iYOff, iX / 2, iY / 2, pBuf, iX, iY, GDT_Byte, iBandCount, NULL, iBandCount, iX * iBandCount, 1);
                        iXOff+=iX / 2;
                    }
                    iYOff+=iY/2;
                }
                //处理结束
                GDALClose(pDsWrite);
            }
            //直接将原图复制
            else
            {
                // TODO
                //读图像
                //含中文QString->char*
                qbarr = qstrInPath.toUtf8();// 不能两句合起来写
                ch= qbarr.constData();
                pDataset = static_cast<GDALDataset*>(GDALOpen(ch, GA_Update));
                if (!pDataset)
                {
                    return "";
                }

                iX=pDataset->GetRasterXSize();
                iY=pDataset->GetRasterYSize();
                pDataset->RasterIO(GF_Read, 0, 0, iX, iY, pBuf, iX, iY , GDT_Byte, iBandCount, NULL, iBandCount, iX * iBandCount, 1);
                GDALClose(pDataset);

                pDsWrite = pDriver->Create(ch, iX, iY, iBandCount, GDT_Byte, NULL);
                pDsWrite->RasterIO(GF_Write, 0, 0, iY, iY, pBuf, iX, iY, GDT_Byte, iBandCount, NULL, iBandCount, iX * iBandCount, 1);
            }       
        }
        /*-------------------------------*/
        //第一级直接分割
        else if(i==1)
        {
            //从原图开始分割
            //读图像
            //含中文QString->char*
            qbarr = qstrInPath.toUtf8();// 不能两句合起来写
            ch= qbarr.constData();
            pDataset = static_cast<GDALDataset*>(GDALOpen(ch, GA_Update));
            if (!pDataset)
            {
                return "";
            }
            iCurColCount=ceil(iCurX/double(SLICE_SIZE));
            iCurRowCount=ceil(iCurY/double(SLICE_SIZE));

            int iCount=1;
            for(int y=0;y<iCurRowCount;++y)
            {
                if(y==iCurRowCount-1)
                {
                    iY=iCurY-SLICE_SIZE*(iCurRowCount-1);
                }
                else
                {
                    iY=SLICE_SIZE;
                }
                iYOff=y*SLICE_SIZE;
                for(int x=0;x<iCurColCount;++x)
                {
                    if(x==iCurColCount-1)
                    {
                        iX=iCurX-SLICE_SIZE*(iCurColCount-1);
                    }
                    else
                    {
                        iX=SLICE_SIZE;
                    }
                    iXOff=x*SLICE_SIZE;
                    pDataset->RasterIO(GF_Read, iXOff, iYOff, iX, iY, pBuf, iX, iY , GDT_Byte, iBandCount, NULL, iBandCount, iX * iBandCount, 1);

                    //创建当前级的文件夹
                    if(!qdirLevelFile.exists(QString::number(pow(2,i-1)))){
                        qdirLevelFile.mkdir(QString::number(pow(2,i-1)));
                    }
                    //当前级的文件夹路径
                    qstrCurLevel = qdirLevelFile.filePath(QString::number(pow(2,i-1)));
                    //当前级的创建切片文件路径
                    qstrCurSlice = qstrCurLevel+ "/"+qstrBaseName+"_"+QString::number(iCount)+".tif";

                    qbarr = qstrCurSlice.toUtf8();// 不能两句合起来写
                    ch= qbarr.constData();
                    //TODO:进度条以后可能要用
                    pDsWrite = pDriver->Create(ch, iX, iY, iBandCount, GDT_Byte, NULL);
                    pDsWrite -> RasterIO(GF_Write, 0, 0, iX, iY, pBuf, iX, iY, GDT_Byte, iBandCount, NULL, iBandCount, iX * iBandCount, 1);


                    GDALClose(pDsWrite);
                    ++iCount;
                    //写图像
                }
            }
            GDALClose(pDataset);
        }
        //其他情况从上一级文件夹中读取
        else
        {
            iPreColCount=iCurColCount;
            iCurColCount=ceil(iCurX/double(SLICE_SIZE));
            iPreRowCount=iCurRowCount;
            iCurRowCount=ceil(iCurY/double(SLICE_SIZE));


            //创建当前级的文件夹
            if(!qdirLevelFile.exists(QString::number(pow(2,i-1)))){
                qdirLevelFile.mkdir(QString::number(pow(2,i-1)));
            }
            //当前级的文件夹路径
            qstrCurLevel = qdirLevelFile.filePath(QString::number(pow(2,i-1)));
            //上一级的文件夹路径
            qstrPreLevel = qdirLevelFile.filePath(QString::number(pow(2,i-2)));

            int iCount=1;

            for(int y=0;y<iCurRowCount;++y)
            {
                if(y==iCurRowCount-1)
                {
                    iY=iCurY-SLICE_SIZE*(iCurRowCount-1);
                }
                else
                {
                    iY=SLICE_SIZE;
                }
                for(int x=0;x<iCurColCount;++x)
                {
                    if(x==iCurColCount-1)
                    {
                        iX=iCurX-SLICE_SIZE*(iCurColCount-1);
                    }
                    else
                    {
                        iX=SLICE_SIZE;
                    }
                    //写图像
                    //当前级的创建切片文件路径
                    qstrCurSlice = qstrCurLevel+ "/"+qstrBaseName+"_"+QString::number(iCount)+".tif";
                    qbarr = qstrCurSlice.toUtf8();// 不能两句合起来写
                    ch= qbarr.constData();
                    pDsWrite = pDriver->Create(ch, iX, iY, iBandCount, GDT_Byte, NULL);


                    iPreX=0,iPreY=0;
                    iYOff=0;
                    for(int m=0,py=2*y;py<iPreRowCount&&m<2;++m,++py)
                    {
                        iXOff=0;
                        for(int n=0,px=2*x;px<iPreColCount&&n<2;++n,++px)
                        {
                            //TODO:这个地方直接用.tif后缀感觉有些不太保险,以后可以考虑改一下
                            qstrPreSlice=qstrPreLevel+"/"+qstrBaseName+"_"+QString::number(iPreColCount*py+px+1)+".tif";


                            //读图像
                            //含中文QString->char*
                            qbarr = qstrPreSlice.toUtf8();// 不能两句合起来写
                            ch= qbarr.constData();
                            pDataset = static_cast<GDALDataset*>(GDALOpen(ch, GA_Update));
                            if (!pDataset)
                            {
                                continue;
                            }
                            iPreX=pDataset->GetRasterXSize();
                            iPreY=pDataset->GetRasterYSize();
                            pDataset->RasterIO(GF_Read, 0, 0, iPreX, iPreY, pBuf, iPreX, iPreY , GDT_Byte, iBandCount, NULL, iBandCount, iX * iBandCount, 1);
                            GDALClose(pDataset);


                            pDsWrite -> RasterIO(GF_Write, iXOff, iYOff, iPreX/2, iPreY/2, pBuf, iPreX, iPreY, GDT_Byte, iBandCount, NULL, iBandCount, iX * iBandCount, 1);
                            iXOff+=iPreX/2;

                        }
                        iYOff+=iPreY/2;
                    }
                    //写完一张图像
                    GDALClose(pDsWrite);
                    ++iCount;
                }
            }


        }
        //保存元文件数据
        out << QStringLiteral("PYRAMID_LEVEL= %1\n").arg(pow(2,i-1))
            << QStringLiteral("LEVEL_WIDTH= %1\n").arg(iCurX)
            << QStringLiteral("LEVEL_HEIGHT= %1\n").arg(iCurY)
            << QStringLiteral("FRAG_WIDTH= %1\n").arg(iFrgW)
            << QStringLiteral("FRAG_HEIGHT= %1\n").arg(iFrgH)
            << QStringLiteral("FRAG_ROWS= %1\n").arg(iCurColCount)
            << QStringLiteral("FRAG_COLS= %1\n\n").arg(iCurRowCount);
    }

    //内存分配
    delete[] pBuf;
    return qstrOutPath;
}
