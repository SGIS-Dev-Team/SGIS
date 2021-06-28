#include "modules/widget/mainwindow.h"
#include "modules/global.h"
#include "gdal_priv.h"
#include <QApplication>
#include <QFileDialog>
#include "modules/algorithm/soverviewbuilder.h"
#include "./modules//algorithm/sdecompression.h"
int main(int argc, char *argv[])
{
#ifndef QT_NO_DEBUG
    //VLDGlobalDisable();
#endif

	Sdecompression s("F:/SGIS_Dev/zlib-1.2.11.tar.gz", "D:/456");
	//Sdecompression s("F:/SGIS_Dev/123.zip", "D:/456");
 	auto lstFiles = s.preview();
 	s.unTar();
// 
// 	Sdecompression s1("D:/456/zlib-1.2.11.tar", "D:/789");
// 	s1.unZip();

	//QStringList list = JlCompress::extractDir("F:/SGIS_Dev/zlib-1.2.11.tar.gz", "D:/456");
    QApplication app(argc, argv);
    //使得程序可以在系统托盘运行
    app.setQuitOnLastWindowClosed(false);

    //启动主程序
    MainWindow wnd;

    GDALAllRegister();
    qRegisterMetaType<SOverviewBuilder::Format>("Format");


    return app.exec();
}
