#include "seditor.h"
#include "ui_seditor.h"
#include <modules/paint/sshapefactory.h>
#include <QDir>
#include <QMessageBox>
#include <QListWidgetItem>
#include <vld.h>
#include "../paint/simage.h"
#include "simageinfowidget.h"
SEditor::SEditor(QWidget *parent): QMainWindow(parent),
    ui(new Ui::SEditor)
{
    ui->setupUi(this);
	initCustomDock();
	initialize();
	showMaximized();
	//根据调用函数时发生错误的不同类型，修改字符串
	SLogger::getLogger()->addEntry("xxx", SLogger::LocalError, "Loading meta : meta text not exists.");
	SLogger::getLogger()->addEntry("YYY", SLogger::LocalError, "Loading meta : meta text not exists.");
	SLogger::getLogger()->addEntry("AAA", SLogger::LocalError, "Loading meta : meta text not exists.");
	SLogger::getLogger()->addEntry("BBB", SLogger::LocalError, "Loading meta : meta text not exists.");
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

    SShape * rect = SShapeFactory::createShape(ShapeSet::Hexagon);
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

    for(int i = 0; i < pathList.size(); ++i)
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

    if(path.isEmpty())
        return;

    SFragImage* pFragImg = new SFragImage(mpCurDoc->getFragLoader());

    pFragImg->setFragmentPath(file_info.filePath());
    pFragImg->loadMeta();

    pFragImg->setCenterPoint(QPointF( (DEFAULT_CANVAS_SIZE / 2).width(), (DEFAULT_CANVAS_SIZE / 2).height()));

    pFragImg->setBandIndices(1, 2, 3);

    pFragImg->setHoldTopPyramidEnabled(true);

    mpCurDoc->getLayerManager().addLayer(pFragImg);
}

#include "qbandselectdialog.h"
void SEditor::onActionLoadHugeImageTriggered()
{
    //获取要读取的文件路径
    QString strImagePath = QFileDialog::getOpenFileName(this, tr("Open Huge Image"), "", "raster (*.tif *.tiff)");
    if(strImagePath.isEmpty())
        return;

    //打开波段预览对话框
    QBandSelectDialog dialog(strImagePath, this);
    dialog.exec();

    if(dialog.getPyramidDirPath().isEmpty())
    {
        QMessageBox::critical(this, tr("File Invalid or Cancelled by user."), tr(""));
        return;
    }

    //选择完成：将影像读入
    mpCurDoc->getLayerManager().clearSelection();

    SFragImage* pFragImg = new SFragImage(mpCurDoc->getFragLoader());

    QFileInfo file_info(dialog.getPyramidDirPath());
    pFragImg->setFragmentPath(file_info.filePath());
    pFragImg->loadMeta();

    pFragImg->setCenterPoint(QPointF((DEFAULT_CANVAS_SIZE / 2).width(), (DEFAULT_CANVAS_SIZE / 2).height()));
    pFragImg->setHoldTopPyramidEnabled(true);
    pFragImg->setBandIndices(dialog.getRedBandIdx(), dialog.getGreenBandIdx(), dialog.getBlueBandIdx());

    mpCurDoc->getLayerManager().addLayer(pFragImg);
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
    mpStatLblCursorPos->setText("(" + QString::number(Log_pos.x()) + "," + QString::number(Log_pos.y()) + ")");
}

void SEditor::onCanvasScaled(double value)
{
    mpStatLblCanvasScale->setText(QString::number(value * 100, 'f', 2) + "%");
}

void SEditor::onLayersUpdated(SLayerManager *which)
{
    if(mpCurCanvasArea)
        mpCurCanvasArea->canvas()->repaint();
}

void SEditor::onOutput(const QString& entry)
{
	//添加logger到输出窗口
	mpOutputListWidget->insertItem(-1, entry);
}

void SEditor::updateTiffLayoutInfo(SLayerManager* which)
{
	//更新Tiff图片信息
	//获取当前选择的图层 -- 后续可能需要随着实现更改

	QString strSelectImageFilePath;
	//遍历当前选中图层，选择第一个图片图层
	auto lstSelectLayout = which->getSelectedLayerIterList();
	for (auto it : lstSelectLayout)
	{
		if (auto pImage = dynamic_cast<SImage*>(*it))
		{
			strSelectImageFilePath = pImage->getImagePath();
			break;
		}
	}
	mpImageInfoWidget->fillByImageFilePath(strSelectImageFilePath);
}

void SEditor::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit closed();
}

void SEditor::initialize()
{
    /*-----初始化状态栏-----*/
    mpStatLblCanvasScale = new QLabel(ui->mStatusBar);
    mpStatLblCursorPos = new QLabel(ui->mStatusBar);

    ui->mStatusBar->addPermanentWidget(mpStatLblCursorPos);
    ui->mStatusBar->addPermanentWidget(mpStatLblCanvasScale);

    /*-----创建绘图区-----*/
    createWorkspace();

    initializeConnections();
}

void SEditor::initializeConnections()
{
    connect(ui->mActionZoomin, &QAction::triggered, this, &SEditor::onActionZoominTriggered);
    connect(ui->mActionZoomout, &QAction::triggered, this, &SEditor::onActionZoomoutTriggered);
    connect(ui->mActionCreateRect, &QAction::triggered, this, &SEditor::onActionCreateRectTriggered);

    //[测试用链接]
    connect(ui->mActionLoadImage, &QAction::triggered, this, &SEditor::onActionLoadImageTriggered);
    connect(ui->mActionLoadFragments, &QAction::triggered, this, &SEditor::onActionLoadFragmentsTriggered);
    connect(ui->mActionLoadHugeImage, &QAction::triggered, this, &SEditor::onActionLoadHugeImageTriggered);
    connect(ui->mActionReportLeaks, &QAction::triggered, this, &SEditor::onActionReportLeaksTriggered);

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


}

void SEditor::createWorkspace(const QSize &CanvasSize)
{
    //创建新的绘图区控件,保存绘图区控件指针
    std::shared_ptr<QCanvasArea> newCanvasArea = std::make_shared<QCanvasArea>(CanvasSize);
    mpCanvasAreaVec.push_back(newCanvasArea);
    //创建新的工作区文档,保存文档指针
    std::shared_ptr<SDocument> newDocument = std::make_shared<SDocument>(newCanvasArea->canvas());
    this->mpDocVec.push_back(newDocument);
    //添加Tab页面并激活
    //TODO:解决重名问题
    int newIdx = ui->mTabWidget->addTab(newCanvasArea.get(), tr("Untitled Workspace"));
    ui->mTabWidget->setCurrentIndex(newIdx);
    //设置为当前激活的绘图区
    mpCurCanvasArea = newCanvasArea;
    mpCurDoc = newDocument;
    //链接绘图区信息显示
    connect(mpCurCanvasArea->canvas().get(), &QCanvas::mouseMoved, this, &SEditor::onCanvasMouseMoved);
    connect(mpCurCanvasArea->canvas().get(), &QCanvas::scaled, this, &SEditor::onCanvasScaled);
	//SLayerManager 图层更新时会调用 SEditor::onLayersUpdated
	connect(&mpCurDoc->getLayerManager(), &SLayerManager::layersUpdated, this, &SEditor::onLayersUpdated);
	//更新图片信息- 但是为了更明确下，就独立出来了 - 后续有可能有更好的时机（添加更合适的信号）来更新图片信息，
	connect(&mpCurDoc->getLayerManager(), &SLayerManager::layersUpdated, this, &SEditor::updateTiffLayoutInfo);
    onCanvasScaled(1);
    //设置图层视图
    ui->mLayerView->setDocument(mpCurDoc);
}

void SEditor::initCustomDock()
{
	//创建tiff信息窗口
	auto pDock = new QDockWidget("ImageInfo");
	pDock->setWidget(mpImageInfoWidget = new ImageInfoWidget());
	addDockWidget(Qt::RightDockWidgetArea, pDock);

	//创建输出信息窗口
	pDock = new QDockWidget("Output");
	pDock->setWidget(mpOutputListWidget = new QListWidget());
	addDockWidget(Qt::RightDockWidgetArea, pDock);
	//绑定 logger日志 添加信号
	QObject::connect(SLogger::getLogger(), &SLogger::signalAddEntry,
		this, &SEditor::onOutput);
}
