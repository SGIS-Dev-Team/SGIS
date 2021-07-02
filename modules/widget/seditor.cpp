#include "seditor.h"
#include "ui_seditor.h"
#include <modules/paint/sshapefactory.h>
#include <QDir>
#include <QMessageBox>


SEditor::SEditor(QWidget* parent): QMainWindow(parent),
    ui(new Ui::SEditor)
{
    ui->setupUi(this);
    initialize();

}

SEditor::~SEditor()
{
    //TODO:释放所有手动创建的控件指针
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
void SEditor::onActionLoadHugeImageTriggered()
{
    if (mpImportDialog->isEmpty())
    {
        //获取要读取的文件路径
        QStringList strImagePathList = QFileDialog::getOpenFileNames(this, tr("Open Huge Image"), "", "raster (*.tif *.tiff)");
        if (strImagePathList.isEmpty())
            return;

        mpImportDialog->addImagePaths(strImagePathList);
    }
    //打开数据导入向导对话框
    mpImportDialog->open();
}

void SEditor::onImportingData()
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
            mpCurDoc->getCoordinate().geo2logic(geoTrans[0] + geoTrans[1] * nWidth / 2, geoTrans[3] + geoTrans[5] * nHeight / 2, logicX, logicY);
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
        mpCurDoc->getCoordinate().logic2geo(Log_pos.x(), Log_pos.y(), x, y);
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
}

void SEditor::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event);
    emit closed();
}

void SEditor::initialize()
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
    //数据导入向导
    mpImportDialog = new QDataImportWizard(this);

    /*-----创建绘图区-----*/
    createWorkspace();

    initializeConnections();
}

void SEditor::initializeConnections()
{
    connect(ui->mActionZoomin, &QAction::triggered, this, &SEditor::onActionZoominTriggered);
    connect(ui->mActionZoomout, &QAction::triggered, this, &SEditor::onActionZoomoutTriggered);
    connect(ui->mActionCreateRect, &QAction::triggered, this, &SEditor::onActionCreateRectTriggered);

    //-----测试用链接-----//
    connect(ui->mActionLoadImage, &QAction::triggered, this, &SEditor::onActionLoadImageTriggered);
    connect(ui->mActionLoadFragments, &QAction::triggered, this, &SEditor::onActionLoadFragmentsTriggered);
    connect(ui->mActionLoadHugeImage, &QAction::triggered, this, &SEditor::onActionLoadHugeImageTriggered);
    connect(ui->mActionReportLeaks, &QAction::triggered, this, &SEditor::onActionReportLeaksTriggered);
    //-----测试用链接-----//

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
    //数据导入向导对话框事件响应
    connect(this->mpImportDialog, &QDataImportWizard::importingData, this, &SEditor::onImportingData);
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
