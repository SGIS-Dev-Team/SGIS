#include "modules/widget/mainwindow.h"
#include "modules/global.h"
#include "modules/slogger.h"
#include "gdal_priv.h"
#include <QApplication>
#include <QFileDialog>



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //使得程序可以在系统托盘运行
    app.setQuitOnLastWindowClosed(false);

    //启动主程序
    MainWindow wnd;

    return app.exec();
}
