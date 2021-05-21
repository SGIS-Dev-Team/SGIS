#include "qbandselectdialog.h"
#include "ui_qbandselectdialog.h"
#include <QThread>
#include "modules/algorithm/sslice.h"
#include <QPushButton>
#include <QMessageBox>
#include <gdal_priv.h>
#include <QComboBox>

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
    _releaseAll();
    delete ui;
}

void QBandSelectDialog::onOverviewBuilt(QString strFragPath)
{
    if(mStrFragPath.isEmpty())
    {
        QMessageBox::critical(this, tr("Fragment images path invalid"), tr(""));
        return;
    }

    this->mStrFragPath = strFragPath;
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

    GDALAllRegister();

    QByteArray strTopImgPath_byte = strTopImgPath.toUtf8();
    GDALDataset* pDataSet = static_cast<GDALDataset*>(GDALOpen(strTopImgPath_byte.constData(), GA_ReadOnly));

    //获取数据集信息
    mnWidth = pDataSet->GetRasterXSize();
    mnHeight = pDataSet->GetRasterYSize();
    mnBands = pDataSet->GetRasterCount();

    //分配内存、读入各波段
    mpImageData = new UCHAR* [mnBands];
    for(int i = 0; i < mnBands; ++i)
    {
        mpImageData[i] = new UCHAR[mnWidth * mnHeight];
        pDataSet->GetRasterBand(i + 1)->RasterIO(GF_Read, 0, 0, mnWidth, mnHeight, mpImageData[i], mnWidth, mnHeight, GDT_Byte, 0, 0, 0);
    }

    GDALClose(pDataSet);

    //------更新组合框-----//

    QStringList bandStrList{};
    for(int i = 0; i < mnBands; ++i)
        bandStrList.push_back("Band " + QString::number(i + 1));

    ui->mComboRed->addItems(bandStrList);
    ui->mComboBlue->addItems(bandStrList);
    ui->mComboGreen->addItems(bandStrList);
    ui->mComboRed->setCurrentIndex(0);
    ui->mComboGreen->setCurrentIndex(1);
    ui->mComboBlue->setCurrentIndex(2);

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
    int pBandforRGB[3];
    this->bandIdices(pBandforRGB);

    UCHAR* pRGBData = new UCHAR[mnWidth * mnHeight * 3 * sizeof(UCHAR)] {0};

    //拷贝图像数据
    for(int i = 0; i < mnWidth * mnHeight; ++i)
        for(int band = 0; band < 3; ++band)
            pRGBData[i * 3 + band] = mpImageData[pBandforRGB[band] - 1][i];


    //创建QPixmap用于显示
    QImage previewImg(pRGBData, mnWidth, mnHeight, mnWidth * 3, QImage::Format_RGB888,
                      [](void* pData)->void{delete [] static_cast<uchar*>(pData); pData = nullptr;});

    ui->mLabelPreviewImage->setPixmap(QPixmap::fromImage(previewImg));
}

QString QBandSelectDialog::OriImgPath() const
{
    return mStrOriImgPath;
}

void QBandSelectDialog::setOriImgPath(const QString &strOriImgPath)
{
    mStrOriImgPath = strOriImgPath;
}

int QBandSelectDialog::blueBandIdx() const
{
    return mnBlueBandIdx;
}

void QBandSelectDialog::bandIdices(int *pRGBIdx) const
{
    pRGBIdx[0] = mnRedBandIdx;
    pRGBIdx[1] = mnGreenBandIdx;
    pRGBIdx[2] = mnBlueBandIdx;
}

void QBandSelectDialog::_initialize()
{
    //提示等待
    ui->mLabelStatus->setText(tr("Please wait while building overview images."));
    ui->mButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

#ifdef MULTI_THREAD_BUILDING
    QThread ovrBuildingThread;
    SSlice* slicer = new SSlice();
    slicer->moveToThread(ovrBuildingThread);
#else
    mStrFragPath = SSlice::getOverviewsSlice(mStrOriImgPath);
    onOverviewBuilt(mStrFragPath);
#endif

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
    //链接读取完成事件
}

void QBandSelectDialog::_releaseAll()
{
    if(mpImageData)
    {
        for(int i = 0; i < mnBands; ++i)
            delete [] mpImageData[i];
        delete[] mpImageData;
    }
    mpImageData = nullptr;
}

int QBandSelectDialog::greenBandIdx() const
{
    return mnGreenBandIdx;
}

int QBandSelectDialog::redBandIdx() const
{
    return mnRedBandIdx;
}

QString QBandSelectDialog::fragPath() const
{
    return mStrFragPath;
}

