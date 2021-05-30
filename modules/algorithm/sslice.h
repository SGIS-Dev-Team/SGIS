#ifndef SSLICE_H
#define SSLICE_H

#include <QString>
#include <QTextCodec>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QtCore/qmath.h>
#include <QStandardPaths>
#include "gdal_priv.h"

//分片大小
#define SLICE_SIZE 800


//------------------------------------------
//  SSlice类
//  切片和金字塔
//  直接调用getOverviewsSlice函数即可
//------------------------------------------


class SSlice: public QObject
{
    Q_OBJECT
public:
    explicit SSlice();
    ~SSlice();

    /*-----虚函数重载-----*/
public:

    /*-----属性-----*/
protected:

    /*-----信号-----*/
signals:
    //进度和提示信息
    void progressUpdated(int progress, QString info);
    //“输出文件夹”的路径
    void overviewsBuilt(QString qstrOut);

    /*-----槽-----*/
public slots:
    //将输入路径对应的图片切片后，返回输出文件夹路径
    QString getOverviewsSlice(QString qstrInPath);

    /*-----成员变量-----*/
private:
    /*-----成员函数-----*/
    //（为了解决中文路径问题）输入要读取的影像路径，返回数据集指针，
    GDALDataset* GetDataset(QString qstrInPath);
    //（为了解决中文路径问题）输入将要写入的影像路径以及相关参数(GDALDataset.create的参数)，返回数据集指针
    GDALDataset* CreateDataset(GDALDriver* pDriver, QString qstrInPath, int nXSize, int nYSize, int nBands,
                               GDALDataType eType, char** papszOptions);

public:
    //[访问函数]


    //[修改函数]


    //[功能函数]

};
#endif // SSLICE_H
