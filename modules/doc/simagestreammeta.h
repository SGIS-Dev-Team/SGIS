#ifndef SIMAGESTREAMMETA_H
#define SIMAGESTREAMMETA_H
#include <QString>
#include "modules/global.h"
#include "modules/paint/simage.h"
#include <QObject>

//------------------------------
//      SImageStreamMeta类
//        图像数据流描述类
//------------------------------

class SImageStreamMeta: public QObject
{
    Q_OBJECT
private:
    QString mStrOriImagePath{};
    QString mStrPyramidDir{};
    SImageMeta mImageMeta{};
    int mnBuildingProgress{};
    QString mStrStreamStatus{};
    SImage mPreviewImage{};
    bool mbImported{};

    /*-----信号-----*/
signals:
    void overviewsBuilt(QString pyramidDirPath);
    void previewImageLoaded();

    /*-----槽函数-----*/
public slots:
    void loadPreviewImage(int r = 0, int g = 0, int b = 0);
    void moveBackToMainThread();

public:
    //[访问函数与修改函数]
    inline const QString &originialImagePath()const {return mStrOriImagePath;}
    void setOriginalImagePath(const QString &path);
    const QString &pyramidDirPath()const {return mStrPyramidDir;}
    inline void setPyramidDirPath(const QString &path) {mStrPyramidDir = path;}
    const SImageMeta &imageMeta()const;
    void setImageMeta(const SImageMeta& _imageMeta) {mImageMeta = _imageMeta;}
    inline int buildingProgress()const {return mnBuildingProgress;}
    void setBuildingProgress(int value);
    inline const QString &streamStatus()const {return mStrStreamStatus;}
    inline void setStreamStatus(const QString &infoStr) {mStrStreamStatus = infoStr;}
    const SImage &previewImage()const {return mPreviewImage;}
    SImage &previewImage() {return mPreviewImage;}
    inline bool isImported()const {return mbImported;}
    inline void setImported(bool imported) {mbImported = imported;}

    //[功能函数]
private:
    //延迟调用
    void _loadImageMeta();
    //延迟调用
    void _loadPreviewImage(int r = 0, int g = 0, int b = 0);
    void _initializeWith(const SImageStreamMeta &smeta);

public:
    //清空
    void clear();
    //判断金字塔是否已经存在
    bool checkPyramidExistence();
    //判断是否构建完毕
    inline bool isOverviewsReady()const {return mnBuildingProgress == 100;}
    //获取被选中的波段
    inline int getRedBandIndex()const {if(!mPreviewImage.isNull()) return mPreviewImage.redBandIdx(); else return mImageMeta.bandCount() > 3 ? 1 : 1;}
    inline int getGreenBandIndex()const {if(!mPreviewImage.isNull()) return mPreviewImage.greenBandIdx(); else return mImageMeta.bandCount() > 3 ? 2 : 1;}
    inline int getBlueBandIndex()const {if(!mPreviewImage.isNull()) return mPreviewImage.blueBandIdx(); else return mImageMeta.bandCount() > 3 ? 3 : 1;}
    void getBandIdices(int pBandIndices[])const;
    //获取均衡化函数
    void getHistEqFunc(std::shared_ptr<void> pHistEqFunc[])const;

    //[构造函数与析构函数]
public:
    explicit SImageStreamMeta() = default;
    explicit SImageStreamMeta(const QString &_oriImgPath,
                              const QString &_pyramidDirPath,
                              const SImageMeta &_imageMeta = SImageMeta(),
                              int _buildingProgress = 0,
                              const SImage &_previewImage = SImage()):
        mStrOriImagePath(_oriImgPath),
        mStrPyramidDir(_pyramidDirPath),
        mImageMeta(_imageMeta),
        mnBuildingProgress(_buildingProgress),
        mPreviewImage(_previewImage)
    {
        if(checkPyramidExistence())mnBuildingProgress = 100;
    }
    virtual ~SImageStreamMeta() {}
};

#endif // SIMAGESTREAMMETA_H
