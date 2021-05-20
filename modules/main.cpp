#include "modules/widget/mainwindow.h"
#include "modules/global.h"
#include "modules/slogger.h"
#include "gdal_priv.h"
#include <QApplication>
#include <QFileDialog>
#include "sslice.h"



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //使得程序可以在系统托盘运行
    app.setQuitOnLastWindowClosed(false);

    //启动主程序
    MainWindow wnd;

    QString  qstr=QFileDialog::getOpenFileName();
    QString out = SSlice::getOverviewsSlice(qstr);

#ifdef USE
    int a = 0;

    const char* fileName = ch;

    GDALAllRegister();  // 注册所有支持的数据格式驱动
    GDALDataset* poDataset = static_cast<GDALDataset*>(GDALOpen(fileName, GA_Update));  // 打开给定的数据文件
    if (!poDataset)
    {

        return EXIT_FAILURE;
    }

    int anOverviewList[3] = { 2, 4, 8 };
    poDataset->BuildOverviews("NEAREST", 3, anOverviewList, 0, nullptr,
        GDALDummyProgress, nullptr);
    a = poDataset->GetRasterCount();
    GDALClose(poDataset); // 关闭数据集



    int iRatio = 2;
    GDALDataset* pSet = (GDALDataset*)GDALOpen(fileName, GA_ReadOnly);

    GDALRasterBand* pBand = pSet->GetRasterBand(1);
    int iX = pSet->GetRasterXSize();
    int iY = pSet->GetRasterYSize();
    int nBand = pSet->GetRasterCount();
    nBand = 3;
    unsigned char* pData = new unsigned char[iX * iY * nBand];

    pSet->RasterIO(GF_Read, 0, 0, iX, iY, pData, iX / iRatio, iY /iRatio, GDT_Byte, nBand, NULL, nBand, iX * nBand, 1);
    GDALClose(pSet);



    const char* fileName1 = "G:/new1.tif";
    GDALDriver* pDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset* pSetWrite = pDriver->Create(fileName1, iX / iRatio, iY / iRatio, nBand, GDT_Byte, NULL);


    pSetWrite->RasterIO(GF_Write, 0, 0, iX / iRatio, iY / iRatio, pData, iX / iRatio, iY / iRatio, GDT_Byte, nBand, NULL, nBand, iX * nBand, 1);
    GDALClose(pSetWrite);


    delete[] pData;
#endif
    return app.exec();
}
