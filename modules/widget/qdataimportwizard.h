#ifndef QBANDSELECTDIALOG_H
#define QBANDSELECTDIALOG_H

#include "modules/global.h"
#include <QDialog>
#include <modules/paint/simage.h>
#include "modules/algorithm/soverviewbuilder.h"
#include <QThread>
#include <modules/doc/simagestreammeta.h>
#include <modules/paint/sfragloader.h>
#include <modules/paint/sfragimage.h>
#include <QStringListModel>
#include <QItemSelectionModel>
#include "modules/widget/wizard/extract/qextractwizard.h"

namespace Ui
{
class QDataImportWizard;
}

class QDataImportWizard : public QDialog
{
    Q_OBJECT

    /*-----构造函数与析构函数-----*/
public:
    explicit QDataImportWizard(QWidget* parent = nullptr);
    explicit QDataImportWizard(const QStringList& imagePathList, QWidget* parent = nullptr);
    virtual ~QDataImportWizard();

    /*-----虚函数重载-----*/
protected:

    /*-----信号-----*/
signals:
    void startBuildingThread(QString imgPath, QString savePath, SOverviewBuilder::Format format = SOverviewBuilder::TIFF);
    void startLoadingPreview(int r = 0, int g = 0, int b = 0);
    //确认进行导入操作
    void importingData();
    //确认在后台进行构建操作
    void holdingBackground();

    /*-----槽函数-----*/
private slots:
    //金字塔、分片处理完毕
    void onOverviewBuilt();
    //进度条
    void onProgressUpdated(int progress, QString info);
    //组合框选项变化
    void onComboBoxIndexChanged(int idx);
    //按下导入按钮
    void onButtonImportClicked();
    //按下后台按钮
    void onButtonBackgroundClicked();
    //预览图加载完成
    void onPreviewImageLoaded();
    //图像列表双击事件
    void onListItemDoubleClicked(const QModelIndex& index);
    //图像列表右键菜单触发
    void onListViewMenuActionOpenInExplorerTriggered();
    void onListViewMenuActionAddImageTriggered();
    void onListViewMenuActionAddArchiveTriggered();
    void onListViewMenuActionRemoveTriggered();
    void onListViewMenuActionRemoveAllTriggered();
    void onListViewMenuActionRebuildOverviewsTriggered();
    void onListViewMenuActionShowMetaDataTriggered();

    /*-----成员变量-----*/
protected:
    //线程锁
    QMutex mMutex;
    //图像数据流描述对象容器：数据量不大，插入操作并不多，可以使用vector
    std::vector<std::shared_ptr<SImageStreamMeta>> mStreamMetaVec{};
    //当前选中的预览图像
    int mnCurrentPreviewIndex{-1};
    //当前正在构建的对象:-1为当前无构造
    int mnCurrentBuildingIndex{-1};
    //当前正在加载预览图的对象
    int mnCurrentLoadingPreviewIndex{-1};
    //所有金字塔缓存目录(sgis/pyramid)
    QString mStrPyramidSavePath = SGIS_DOCUMENT_FOLDER + DEFAULT_PYRAMID_FOLDER_NAME;
    //是否根据选择框更新预览图像
    bool mbUpdatePreviewImg{false};
    //构建器线程
    QThread mBuilderThread{};
    //构建器
    SOverviewBuilder mOverviewBuilder{};
    //图像列表数据模型
    QStringListModel mImageListModel{};
    //图像列表选择模型
    QItemSelectionModel mListItemSltModel{};

    /*-----成员函数-----*/
public:
    //[访问函数]
    inline const SImageStreamMeta& getStreamMeta(size_t idx)const {Q_ASSERT(idx < mStreamMetaVec.size()); return *mStreamMetaVec.at(idx);};
    inline const std::vector<std::shared_ptr<SImageStreamMeta>>& getStreamMetaVec()const {return mStreamMetaVec;}
    inline std::vector<std::shared_ptr<SImageStreamMeta>>& getStreamMetaVec() {return mStreamMetaVec;}
    inline size_t getStreamMetaCount()const {return mStreamMetaVec.size();}

    //[修改函数]
    inline void removeStreamMeta(size_t idx) {mStreamMetaVec.erase(mStreamMetaVec.begin() + idx);}
    void setImagePaths(const QStringList& imagePathList);
    void addImagePaths(const QStringList& imagePathList);
    void removeStream(size_t idx);
    void removeStreams(std::vector<size_t> indices);

    //[功能函数]
public:
    bool isEmpty()const {return mStreamMetaVec.empty();}
private:
    void _initialize();
    void _initializeConnections();
    //设置当前预览的图像
    void _setCurrentImage(size_t idx);
    //根据当前预览的图像生成简略元数据信息文本
    QString _generateMetaInfo(const SImageStreamMeta& streamMeta);
    void _releaseAll();
    //开临时线程加载预览图
    void _loadPreviewImage(size_t idx, int r = 0, int g = 0, int b = 0);

    /*-----UI-----*/
private:
    Ui::QDataImportWizard* ui;

    QExtractWizard* pExtractWizard{nullptr};
};

#endif // QBANDSELECTDIALOG_H
