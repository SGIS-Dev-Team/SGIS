#include "qdataimportwizard.h"
#include "ui_qdataimportwizard.h"
#include <QThread>
#include <QPushButton>
#include <QMessageBox>
#include <gdal_priv.h>
#include <QComboBox>
#include <QFileInfo>
#include <modules/paint/simage.h>

QDataImportWizard::QDataImportWizard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QDataImportWizard)
{
    ui->setupUi(this);
}

QDataImportWizard::QDataImportWizard(const QStringList &imagePathList, QWidget *parent): QDataImportWizard(parent)
{
    setImagePathList(imagePathList);
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

    streamMeta.setImported(false);
}

void QDataImportWizard::onButtonImportClicked()
{

}

void QDataImportWizard::onPreviewImageLoaded()
{
    if(muCurrentPreviewIndex == muCurrentLoadingPreviewIndex)
        ui->mLabelPreviewImage->setPixmap(mStreamMetaVec[muCurrentPreviewIndex].get()->previewImage().getImage());

    this->setCursor(QCursor());
    ui->mLabelTitle->setText(tr("Choose three bands to preview"));

    ui->mComboRed->setEnabled(true);
    ui->mComboGreen->setEnabled(true);
    ui->mComboBlue->setEnabled(true);
}

void QDataImportWizard::setImagePathList(const QStringList &imagePathList)
{
    mStreamMetaVec.clear();

    for(auto &path : imagePathList)
        this->mStreamMetaVec.push_back(
            std::make_shared<SImageStreamMeta>(
                path, SOverviewBuilder::generatePyramidDir(path, mStrPyramidSavePath)));

    _initialize();
}

void QDataImportWizard::_initialize()
{
    if(mStreamMetaVec.empty())
        return;

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
    for(; muCurrentBuildingIndex != mStreamMetaVec.size(); ++muCurrentBuildingIndex)
        if(!mStreamMetaVec[muCurrentBuildingIndex].get()->isOverviewsReady())
            break;

    //发射构建开始信号
    if(muCurrentBuildingIndex != mStreamMetaVec.size())
        emit startBuildingThread(mStreamMetaVec[muCurrentBuildingIndex].get()->originialImagePath(),
                                 this->mStrPyramidSavePath, SOverviewBuilder::TIFF);

    //初始化链接
    _initializeConnections();

    //设置显示第一张图像的缩略图
    _setCurrentImage(0);
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
    ui->mComboRed   ->setCurrentIndex(bandCount > 3 ? 0 : 0);
    ui->mComboGreen ->setCurrentIndex(bandCount > 3 ? 1 : 0);
    ui->mComboBlue  ->setCurrentIndex(bandCount > 3 ? 2 : 0);

    //------更新图像元数据简略信息栏------//
    ui->mLabelImageMeta->setText(_generateMetaInfo(streamMeta));
    mbUpdatePreviewImg = true;

    //更新预览图
    onComboBoxIndexChanged(0);
}

QString QDataImportWizard::_generateMetaInfo(const SImageStreamMeta & streamMeta)
{
    QString metaInfoStr;
    QTextStream textStream(&metaInfoStr);
    textStream << QFileInfo(streamMeta.originialImagePath()).fileName() << '\n';
    const SImageMeta &meta = streamMeta.imageMeta();
    textStream << meta.width() << " * " << meta.height()
               << "\t" << meta.bandCount()
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
    connect(this, &QDataImportWizard::startLoadPreview, pStreamMeta, &SImageStreamMeta::loadPreviewImage);
    connect(pStreamMeta, &SImageStreamMeta::previewImageLoaded, this, &QDataImportWizard::onPreviewImageLoaded);
    //事件队列处理完成后释放该QThread临时对象的内存
    connect(pPreviewLoaderThread, &QThread::finished, pPreviewLoaderThread, &QObject::deleteLater);

    pPreviewLoaderThread->start();
    //为该临时线程的事件队列添加任务
    emit startLoadPreview(r, g, b);
    //在标题设置状态提示
    ui->mLabelTitle->setText(tr("Loading Preview Image..."));
}

void QDataImportWizard::onOverviewBuilt()
{
    auto &streamMeta = *mStreamMetaVec[muCurrentBuildingIndex];
    //更新状态信息
    streamMeta.setStreamStatus(tr("Overviews are ready for this image."));
    streamMeta.setBuildingProgress(100);
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
