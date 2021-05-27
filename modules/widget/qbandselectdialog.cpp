#include "qbandselectdialog.h"
#include "ui_qbandselectdialog.h"
#include <QThread>
#include <QPushButton>
#include <QMessageBox>
#include <gdal_priv.h>
#include <QComboBox>
#include <modules/paint/simage.h>

QBandSelectDialog::QBandSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QBandSelectDialog)
{
    ui->setupUi(this);
}

QBandSelectDialog::QBandSelectDialog(const QString &imagePath, QWidget *parent): QBandSelectDialog(parent)
{
    mStrOriImgPath = imagePath;
    _initialize();
}

QBandSelectDialog::~QBandSelectDialog()
{
    mSlicerThread.quit();
    _releaseAll();
    delete ui;
}

void QBandSelectDialog::onProgressUpdated(int progress, QString info)
{
    ui->mProgressBarBuildOverview->setValue(progress);
    ui->mLabelInfo->setText(info);
}

void QBandSelectDialog::onComboBoxIndexChanged(int idx)
{
    //更新选择
    int channel = -1;
    if(mnRedBandIdx != ui->mComboRed->currentIndex() + 1)
        mnRedBandIdx = ui->mComboRed->currentIndex() + 1, channel = 0;
    if(mnGreenBandIdx != ui->mComboGreen->currentIndex() + 1)
        mnGreenBandIdx = ui->mComboGreen->currentIndex() + 1, channel = 1;
    if(mnBlueBandIdx != ui->mComboBlue->currentIndex() + 1)
        mnBlueBandIdx = ui->mComboBlue->currentIndex() + 1, channel = 2;

    if(!this->mbUpdatePreviewImg)
        return;

    //------根据三个组合框选择的波段更新图像------//
    if(channel != -1)
        mImage.setBandIndex(channel, idx + 1, true);

    ui->mLabelPreviewImage->setPixmap(QPixmap::fromImage(mImage.getImage()));
}

void QBandSelectDialog::onButtonOkClicked()
{
    mSlicerThread.quit();
}

void QBandSelectDialog::onButtonCancelClicked()
{
    mnBands = 0;
    mnRedBandIdx = mnGreenBandIdx = mnBlueBandIdx = 0;
    mStrFragPath = "";

    mSlicerThread.quit();
}

QString QBandSelectDialog::getFragPath() const
{
    return mStrFragPath;
}

QString QBandSelectDialog::getOriginalImagePath() const
{
    return mStrOriImgPath;
}

void QBandSelectDialog::setOriginalImagePath(const QString & strOriImgPath)
{
    mStrOriImgPath = strOriImgPath;
}

int QBandSelectDialog::getBlueBandIdx() const
{
    return mnBlueBandIdx;
}

void QBandSelectDialog::getBandIdices(int *pRGBIdx) const
{
    pRGBIdx[0] = mnRedBandIdx;
    pRGBIdx[1] = mnGreenBandIdx;
    pRGBIdx[2] = mnBlueBandIdx;
}

void QBandSelectDialog::getHistEqFunc(std::shared_ptr<void> pEqFunc[])
{
    if(!mImage.isNull())
        mImage.getHistEqFunc(pEqFunc);
}

void QBandSelectDialog::_initialize()
{
    //提示等待
    ui->mLabelStatus->setText(tr("Please wait while building overview images."));
    ui->mButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    //-----获取影像信息-----//

    SImageMeta meta = SImage::getMetaOf(mStrOriImgPath);
    mnBands = meta.bandCount();
    //计算降采样倍率
    mDownSamplingRatio = pow(2, SOverviewBuilder::calcPyramidLevelCount(meta) - 1);

    //------更新组合框-----//

    QStringList bandStrList{};
    for(int i = 0; i < mnBands; ++i)
        bandStrList.push_back("Band " + QString::number(i + 1));

    ui->mComboRed->addItems(bandStrList);
    ui->mComboGreen->addItems(bandStrList);
    ui->mComboBlue->addItems(bandStrList);

    if(mnBands == 1 || mnBands == 2)
    {
        ui->mComboRed->setCurrentIndex(0);
        ui->mComboGreen->setCurrentIndex(0);
        ui->mComboBlue->setCurrentIndex(0);
        mnRedBandIdx = mnGreenBandIdx = mnBlueBandIdx = 1;
    }
    else
    {
        ui->mComboRed->setCurrentIndex(0);
        ui->mComboGreen->setCurrentIndex(1);
        ui->mComboBlue->setCurrentIndex(2);
        mnRedBandIdx = 1, mnGreenBandIdx = 2, mnBlueBandIdx = 3;
    }

    //------更新图像显示------//
    mbUpdatePreviewImg = true;
    //加载缩略图
    mImage.setBandIndices(mnRedBandIdx, mnGreenBandIdx, mnBlueBandIdx, false);
    mImage.load(QRect(0, 0, 0, 0), mDownSamplingRatio, mStrOriImgPath);

    onComboBoxIndexChanged(0);

    //------开启金字塔构建线程------//
    SOverviewBuilder* builder = new SOverviewBuilder();
    builder->moveToThread(&mSlicerThread);
    connect(builder, &SOverviewBuilder::progressUpdated, this, &QBandSelectDialog::onProgressUpdated);
    connect(builder, &SOverviewBuilder::overviewsBuilt, this, &QBandSelectDialog::onOverviewBuilt);
    connect(this, &QBandSelectDialog::startBuildingThread, builder, &SOverviewBuilder::buildOverviews);
    connect(&mSlicerThread, &QThread::finished, builder, &QObject::deleteLater);

    mSlicerThread.start();

    QString savePath = SGIS_DOCUMENT_FOLDER + PYRAMID_FOLDER_NAME;
    emit startBuildingThread(mStrOriImgPath, savePath, SOverviewBuilder::TIFF);
    //初始化链接
    _initializeConnections();
}

void QBandSelectDialog::_initializeConnections()
{
    //链接组合框事件
    connect(ui->mComboRed, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &QBandSelectDialog::onComboBoxIndexChanged);
    connect(ui->mComboGreen, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &QBandSelectDialog::onComboBoxIndexChanged);
    connect(ui->mComboBlue, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &QBandSelectDialog::onComboBoxIndexChanged);

    //链接按钮响应
    connect(ui->mButtonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &QBandSelectDialog::onButtonOkClicked);
    connect(ui->mButtonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &QBandSelectDialog::onButtonCancelClicked);
}

void QBandSelectDialog::_releaseAll()
{
}

int QBandSelectDialog::getGreenBandIdx() const
{
    return mnGreenBandIdx;
}

int QBandSelectDialog::getRedBandIdx() const
{
    return mnRedBandIdx;
}

void QBandSelectDialog::onOverviewBuilt(QString strFragPath)
{
}

