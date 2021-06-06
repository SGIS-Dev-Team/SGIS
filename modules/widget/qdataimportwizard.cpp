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

QDataImportWizard::QDataImportWizard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QDataImportWizard)
{
    ui->setupUi(this);
    //初始化
    _initialize();
}

QDataImportWizard::QDataImportWizard(const QStringList &imagePathList, QWidget *parent): QDataImportWizard(parent)
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
    this->mStreamMetaVec[muCurrentBuildingIndex].get()->setBuildingProgress(progress);
}

void QDataImportWizard::onComboBoxIndexChanged(int idx)
{
    if(!this->mbUpdatePreviewImg)
        return;
    //改变鼠标图标
    this->setCursor(QCursor(Qt::BusyCursor));

    auto &streamMeta = *mStreamMetaVec[muCurrentPreviewIndex];
    //根据三个组合框选择的波段更新预览图
    _loadPreviewImage(muCurrentPreviewIndex,
                      ui->mComboRed->currentIndex() + 1,
                      ui->mComboGreen->currentIndex() + 1,
                      ui->mComboBlue->currentIndex() + 1);
    //非信号调用
    if(idx != -1)
        streamMeta.setImported(false);
}

void QDataImportWizard::onButtonImportClicked()
{

}

void QDataImportWizard::onPreviewImageLoaded()
{
    if(muCurrentPreviewIndex == muCurrentLoadingPreviewIndex)
    {
        SImageStreamMeta &streamMeta = *mStreamMetaVec[muCurrentPreviewIndex];
        ui->mLabelPreviewImage->setPixmap(streamMeta.previewImage().getImage());
        ui->mLabelPreviewImage->setMaximumSize(streamMeta.imageMeta().imageSize());
    }

    this->setCursor(QCursor());
    ui->mLabelTitle->setText(tr("Choose three bands to preview"));

    ui->mComboRed->setEnabled(true);
    ui->mComboGreen->setEnabled(true);
    ui->mComboBlue->setEnabled(true);

    auto pStreamMeta = mStreamMetaVec[muCurrentLoadingPreviewIndex].get();
    disconnect(this, &QDataImportWizard::startLoadingPreview, pStreamMeta, &SImageStreamMeta::loadPreviewImage);
    disconnect(pStreamMeta, &SImageStreamMeta::previewImageLoaded, this, &QDataImportWizard::onPreviewImageLoaded);
}

void QDataImportWizard::onListItemDoubleClicked(const QModelIndex &index)
{
    size_t rowIdx = static_cast<size_t>(index.row());
    if(rowIdx != muCurrentPreviewIndex)
        _setCurrentImage(rowIdx);
}

void QDataImportWizard::onListViewMenuActionOpenInExplorerTriggered()
{
    QModelIndex index = mListItemSltModel.currentIndex();
    const QString &imagePath = mStreamMetaVec[index.row()]->originialImagePath();
    QDesktopServices::openUrl(QUrl(QFileInfo(imagePath).path()));
}

void QDataImportWizard::onListViewMenuActionAddImageTriggered()
{
    QStringList imagePathList = QFileDialog::getOpenFileNames(this, tr("Add Image"), tr(""), tr("Images (*.tif *.tiff)"));
    if(imagePathList.isEmpty())
        return;

    addImagePaths(imagePathList);
}

void QDataImportWizard::onListViewMenuActionRemoveTriggered()
{

}

void QDataImportWizard::onListViewMenuActionRemoveAllTriggered()
{

}

void QDataImportWizard::onListViewMenuActionRebuildOverviewsTriggered()
{

}

void QDataImportWizard::onListViewMenuActionShowMetaDataTriggered()
{

}

void QDataImportWizard::setImagePaths(const QStringList &imagePathList)
{
    mStreamMetaVec.clear();
    mImageListModel.removeRows(0, mImageListModel.rowCount());
    mListItemSltModel.clear();

    addImagePaths(imagePathList);
}

void QDataImportWizard::addImagePaths(const QStringList &imagePathList)
{
    //构造并追加图像数据流
    for(auto &path : imagePathList)
        this->mStreamMetaVec.push_back(
            std::make_shared<SImageStreamMeta>(
                path, SOverviewBuilder::generatePyramidDir(path, mStrPyramidSavePath)));

    //更新图像列表数据模型
    QStringList imageFileNameList;
    for(auto &imagePath : imagePathList)
        imageFileNameList << QFileInfo(imagePath).fileName();
    mImageListModel.setStringList(mImageListModel.stringList() << imageFileNameList);

    //设置显示第一张图像的缩略图
    if(!mListItemSltModel.currentIndex().isValid())
        _setCurrentImage(0);
}

void QDataImportWizard::removeStream(size_t idx)
{
    Q_ASSERT(idx < mStreamMetaVec.size());
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
    connect(&mBuilderThread, &QThread::finished, &mOverviewBuilder, &QObject::deleteLater);
    mBuilderThread.start();

    //找到第一个未构建金字塔的图像
    for(muCurrentBuildingIndex = 0; muCurrentBuildingIndex != mStreamMetaVec.size(); ++muCurrentBuildingIndex)
        if(!mStreamMetaVec[muCurrentBuildingIndex].get()->isOverviewsReady())
            break;

    //发射构建开始信号
    if(muCurrentBuildingIndex != mStreamMetaVec.size())
        emit startBuildingThread(mStreamMetaVec[muCurrentBuildingIndex].get()->originialImagePath(),
                                 this->mStrPyramidSavePath, SOverviewBuilder::TIFF);

    //------准备图像列表数据模型------//
    //数据模型
    mImageListModel.removeRows(0, mImageListModel.rowCount());
    //选择模型
    mListItemSltModel.clear();
    mListItemSltModel.setModel(&mImageListModel);
    //链接模型与视图
    ui->mImageListView->setModel(&mImageListModel);
    ui->mImageListView->setSelectionModel(&mListItemSltModel);

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
    connect(ui->mImageListView->getAction(QImageListView::Remove), &QAction::triggered,
            this, &QDataImportWizard::onListViewMenuActionRemoveTriggered);
    connect(ui->mImageListView->getAction(QImageListView::Remove_All), &QAction::triggered,
            this, &QDataImportWizard::onListViewMenuActionRemoveAllTriggered);
    connect(ui->mImageListView->getAction(QImageListView::Show_Meta_Data), &QAction::triggered,
            this, &QDataImportWizard::onListViewMenuActionShowMetaDataTriggered);
    connect(ui->mImageListView->getAction(QImageListView::Rebuild_Overviews), &QAction::triggered,
            this, &QDataImportWizard::onListViewMenuActionRebuildOverviewsTriggered);
}

void QDataImportWizard::_setCurrentImage(size_t idx)
{
    Q_ASSERT(idx < mStreamMetaVec.size());
    mbUpdatePreviewImg = false;
    muCurrentPreviewIndex = idx;

    const SImageStreamMeta &streamMeta = *mStreamMetaVec[idx];

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
    for(int i = 0; i < bandCount; ++i)
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

QString QDataImportWizard::_generateMetaInfo(const SImageStreamMeta & streamMeta)
{
    QString metaInfoStr;
    QTextStream textStream(&metaInfoStr);
    textStream << QFileInfo(streamMeta.originialImagePath()).fileName() << '\n';
    const SImageMeta &meta = streamMeta.imageMeta();
    textStream << meta.width() << " * " << meta.height()
               << "    " << meta.bandCount()
               << tr(" bands(") << GDALGetDataTypeSizeBits(meta.dataType()) << tr(" bit)\n");
    return metaInfoStr;
}

void QDataImportWizard::_releaseAll()
{

}

void QDataImportWizard::_loadPreviewImage(size_t idx, int r, int g, int b)
{
    Q_ASSERT(idx < mStreamMetaVec.size());
    //锁定组合框
    ui->mComboRed->setEnabled(false);
    ui->mComboGreen->setEnabled(false);
    ui->mComboBlue->setEnabled(false);

    muCurrentLoadingPreviewIndex = idx;

    QThread *pPreviewLoaderThread = new QThread();

    SImageStreamMeta *pStreamMeta = mStreamMetaVec[idx].get();

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
    auto &streamMeta = *mStreamMetaVec[muCurrentBuildingIndex];
    //更新状态信息
    streamMeta.setStreamStatus(tr("Overviews are ready for this image."));
    streamMeta.setBuildingProgress(100);
    //TODO:将列表项底色变绿

    //找到下一张未构建金字塔的影像
    while(++muCurrentBuildingIndex != mStreamMetaVec.size())
        if(!streamMeta.isOverviewsReady())
            break;
    //进行下一张图像的构建
    if(muCurrentBuildingIndex != mStreamMetaVec.size())
        emit startBuildingThread(streamMeta.originialImagePath(),
                                 mStrPyramidSavePath,
                                 SOverviewBuilder::Format::TIFF);
}
