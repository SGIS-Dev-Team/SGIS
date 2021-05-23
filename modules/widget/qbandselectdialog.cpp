#include "qbandselectdialog.h"
#include "ui_qbandselectdialog.h"
#include <QThread>
#include "modules/algorithm/sslice.h"
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

void QBandSelectDialog::onOverviewBuilt(QString strFragPath)
{
    this->mStrFragPath = strFragPath;
    if(mStrFragPath.isEmpty())
    {
        QMessageBox::critical(this, tr("Fragment images path invalid"), tr(""));
        return;
    }

    //提示可以操作
    ui->mLabelStatus->setText(tr("Choose three bands to preview"));
    ui->mButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);

    //------获取到顶层图像位置------//

    QString strImageName = QFileInfo(strFragPath).fileName();
    //读入金字塔元数据（金字塔层数）
    QFile metaFile(strFragPath + '/' + strImageName + "_Meta.txt");
    metaFile.open(QFile::ReadOnly);
    QTextStream metaStream(&metaFile);
    int pyramidCount{};
    QString buf{};
    metaStream >> buf >> pyramidCount;
    //合成顶层图像路径
    QString strTopImgPath = mStrFragPath + '/' +
                            QString::number(static_cast<int>(pow(2, pyramidCount - 1))) + '/' +
                            strImageName + "_1.tif";
    metaFile.close();

    //------读入影像------//

    mImage.load(strTopImgPath);
    mnBands = mImage.getBandCount();

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
    }
    else
    {
        ui->mComboRed->setCurrentIndex(0);
        ui->mComboGreen->setCurrentIndex(1);
        ui->mComboBlue->setCurrentIndex(2);
    }

    //------更新图像显示------//
    mbUpdatePreviewImg = true;
    onComboBoxIndexChanged(0);
}

void QBandSelectDialog::onProgressUpdated(int progress, QString info)
{
    ui->mProgressBarBuildOverview->setValue(progress);
    ui->mLabelInfo->setText(info);
}

void QBandSelectDialog::onComboBoxIndexChanged(int idx)
{
    Q_UNUSED(idx);
    //更新选择
    mnRedBandIdx = ui->mComboRed->currentIndex() + 1;
    mnGreenBandIdx = ui->mComboGreen->currentIndex() + 1;
    mnBlueBandIdx = ui->mComboBlue->currentIndex() + 1;

    if(!this->mbUpdatePreviewImg)
        return;

    //------根据三个组合框选择的波段更新图像------//

    mImage.setBandIndices(mnRedBandIdx, mnGreenBandIdx, mnBlueBandIdx);
    mImage.load();

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

    SSlice* slicer = new SSlice();
    slicer->moveToThread(&mSlicerThread);
    connect(slicer, &SSlice::progressUpdated, this, &QBandSelectDialog::onProgressUpdated);
    connect(slicer, &SSlice::overviewsBuilt, this, &QBandSelectDialog::onOverviewBuilt);
    connect(this, &QBandSelectDialog::startBuildingThread, slicer, &SSlice::getOverviewsSlice);
    connect(&mSlicerThread, &QThread::finished, slicer, &QObject::deleteLater);

    mSlicerThread.start(QThread::HighestPriority);

    emit startBuildingThread(mStrOriImgPath);

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


