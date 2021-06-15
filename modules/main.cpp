#include "modules/widget/mainwindow.h"
#include "modules/global.h"
#include "gdal_priv.h"
#include <QApplication>
#include <QFileDialog>
#include "modules/algorithm/soverviewbuilder.h"

int main(int argc, char *argv[])
{
#ifndef QT_NO_DEBUG
    //VLDGlobalDisable();
#endif
    QApplication app(argc, argv);
    //使得程序可以在系统托盘运行
    app.setQuitOnLastWindowClosed(false);

    //启动主程序
    MainWindow wnd;

    GDALAllRegister();
    qRegisterMetaType<SOverviewBuilder::Format>("Format");


    return app.exec();
}
