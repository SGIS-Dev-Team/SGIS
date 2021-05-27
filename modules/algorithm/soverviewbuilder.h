#ifndef SOVERVIEWBUILDER_H
#define SOVERVIEWBUILDER_H

//-----------------------------------
//          SOverviewBuilder
//           金字塔构建器类
//  金字塔层数包含原图层。
//-----------------------------------

#include <QObject>
#include <gdal_priv.h>
#include "modules/paint/simage.h"

class SOverviewBuilder : public QObject
{
    Q_OBJECT
public:
    //金字塔文件格式枚举
    enum Format
    {
        JPEG,
        TIFF
    };

public:
    explicit SOverviewBuilder(QObject *parent = nullptr);
    virtual ~SOverviewBuilder() {}

    /*-----信号-----*/
signals:
    //进度和提示信息
    void progressUpdated(int progress, QString info);
    //“输出文件夹”的路径
    void overviewsBuilt(QString savePath);

    /*-----槽-----*/
public slots:
    //路径对应的影像
    void buildOverviews(QString imagePath, QString savePath, Format format = TIFF);

private:
    void _build_overviews_tiff(const QString &imagePath, const QString& savePath);
    /*-----成员变量-----*/
protected:
    //目标图像路径
    QString mStrImagePath;
    //保存文件夹路径
    QString mStrSavePath;
    //[Me]
    const QString Me = "SOverviewBuilder";


public:
    //[访问函数]
    const QString &getImagePath()const {return mStrImagePath;}
    const QString &getSavePath()const {return mStrSavePath;}

    //[功能函数]
public:
    //计算金字塔层数
    static int calcPyramidLevelCount(const SImageMeta &meta);
private:
    //生成元数据字符串
    QString _generateMetaString(const QString &oriImgPath, const QString &savePath, int levelCount);

};

#endif // SOVERVIEWBUILDER_H
