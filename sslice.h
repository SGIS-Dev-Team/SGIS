#ifndef SSLICE_H
#define SSLICE_H

#include <QString>
#include <QTextCodec>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QtCore/qmath.h>
#include "gdal_priv.h"

//分片大小
#define SLICE_SIZE 800


//------------------------------------------
//  SSlice类
//  切片和金字塔
//  该类是一个静态的类，不能实例化和继承，
//  直接调用静态函数即可
//------------------------------------------


class SSlice
{
private:
    explicit SSlice();
    ~SSlice();

    /*-----虚函数重载-----*/
public:

    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
private:
    /*-----成员函数-----*/
    //（为了解决中文路径问题）输入要读取的影像路径，返回数据集指针，
    static GDALDataset* GetDataset(QString qstrInPath);
    //（为了解决中文路径问题）输入将要写入的影像路径以及相关参数(GDALDataset.create的参数)，返回数据集指针
    static GDALDataset* CreateDataset(GDALDriver* pDriver, QString qstrInPath, int nXSize, int nYSize, int nBands,
                                      GDALDataType eType, char** papszOptions);

public:
    //[访问函数]


    //[修改函数]


    //[功能函数]
    //将输入路径对应的图片切片后，返回输出文件夹路径
    static QString getOverviewsSlice(QString qstrInPath);

};
#endif // SSLICE_H
