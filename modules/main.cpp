#include "modules/widget/mainwindow.h"
#include "modules/global.h"
#include "gdal_priv.h"
#include <QApplication>
#include <QFileDialog>
#include "modules/algorithm/soverviewbuilder.h"
#include <QStyleFactory>
#include "modules/algorithm/sreadshp.h"

int main(int argc, char* argv[])
{
    GDALAllRegister();

#ifndef QT_NO_DEBUG
    //VLDGlobalDisable();
#endif
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("mac"));
    //使得程序可以在系统托盘运行
    app.setQuitOnLastWindowClosed(false);

    //启动主程序
    MainWindow wnd;

    SReadShp readShp;
    readShp.setShpPath("G:/SHP/road.shp");

    RecordMap temp = readShp.getFeatureWithCoor();
    QRectF rect = readShp.getboundingRect(true);
    return app.exec();
}
