#ifndef SOVERVIEWBUILDER_H
#define SOVERVIEWBUILDER_H
#include <QMetaType>
//-----------------------------------
//          SOverviewBuilder
//           金字塔构建器类
//  金字塔层数包含原图层。
//
//  --------------------------------------------------------------------------------------------------
//  变量                    描述           示例
//  (original)imagePath   源图像路径       xxx/xxx/xxx.tif(.tiff)
//  savePath              保存路径         通常为 [文档路径]/sgis/pyramid
//  pyramidDir            金字塔路径       通常为 [文档路径]/sgis/pyramid/[原始图像名称]
//  metaFilePath          元数据文件路径    通常为 [文档路径]/sgis/pyramid/[原始图像名称]/[原始图像名称]_Meta.txt
//  --------------------------------------------------------------------------------------------------
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
    //金字塔文件夹的路径
    void overviewsBuilt(QString pyramidDir);

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
    //生成元数据字符串
    static QString generateMetaString(const QString &oriImgPath, const QSize &oriImgSize, const QString &pyramidDir, int levelCount);

    /*检查金字塔是否已经构建，使得金字塔失效的情况：
     * 1. 原始图像文件被移动、重命名或删除
     * 2. 金字塔目录被移动、重命名或删除
     * 3. 金字塔描述文件被修改或软件版本更新
     * 4. 金字塔影像被移动、重命名或删除
     * 函数不会检查金字塔影像内容是否被编辑过
     * @param   oriImgPath      原始图像文件路径
     * @param   pyramidDir  金字塔目录路径（通常为pyramid/xxxxx(图像文件名)）
     * @return  是否为有效金字塔
     */
    static bool varifyPyramid(const QString &oriImgPath, const QString &pyramidDir);

    //根据源图像路径和金字塔保存路径生成目标金字塔文件夹路径，详见"SOverviewBuilder.h"最顶上的说明
    static QString generatePyramidDir(const QString &oriImagePath, const QString &savePath);

    //删除该目录下的金字塔，返回删除操作是否成功
    static bool removeExistingPyramid(const QString &pyramidDir);
};

Q_DECLARE_METATYPE(SOverviewBuilder::Format);

#endif // SOVERVIEWBUILDER_H
