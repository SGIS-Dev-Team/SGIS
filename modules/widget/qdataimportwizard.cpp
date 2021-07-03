#include "qdataimportwizard.h"
#include "ui_qdataimportwizard.h"
#include <QThread>
#include <QPushButton>
#include <QMessageBox>
#include <gdal_priv.h>
#include <QComboBox>
#include <QFileInfo>
#include <modules/paint/simage.h>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QFileDialog>
#include <modules/widget/wizard/extract/qextractwizard.h>

QDataImportWizard::QDataImportWizard(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::QDataImportWizard)
{
    ui->setupUi(this);
    //初始化
    _initialize();
}

QDataImportWizard::QDataImportWizard(const QStringList& imagePathList, QWidget* parent): QDataImportWizard(parent)
{
    //添加数据
    addImagePaths(imagePathList);
}

QDataImportWizard::~QDataImportWizard()
{
    mBuilderThread.quit();
    _releaseAll();
    delete ui;
}

void QDataImportWizard::onProgressUpdated(int progress, QString info)
{
    //更新GUI
    ui->mProgressBarBuildOverview->setValue(progress);
    ui->mLabelStatus->setText(info);
    //更新流
    this->mStreamMetaVec[mnCurrentBuildingIndex].get()->setBuildingProgress(progress);
}

void QDataImportWizard::onComboBoxIndexChanged(int idx)
{
    if (!this->mbUpdatePreviewImg)
        return;
    //改变鼠标图标
    this->setCursor(QCursor(Qt::BusyCursor));

    auto& streamMeta = *mStreamMetaVec[mnCurrentPreviewIndex];
    //根据三个组合框选择的波段更新预览图
    _loadPreviewImage(mnCurrentPreviewIndex,
                      ui->mComboRed->currentIndex() + 1,
                      ui->mComboGreen->currentIndex() + 1,
                      ui->mComboBlue->currentIndex() + 1);
    //非信号调用
    if (idx != -1)
        streamMeta.setImported(false);
}

void QDataImportWizard::onButtonImportClicked()
{
    //全部已完成构建
    if (mnCurrentBuildingIndex == -1)
    {
        this->hide();
        emit importingData();
    }
    else
    {
        QMessageBox::critical(this, tr("Failed"), tr("Currently building overviews for added images."));
    }
}

void QDataImportWizard::onButtonBackgroundClicked()
{
    this->hide();
    emit holdingBackground();
}

void QDataImportWizard::onPreviewImageLoaded()
{
    if (mnCurrentPreviewIndex == mnCurrentLoadingPreviewIndex)
    {
        SImageStreamMeta& streamMeta = *mStreamMetaVec[mnCurrentPreviewIndex];
        ui->mLabelPreviewImage->setPixmap(streamMeta.previewImage().getImage());
        ui->mLabelPreviewImage->setMaximumSize(streamMeta.imageMeta().imageSize());
    }

    this->setCursor(QCursor());
    ui->mLabelTitle->setText(tr("Choose three bands to preview"));

    //恢复控件响应
    ui->mComboRed->setEnabled(true);
    ui->mComboGreen->setEnabled(true);
    ui->mComboBlue->setEnabled(true);
    ui->mImageListView->setEnabled(true);


    auto pStreamMeta = mStreamMetaVec[mnCurrentLoadingPreviewIndex].get();
    disconnect(this, &QDataImportWizard::startLoadingPreview, pStreamMeta, &SImageStreamMeta::loadPreviewImage);
    disconnect(pStreamMeta, &SImageStreamMeta::previewImageLoaded, this, &QDataImportWizard::onPreviewImageLoaded);
}

void QDataImportWizard::onListItemDoubleClicked(const QModelIndex& index)
{
    int rowIdx = index.row();
    if (rowIdx != mnCurrentPreviewIndex)
        _setCurrentImage(rowIdx);
    //清空选择
    mListItemSltModel.clear();
}

void QDataImportWizard::onListViewMenuActionOpenInExplorerTriggered()
{
    QModelIndex index = mListItemSltModel.currentIndex();
    const QString& imagePath = mStreamMetaVec[index.row()]->originialImagePath();
    QDesktopServices::openUrl(QUrl(QFileInfo(imagePath).path()));
    //清空选择
    mListItemSltModel.clear();
}

void QDataImportWizard::onListViewMenuActionAddImageTriggered()
{
    QStringList imagePathList = QFileDialog::getOpenFileNames(this, tr("Add Image"), tr(""), tr("Images (*.tif *.tiff)"));
    if (imagePathList.isEmpty())
        return;

    addImagePaths(imagePathList);
    //清空选择
    mListItemSltModel.clear();
}

void QDataImportWizard::onListViewMenuActionAddArchiveTriggered()
{
    //清空选择
    mListItemSltModel.clear();

    //初始化解压向导
    if (!pExtractWizard)
        pExtractWizard = new QExtractWizard();
    else
    {
        if (pExtractWizard->page(QExtractWizard::ProgressPage)->isComplete())
        {
            delete pExtractWizard;
            pExtractWizard = new QExtractWizard;
        }
    }

    pExtractWizard->show();
    pExtractWizard->raise();
}

void QDataImportWizard::onListViewMenuActionRemoveTriggered()
{
    QModelIndexList idxList = mListItemSltModel.selectedIndexes();

    //是否包含正在显示的图像
    bool includeCurrentPreview {false};
    //临时数组；用来存要删除的序号
    std::vector<size_t> dltIdxVec;
    dltIdxVec.reserve(idxList.size());

    for (auto& idx : idxList)
    {
        if (idx.row() == mnCurrentPreviewIndex)
            includeCurrentPreview = true;
        dltIdxVec.emplace_back(idx.row());
    }

    //执行删除
    removeStreams(dltIdxVec);

    if (includeCurrentPreview && mStreamMetaVec.size() != 0)
    {
        _setCurrentImage(0);
        mListItemSltModel.select(mImageListModel.index(0), QItemSelectionModel::ClearAndSelect);
    }
}

void QDataImportWizard::onListViewMenuActionRemoveAllTriggered()
{
    setImagePaths(QStringList());
}

void QDataImportWizard::onListViewMenuActionRebuildOverviewsTriggered()
{
    //检测当前是否正在构建该图像的金字塔
    int idx = mListItemSltModel.currentIndex().row();
    if (idx == mnCurrentBuildingIndex)
    {
        QMessageBox::critical(this, tr("Unsuccessful"), tr("Currently building overviews for the selected image.\nPlease retry later."));
        return;
    }

    //重置金字塔的显示
    SImageStreamMeta& streamMeta = *mStreamMetaVec[idx];

    if (idx == mnCurrentPreviewIndex)
    {
        ui->mLabelStatus->setText(tr(""));
        ui->mProgressBarBuildOverview->setValue(0);
    }

    //重设构建进度
    streamMeta.setBuildingProgress(0);

    //删除原有金字塔数据
    SOverviewBuilder::removeExistingPyramid(streamMeta.pyramidDirPath());

    //如果当前不在构建金字塔，则继续构建
    if (mnCurrentBuildingIndex == -1)
        onOverviewBuilt();
}

void QDataImportWizard::onListViewMenuActionShowMetaDataTriggered()
{

}

void QDataImportWizard::setImagePaths(const QStringList& imagePathList)
{
    mStreamMetaVec.clear();
    mImageListModel.removeRows(0, mImageListModel.rowCount());
    mListItemSltModel.clear();
    //添加图像
    addImagePaths(imagePathList);
}

void QDataImportWizard::addImagePaths(const QStringList& imagePathList)
{
    //构造并追加图像数据流
    for (auto& path : imagePathList)
        this->mStreamMetaVec.push_back(
            std::make_shared<SImageStreamMeta>(
                path, SOverviewBuilder::generatePyramidDir(path, mStrPyramidSavePath)));

    //更新图像列表数据模型
    QStringList imageFileNameList;
    for (auto& imagePath : imagePathList)
        imageFileNameList << QFileInfo(imagePath).fileName();
    mImageListModel.setStringList(mImageListModel.stringList() << imageFileNameList);

    //设置显示第一张图像的缩略图
    if (mStreamMetaVec.size() > 0)
        _setCurrentImage(0);

    //如果没有正在构建的金字塔
    if (mnCurrentBuildingIndex == -1)
        onOverviewBuilt();
}

void QDataImportWizard::removeStream(size_t idx)
{
    Q_ASSERT(idx < mStreamMetaVec.size());
    mImageListModel.removeRow(idx);
    mStreamMetaVec.erase(mStreamMetaVec.begin() + idx);
}

void QDataImportWizard::removeStreams(std::vector<size_t> indices)
{
    //先从大到小排序
    std::sort(indices.rbegin(), indices.rend());
    //从大到小删
    for (auto idx : indices)
        removeStream(idx);
}

void QDataImportWizard::_initialize()
{
    mStreamMetaVec.clear();

    //------更新图像显示------//
    mbUpdatePreviewImg = true;

    //------准备金字塔构建线程------//
    mOverviewBuilder.moveToThread(&mBuilderThread);
    connect(&mOverviewBuilder, &SOverviewBuilder::progressUpdated, this, &QDataImportWizard::onProgressUpdated);
    connect(&mOverviewBuilder, &SOverviewBuilder::overviewsBuilt, this, &QDataImportWizard::onOverviewBuilt);
    qRegisterMetaType<SOverviewBuilder::Format>("Format");
    connect(this, &QDataImportWizard::startBuildingThread, &mOverviewBuilder, &SOverviewBuilder::buildOverviews);

    mBuilderThread.start();

    //------准备图像列表数据模型------//
    //数据模型
    mImageListModel.removeRows(0, mImageListModel.rowCount());
    //选择模型
    mListItemSltModel.clear();
    mListItemSltModel.setModel(&mImageListModel);
    //链接模型与视图
    ui->mImageListView->setModel(&mImageListModel);
    ui->mImageListView->setSelectionModel(&mListItemSltModel);
    //设置选择模式
    ui->mImageListView->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);

    //初始化链接
    _initializeConnections();
}

void QDataImportWizard::_initializeConnections()
{
    //链接组合框事件
    connect(ui->mComboRed, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &QDataImportWizard::onComboBoxIndexChanged);
    connect(ui->mComboGreen, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &QDataImportWizard::onComboBoxIndexChanged);
    connect(ui->mComboBlue, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &QDataImportWizard::onComboBoxIndexChanged);
    //链接列表视图事件
    connect(ui->mImageListView, &QListView::doubleClicked, this, &QDataImportWizard::onListItemDoubleClicked);
    //链接列表视图右键菜单触发事件
    connect(ui->mImageListView->getAction(QImageListView::Open_In_Explorer), &QAction::triggered,
            this, &QDataImportWizard::onListViewMenuActionOpenInExplorerTriggered);
    connect(ui->mImageListView->getAction(QImageListView::Add_Image), &QAction::triggered,
            this, &QDataImportWizard::onListViewMenuActionAddImageTriggered);
    connect(ui->mImageListView->getAction(QImageListView::Add_Archive), &QAction::triggered,
            this, &QDataImportWizard::onListViewMenuActionAddArchiveTriggered);
    connect(ui->mImageListView->getAction(QImageListView::Remove), &QAction::triggered,
            this, &QDataImportWizard::onListViewMenuActionRemoveTriggered);
    connect(ui->mImageListView->getAction(QImageListView::Remove_All), &QAction::triggered,
            this, &QDataImportWizard::onListViewMenuActionRemoveAllTriggered);
    connect(ui->mImageListView->getAction(QImageListView::Show_Meta_Data), &QAction::triggered,
            this, &QDataImportWizard::onListViewMenuActionShowMetaDataTriggered);
    connect(ui->mImageListView->getAction(QImageListView::Rebuild_Overviews), &QAction::triggered,
            this, &QDataImportWizard::onListViewMenuActionRebuildOverviewsTriggered);
    //链接对话框按钮响应
    connect(ui->mButtonImport, &QPushButton::clicked, this, &QDataImportWizard::onButtonImportClicked);
    connect(ui->mButtonBackground, &QPushButton::clicked, this, &QDataImportWizard::onButtonBackgroundClicked);
}

void QDataImportWizard::_setCurrentImage(size_t idx)
{
    Q_ASSERT(idx < mStreamMetaVec.size());
    mbUpdatePreviewImg = false;
    mnCurrentPreviewIndex = idx;

    const SImageStreamMeta& streamMeta = *mStreamMetaVec[idx];

    //显示状态文本、设置进度条
    ui->mLabelStatus->setText(streamMeta.streamStatus());
    ui->mProgressBarBuildOverview->setValue(streamMeta.buildingProgress());

    //------更新波段选择组合框-----//
    ui->mComboRed   ->clear();
    ui->mComboGreen ->clear();
    ui->mComboBlue  ->clear();

    int bandCount = streamMeta.imageMeta().bandCount();
    //根据当前图片生成波段列表
    QStringList bandStrList{};
    for (int i = 0; i < bandCount; ++i)
        bandStrList.push_back("Band " + QString::number(i + 1));
    //添加波段列表到组合框
    ui->mComboRed   ->addItems(bandStrList);
    ui->mComboGreen ->addItems(bandStrList);
    ui->mComboBlue  ->addItems(bandStrList);
    //确定当前显示的波段
    ui->mComboRed   ->setCurrentIndex(bandCount >= 3 ? 0 : 0);
    ui->mComboGreen ->setCurrentIndex(bandCount >= 3 ? 1 : 0);
    ui->mComboBlue  ->setCurrentIndex(bandCount >= 3 ? 2 : 0);

    //------更新图像元数据简略信息栏------//
    ui->mLabelImageMeta->setText(_generateMetaInfo(streamMeta));
    mbUpdatePreviewImg = true;

    //更新选中的图像
    mListItemSltModel.select(mImageListModel.index(idx), QItemSelectionModel::ClearAndSelect);

    //更新预览图
    onComboBoxIndexChanged(-1);
}

QString QDataImportWizard::_generateMetaInfo(const SImageStreamMeta& streamMeta)
{
    QString metaInfoStr;
    QTextStream textStream(&metaInfoStr);
    textStream << QFileInfo(streamMeta.originialImagePath()).fileName() << '\n';
    const SImageMeta& meta = streamMeta.imageMeta();
    textStream << meta.width() << " * " << meta.height()
               << "    " << meta.bandCount()
               << tr(" bands(") << GDALGetDataTypeSizeBits(meta.dataType()) << tr(" bit)\n");
    return metaInfoStr;
}

void QDataImportWizard::_releaseAll()
{
    //释放
    if (pExtractWizard)delete pExtractWizard;
    pExtractWizard = nullptr;
}

void QDataImportWizard::_loadPreviewImage(size_t idx, int r, int g, int b)
{
    Q_ASSERT(idx < mStreamMetaVec.size());
    //锁定组合框
    ui->mComboRed->setEnabled(false);
    ui->mComboGreen->setEnabled(false);
    ui->mComboBlue->setEnabled(false);
    //锁定影像列表框
    ui->mImageListView->setEnabled(false);

    mnCurrentLoadingPreviewIndex = idx;

    QThread* pPreviewLoaderThread = new QThread();

    SImageStreamMeta* pStreamMeta = mStreamMetaVec[idx].get();

    pStreamMeta->moveToThread(pPreviewLoaderThread);
    connect(this, &QDataImportWizard::startLoadingPreview, pStreamMeta, &SImageStreamMeta::loadPreviewImage);
    connect(pStreamMeta, &SImageStreamMeta::previewImageLoaded, this, &QDataImportWizard::onPreviewImageLoaded);
    //事件队列处理完成后释放该QThread临时对象的内存
    connect(pPreviewLoaderThread, &QThread::finished, pPreviewLoaderThread, &QObject::deleteLater);

    pPreviewLoaderThread->start();
    //为该临时线程的事件队列添加任务
    emit startLoadingPreview(r, g, b);
    //在标题设置状态提示
    ui->mLabelTitle->setText(tr("Loading Preview Image..."));
}

void QDataImportWizard::onOverviewBuilt()
{
    //检查当前是否在构建金字塔
    if (mnCurrentBuildingIndex != -1)
    {
        auto& streamMeta = *mStreamMetaVec[mnCurrentBuildingIndex];
        //更新状态信息
        streamMeta.setStreamStatus(tr("Overviews are ready for this image."));
        streamMeta.setBuildingProgress(100);
        //TODO:将列表项底色变绿
    }

    //找到一张未构建金字塔的影像
    for (mnCurrentBuildingIndex = 0; mnCurrentBuildingIndex != (int)mStreamMetaVec.size(); ++mnCurrentBuildingIndex)
        if (!mStreamMetaVec[mnCurrentBuildingIndex]->isOverviewsReady())
            break;
    //进行下一张图像的构建
    if (mnCurrentBuildingIndex != (int)mStreamMetaVec.size())
        emit startBuildingThread(mStreamMetaVec[mnCurrentBuildingIndex]->originialImagePath(),
                                 mStrPyramidSavePath,
                                 SOverviewBuilder::Format::TIFF);
    else
        mnCurrentBuildingIndex = -1;
}
