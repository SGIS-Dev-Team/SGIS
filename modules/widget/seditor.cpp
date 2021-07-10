#include "seditor.h"
#include "ui_seditor.h"
#include <modules/paint/sshapefactory.h>
#include <QDir>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QDateTime>
#include <QVBoxLayout>
#include <QGridLayout>
#include <ctime>
#include "../paint/simage.h"
#include "simageinfowidget.h"
#include "modules/algorithm/sreadshp.h"
#include "ogrsf_frmts.h"
#include "scolorarea.h"
SEditor::SEditor(QWidget* parent): QMainWindow(parent),
    ui(new Ui::SEditor)
{
    ui->setupUi(this);
    _initialize();
    initDockPalleteWidget();
    showMaximized();
    //根据调用函数时发生错误的不同类型，修改字符串
    SLogger::getLogger()->addEntry("xxx", SLogger::LocalError, "Loading meta : meta text not exists.");
    SLogger::getLogger()->addEntry("YYY", SLogger::LocalError, "Loading meta : meta text not exists.");
    SLogger::getLogger()->addEntry("AAA", SLogger::LocalError, "Loading meta : meta text not exists.");
    SLogger::getLogger()->addEntry("BBB", SLogger::LocalError, "Loading meta : meta text not exists.");
}

SEditor::~SEditor()
{
    //TODO:释放所有手动创建且没有指定父控件的控件指针
    if (mpImportDialog)delete mpImportDialog;
    mpImportDialog = nullptr;

    delete ui;
}

void SEditor::onActionZoominTriggered()
{
    mpCurCanvasArea->canvas()->setScaleLevelUp();
}

void SEditor::onActionZoomoutTriggered()
{
    mpCurCanvasArea->canvas()->setScaleLevelDown();
}

#include <QRandomGenerator>
void SEditor::onActionCreateRectTriggered()
{
    mpCurDoc->getLayerManager().clearSelection();

    quint32 x = QRandomGenerator::system()->bounded(0, mpCurCanvasArea->canvas()->logicalSize().width());
    quint32 y = QRandomGenerator::system()->bounded(0, mpCurCanvasArea->canvas()->logicalSize().height());

    SShape* rect = SShapeFactory::createShape(ShapeSet::Hexagon);
    rect->setCenterPoint(QPoint(x, y));
    rect->scale(800, 800);
    rect->rotate(15);
    rect->setLayerName(rect->layerName() + QString::number(x + y));
    rect->setClose(true);
    rect->setFillRule(Qt::FillRule::WindingFill);
    mpCurDoc->getLayerManager().addLayer(rect);
}

#include <QFileDialog>
void SEditor::onActionLoadImageTriggered()
{
    mpCurDoc->getLayerManager().clearSelection();
    QStringList pathList = QFileDialog::getOpenFileNames(this);

    for (int i = 0; i < pathList.size(); ++i)
    {
        quint32 x = QRandomGenerator::system()->bounded(0, mpCurCanvasArea->canvas()->logicalSize().width());
        quint32 y = QRandomGenerator::system()->bounded(0, mpCurCanvasArea->canvas()->logicalSize().height());

        SImage* pImg = new SImage(pathList[i], true, QPoint(x, y));
        pImg->load();
        pImg->rotate(60);

        mpCurDoc->getLayerManager().addLayer(pImg);
    }
}

#include <modules/paint/sfragimage.h>
void SEditor::onActionLoadFragmentsTriggered()
{
    mpCurDoc->getLayerManager().clearSelection();

    QString path = QFileDialog::getExistingDirectory(this);

    QFileInfo file_info(path);

    if (path.isEmpty())
        return;

    SFragImage* pFragImg = new SFragImage(mpCurDoc->getFragLoader());

    pFragImg->setPyramidDir(file_info.filePath());
    pFragImg->loadMeta();

    pFragImg->setCenterPoint(QPointF((DEFAULT_CANVAS_SIZE / 2).width(), (DEFAULT_CANVAS_SIZE / 2).height()));

    pFragImg->setBandIndices(1, 2, 3);

    pFragImg->setHoldTopPyramidEnabled(true);

    mpCurDoc->getLayerManager().addLayer(pFragImg);


}

#include "qdataimportwizard.h"
void SEditor::onActionImportRasterTriggered()
{
    if (!mpImportDialog)
    {
        mpImportDialog = new QDataImportWizard();
        _connectDataImportWizard();
    }

    if (mpImportDialog->isEmpty())
    {
        //获取要读取的文件路径
        QStringList strImagePathList = QFileDialog::getOpenFileNames(this, tr("Select Raster Images"), "", "raster (*.tif *.tiff)");
        if (strImagePathList.isEmpty())
            return;

        mpImportDialog->addImagePaths(strImagePathList);
    }

    //打开数据导入向导对话框
    mpImportDialog->show();
    mpImportDialog->raise();
}

void SEditor::onImportingRaster()
{
    //将影像读入
    mpCurDoc->getLayerManager().clearSelection();

    auto& streamMetaVec = mpImportDialog->getStreamMetaVec();
    for (auto& pStreamMeta : streamMetaVec)
    {
        SImageStreamMeta& streamMeta = *pStreamMeta;

        if (streamMeta.isImported())
            continue;

        SFragImage* pFragImg = new SFragImage(streamMeta, mpCurDoc->getFragLoader());

        //更新坐标映射
        if (mpCurDoc->getCoordinate().isEmpty())
        {
            pFragImg->setCenterPoint(QPointF((DEFAULT_CANVAS_SIZE / 2).width(), (DEFAULT_CANVAS_SIZE / 2).height()));
            double* geoTrans = SImage::getMetaOf(pFragImg->getLargestImgPath()).geoTransform();
            double nWidth = SImage::getMetaOf(pFragImg->getLargestImgPath()).width();
            double nHeight = SImage::getMetaOf(pFragImg->getLargestImgPath()).height();
            QString proRef = SImage::getMetaOf(pFragImg->getLargestImgPath()).projRef();


            mpCurDoc->getCoordinate() = SCoordinate(pFragImg->centerPoint(), QPointF(geoTrans[0] + geoTrans[1] * nWidth / 2, geoTrans[3] + geoTrans[5] * nHeight / 2), geoTrans[1], geoTrans[5], proRef);
            if (geoTrans != nullptr)
                delete[] geoTrans;

            //获取了投影坐标和像素分辨率
            mpStatLblGSD->setText("1:" + QString::number(mpCurDoc->getCoordinate().deltaX()) + "meter");
            mpStatLblProjCS->setText(mpCurDoc->getCoordinate().projCS());

            //调整影像位置
            double min = mpCurCanvasArea->horizontalScrollBar()->pageStep() * 1.0 / nWidth;
            if (min > mpCurCanvasArea->verticalScrollBar()->pageStep() * 1.0 / nHeight)
                min = mpCurCanvasArea->verticalScrollBar()->pageStep() * 1.0 / nHeight;
            mpCurCanvasArea->canvas()->setScaleValue(0.9 * min);
            mpCurCanvasArea->horizontalScrollBar()->setValue(0.5 * mpCurCanvasArea->canvas()->actualSize().width() - 0.5 * mpCurCanvasArea->horizontalScrollBar()->pageStep());
            mpCurCanvasArea->verticalScrollBar()->setValue(0.5 * mpCurCanvasArea->canvas()->actualSize().height() - 0.5 * mpCurCanvasArea->verticalScrollBar()->pageStep());
        }
        else
        {
            //强制根据地理坐标设置像素坐标，和缩放像素分辨率
            double* geoTrans = SImage::getMetaOf(pFragImg->getLargestImgPath()).geoTransform();
            double nWidth = SImage::getMetaOf(pFragImg->getLargestImgPath()).width();
            double nHeight = SImage::getMetaOf(pFragImg->getLargestImgPath()).height();
            double logicX, logicY;
            mpCurDoc->getCoordinate().mapFromGeo(geoTrans[0] + geoTrans[1] * nWidth / 2, geoTrans[3] + geoTrans[5] * nHeight / 2, logicX, logicY);
            pFragImg->setCenterPoint(QPointF(logicX, logicY));
            pFragImg->scale(geoTrans[1] / mpCurDoc->getCoordinate().deltaX(), geoTrans[5] / mpCurDoc->getCoordinate().deltaY());
            if (geoTrans != nullptr)
                delete[] geoTrans;
        }

        //强制加载ImageMeta
        streamMeta.imageMeta();
        pFragImg->setBandIndices(streamMeta.getRedBandIndex(), streamMeta.getGreenBandIndex(), streamMeta.getBlueBandIndex());

        pFragImg->setHoldTopPyramidEnabled(true);

        std::shared_ptr<void> pEqFunc[3] {nullptr};
        streamMeta.getHistEqFunc(pEqFunc);
        pFragImg->setHistEqFunc(pEqFunc);

        mpCurDoc->getLayerManager().addLayer(pFragImg);
        streamMeta.setImported(true);
    }

}

#include <QTextEdit>
void SEditor::onActionReportLeaksTriggered()
{
#ifndef QT_NO_DEBUG
    VLDReportLeaks();
#endif
}

void SEditor::onActionImportShapeFileTriggered()
{
    QStringList shpFileList = QFileDialog::getOpenFileNames(this, tr("Import Shape File"), tr(""), tr("ESRI Shape File (*.shp)"));

    for (auto& filePath : shpFileList)
    {
        SReadShp reader(filePath);
    }
}

void SEditor::onActionBringForwardTriggered()
{
    mpCurDoc->getLayerManager().bringForward();
    mpCurCanvasArea->canvas()->updateViewArea();
}

void SEditor::onActionSendBackwardTriggered()
{
    mpCurDoc->getLayerManager().sendBackward();
    mpCurCanvasArea->canvas()->updateViewArea();
}

void SEditor::onActionBringtoFrontTriggered()
{
    mpCurDoc->getLayerManager().bringToFront();
    mpCurCanvasArea->canvas()->updateViewArea();
}

void SEditor::onActionSendtoBackTriggered()
{
    mpCurDoc->getLayerManager().sendToBack();
    mpCurCanvasArea->canvas()->updateViewArea();
}

void SEditor::onActionAlignLeftTriggered()
{
}

void SEditor::onActionAlignMiddleTriggered()
{
}

void SEditor::onActionAlignRightTriggered()
{
}

void SEditor::onActionAlignTopTriggered()
{
}

void SEditor::onActionAlignCenterTriggered()
{
}

void SEditor::onActionAlignBottomTriggered()
{
}

void SEditor::onActionDistributeHorizentallyTriggered()
{
}


void SEditor::onActionDistributeVerticallyTriggered()
{
}

void SEditor::onTabSwitched()
{

}

void SEditor::onCanvasMouseMoved(QPointF Log_pos)
{
    double x = 0, y = 0;
    if (mpCurDoc->getCoordinate().isEmpty())
    {
        //未建立坐标映射，显示逻辑坐标
        mpStatLblCursorPos->setText("(" + QString::number(Log_pos.x()) + "," + QString::number(Log_pos.y()) + ")");
    }
    else
    {
        //已建立坐标映射，显示投身坐标
        mpCurDoc->getCoordinate().mapToGeo(Log_pos.x(), Log_pos.y(), x, y);
        mpStatLblCursorPos->setText("(" + QString::number(x, 'f', 3) + "," + QString::number(y, 'f', 3) + ")meter");
    }
}

void SEditor::onCanvasScaled(double value)
{
    mpStatLblCanvasScale->setText(QString::number(value * 100, 'f', 2) + "%");
}

void SEditor::onLayersUpdated(SLayerManager* which)
{
    if (mpCurCanvasArea)
        mpCurCanvasArea->canvas()->repaint();
    //更新图层变更图像信息
    updateImageInfo(which);
}

void SEditor::onOutput(const QString& entry)
{
    //添加logger到输出窗口
    ui->mpOutputListWidget->insertItem(-1, entry);
}

void SEditor::onSelectColor()
{
    //根据选中颜色的控件，获取选中的颜色
    if (auto pColorArea = qobject_cast<SColorArea*>(sender()))
    {
        auto color = pColorArea->getColor();
        //auto strColor = QString("RGB(%0, %1, %2)").arg(color.red()).arg(color.green()).arg(color.blue());
        if (mpCurCanvasArea)
        {
            //绘图区域 发送颜色选中信号 - 回去区域中的画布会响应颜色改变的信号，从新使用新颜色绘制
            emit mpCurCanvasArea->colorChanged(color);
        }
    }
}

void SEditor::updateImageInfo(SLayerManager* which)
{
    //更新Tiff图片信息
    //获取当前选择的图层 -- 后续可能需要随着实现更改
    QString strSelectImageFilePath;
    //遍历当前选中图层，选择第一个图片图层
    auto lstSelectLayout = which->getSelectedLayerIterList();
    for (auto it : lstSelectLayout)
    {
        auto pLayer = *it;
        if (pLayer->isVisible())
        {
            if (auto pImage = dynamic_cast<SImage*>(pLayer))
            {
                strSelectImageFilePath = pImage->getImagePath();
                break;
            }
        }
    }
    fillByImageFilePath(strSelectImageFilePath);
}

void SEditor::fillByImageFilePath(QString strFilePath)
{
    if (auto pDataset = (GDALDataset *)GDALOpen(strFilePath.toUtf8().data(), GA_ReadOnly))
    {
        fillImageFileInfo(pDataset);
        fillImageLayerInfo(pDataset);
        fillImageProjectionInfo(pDataset);
        GDALClose(pDataset);
    }
    else
    {
        //
        ui->mpLayerNameLabel->clear();
        ui->mpFileTypeLabel->clear();
        ui->mpLastModifyTimeLabel->clear();
        ui->mpLayerCountLabel->clear();
        ui->mpAuxiliaryFilesComboBox->clear();
        ui->mpFileSizeLabel->clear();
        //
        ui->mpWidthLabel->clear();
        ui->mpHeightLabel->clear();
        ui->mpTypeLabel->clear();
        ui->mpBlockWidthLabel->clear();
        ui->mpBlockHeightLabel->clear();
        ui->mpDataTypeLabel->clear();
        ui->mpCompressionLabel->clear();
        ui->mpDataOrderLabel->clear();
        ui->mpPyramidLayerAlgorithmLabel->clear();
        //
        ui->mpProjectionRefTextEdit->clear();
        ui->mpGeoTransformCombox->clear();
    }
}
//
static QString formatFileSize(quint64 size)
{
    if (size < 1024 * 1024)
    {
        return QString::number((float)size / (1024.0), 'f', 2) + "K";
    }
    else if (size < 1024 * 1024 * 1024)
    {
        return QString::number((float)size / (1024.0 * 1024), 'f', 2) + "M";
    }
    return QString::number((float)size / (1024.0 * 1024 * 1024), 'f', 2) + "G";
}

void SEditor::fillImageFileInfo(GDALDataset* pGDALDataset)
{
    auto nLayerCount = pGDALDataset->GetLayerCount();
    auto pLayer = nLayerCount == 0 ? nullptr : pGDALDataset->GetLayer(0);
    auto strLayerName = pLayer != nullptr ? QString::fromUtf8(pLayer->GetName()) : "";
    ui->mpLayerNameLabel->setText(strLayerName);

    auto pGDALDriver = pGDALDataset->GetDriver();
    auto strType = pGDALDriver != nullptr ? QString::fromUtf8(pGDALDriver->GetDescription()) : "";
    ui->mpFileTypeLabel->setText(strType);

    QFileInfo fileInfo(pGDALDataset->GetFileList()[0]);
    ui->mpLastModifyTimeLabel->setText(fileInfo.lastModified().toString("ddd MMMM hh:mm:ss yyyy"));
    //
    ui->mpFileSizeLabel->setText(formatFileSize(fileInfo.size()));
    //
    ui->mpLayerCountLabel->setText(QString::number(nLayerCount));
}

void SEditor::fillImageLayerInfo(GDALDataset* pGDALDataset)
{
    //根据从GDALDataset读取到的实际信息填充 - 这里具体 都是代表啥意义 得你完善一下

    //比如 --下面这个我查了下是指图像像素值宽高
    ui->mpWidthLabel->setText(QString::number(pGDALDataset->GetRasterXSize()));
    ui->mpHeightLabel->setText(QString::number(pGDALDataset->GetRasterYSize()));
    //
    QString strBlockWidth, strBlockHeight, strDataType, strDataOrder;
    if (auto nRasterCount = pGDALDataset->GetRasterCount())
    {
        if (auto pRasterBand = pGDALDataset->GetRasterBand(1))
        {
            int nBlockWidth = 0, nBlockHeight = 0;
            pRasterBand->GetBlockSize(&nBlockWidth, &nBlockHeight);
            strBlockWidth = QString::number(nBlockWidth);
            strBlockHeight = QString::number(nBlockHeight);
            strDataType = GDALGetDataTypeName(pRasterBand->GetRasterDataType());
            strDataOrder = GDALGetColorInterpretationName(pRasterBand->GetColorInterpretation());
        }
    }
    ui->mpBlockWidthLabel->setText(strBlockWidth);
    ui->mpBlockHeightLabel->setText(strBlockHeight);
    ui->mpDataTypeLabel->setText(strDataType);
    ui->mpDataOrderLabel->setText(strDataOrder);
}
void SEditor::fillImageProjectionInfo(GDALDataset* pGDALDataset)
{
    //获取仿射矩阵信息
    double trans[6];
    pGDALDataset->GetGeoTransform(trans);
    for (size_t i = 0; i < sizeof(trans) / sizeof(trans[0]); i++)
    {
        ui->mpGeoTransformCombox->addItem(QString::number(trans[i], 'f', 2));
    }
    //获取投影信息
    auto strProjectionRef = QString::fromUtf8(pGDALDataset->GetProjectionRef());
    ui->mpProjectionRefTextEdit->setPlainText(strProjectionRef);
}
void SEditor::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event);
    emit closed();
    QMainWindow::closeEvent(event);
}

bool SEditor::eventFilter(QObject *watched, QEvent *event)
{
    //调色板大小变化
    if (watched == ui->mDockPalleteWidget
            && event->type() == QEvent::Resize)
    {
        //根据调色板新的大小，重新进行排布布局
        adjustPallete(static_cast<QResizeEvent*>(event)->size());
    }
    return QMainWindow::eventFilter(watched, event);
}

void SEditor::initDockPalleteWidget()
{
    //对调色板dock 添加网格布局
    auto pLayout = new QVBoxLayout(ui->mDockPalleteWidget);
    pLayout->setMargin(0);
    mpPaletteLayout = new QGridLayout();
    mpPaletteLayout->setVerticalSpacing(0);
    mpPaletteLayout->setHorizontalSpacing(0);
    pLayout->addLayout(mpPaletteLayout);
    pLayout->addStretch();
    //创建调色板上颜色
    qsrand(time(nullptr));
    for (auto i = 0; i < 100; i++)
    {
        QColor color(qrand() % 255, qrand() % 255, qrand() % 255);
        //关联颜色选中响应槽函数
        auto pColorArea = new SColorArea(color);
        QObject::connect(pColorArea, &QPushButton::clicked, this, &SEditor::onSelectColor);
        pColorArea->setMaximumWidth(30);
        mpPaletteButtons.push_back(pColorArea);
    }
    //添加事件过滤 监控 调色板大小变化
    ui->mDockPalleteWidget->installEventFilter(this);
    //把调色板颜色 放到网格布局中
    adjustPallete(QSize(300, 200));
}

void SEditor::adjustPallete(QSize size)
{
    //调色板大小变化，重新排布 - 先清空原布局
    while(mpPaletteLayout->count() != 0)
    {
        if (auto pLayerItem = mpPaletteLayout->takeAt(0))
        {
            delete pLayerItem;
        }
    }
    //根据宽度，计算每行能放置多少个颜色
    auto nColCount = (size.width() / 35);
    qDebug()<<"ColCount:"<<nColCount;
    auto nRowCount = (mpPaletteButtons.size() + nColCount - 1) % nColCount;
    for (auto i = 0; i < mpPaletteButtons.size(); i++)
    {
        auto nCol = i % nColCount;
        auto nRow = i / nColCount;
        //把颜色控件 放到网格布局中
        mpPaletteLayout->addWidget(mpPaletteButtons[i], nRow, nCol);
    }
}

void SEditor::_initialize()
{
    /*-----初始化状态栏-----*/
    //画布缩放等级
    mpStatLblCanvasScale = new QLabel(ui->mStatusBar);
    //鼠标逻辑位置
    mpStatLblCursorPos = new QLabel(ui->mStatusBar);
    //影像的GSD
    mpStatLblGSD = new QLabel(ui->mStatusBar);
    //投影坐标
    mpStatLblProjCS = new QLabel(ui->mStatusBar);

    ui->mStatusBar->addPermanentWidget(mpStatLblProjCS);
    ui->mStatusBar->addPermanentWidget(mpStatLblGSD);
    ui->mStatusBar->addPermanentWidget(mpStatLblCursorPos);
    ui->mStatusBar->addPermanentWidget(mpStatLblCanvasScale);

    /*-----初始化子窗口-----*/

    /*-----创建绘图区-----*/
    createWorkspace();

    _initializeConnections();

    emit initComplete();
}

void SEditor::_initializeConnections()
{
    connect(ui->mActionZoomin, &QAction::triggered, this, &SEditor::onActionZoominTriggered);
    connect(ui->mActionZoomout, &QAction::triggered, this, &SEditor::onActionZoomoutTriggered);
    connect(ui->mActionCreateRect, &QAction::triggered, this, &SEditor::onActionCreateRectTriggered);

    //-----测试用链接-----//
    connect(ui->mActionLoadImage, &QAction::triggered, this, &SEditor::onActionLoadImageTriggered);
    connect(ui->mActionLoadFragments, &QAction::triggered, this, &SEditor::onActionLoadFragmentsTriggered);
    connect(ui->mActionReportLeaks, &QAction::triggered, this, &SEditor::onActionReportLeaksTriggered);
    //-----测试用链接-----//

    connect(ui->mActionImportRaster, &QAction::triggered, this, &SEditor::onActionImportRasterTriggered);
    connect(ui->mActionBringForward, &QAction::triggered, this, &SEditor::onActionBringForwardTriggered);
    connect(ui->mActionSendBackward, &QAction::triggered, this, &SEditor::onActionSendBackwardTriggered);
    connect(ui->mActionBringtoFront, &QAction::triggered, this, &SEditor::onActionBringtoFrontTriggered);
    connect(ui->mActionSendtoBack, &QAction::triggered, this, &SEditor::onActionSendtoBackTriggered);

    connect(ui->mActionAlignLeft, &QAction::triggered, this, &SEditor::onActionAlignLeftTriggered);
    connect(ui->mActionAlignMiddle, &QAction::triggered, this, &SEditor::onActionAlignMiddleTriggered);
    connect(ui->mActionAlignRight, &QAction::triggered, this, &SEditor::onActionAlignRightTriggered);
    connect(ui->mActionAlignTop, &QAction::triggered, this, &SEditor::onActionAlignTopTriggered);
    connect(ui->mActionAlignCenter, &QAction::triggered, this, &SEditor::onActionAlignCenterTriggered);
    connect(ui->mActionAlignBottom, &QAction::triggered, this, &SEditor::onActionAlignBottomTriggered);

    connect(ui->mActionDistributeHorizentally, &QAction::triggered, this, &SEditor::onActionDistributeHorizentallyTriggered);
    connect(ui->mActionDistributeVertically, &QAction::triggered, this, &SEditor::onActionDistributeVerticallyTriggered);

    if (mpImportDialog) _connectDataImportWizard();
}

void SEditor::_connectDataImportWizard()
{
    //数据导入向导对话框事件响应
    connect(this->mpImportDialog, &QDataImportWizard::importingData, this, &SEditor::onImportingRaster);

    //绑定 logger日志 添加信号
    QObject::connect(SLogger::getLogger(), &SLogger::signalAddEntry, this, &SEditor::onOutput);
}

void SEditor::createWorkspace(const QSize& CanvasSize)
{
    //创建新的绘图区控件,保存绘图区控件指针
    QCanvasArea* newCanvasArea = new QCanvasArea(CanvasSize);
    mpCanvasAreaVec.push_back(newCanvasArea);
    //创建新的工作区文档,保存文档指针
    std::shared_ptr<SDocument> newDocument = std::make_shared<SDocument>(newCanvasArea->canvas());
    this->mpDocVec.push_back(newDocument);
    //添加Tab页面并激活
    //TODO:解决重名问题
    int newIdx = ui->mTabWidget->addTab(newCanvasArea, tr("Untitled Workspace"));
    ui->mTabWidget->setCurrentIndex(newIdx);
    //设置为当前激活的绘图区
    mpCurCanvasArea = newCanvasArea;
    mpCurDoc = newDocument;
    //链接绘图区信息显示
    connect(mpCurCanvasArea->canvas(), &QCanvas::mouseMoved, this, &SEditor::onCanvasMouseMoved);
    connect(mpCurCanvasArea->canvas(), &QCanvas::scaled, this, &SEditor::onCanvasScaled);
    connect(&mpCurDoc->getLayerManager(), &SLayerManager::layersUpdated, this, &SEditor::onLayersUpdated);
    onCanvasScaled(1);
    //设置图层视图
    ui->mLayerView->setDocument(mpCurDoc);
}
