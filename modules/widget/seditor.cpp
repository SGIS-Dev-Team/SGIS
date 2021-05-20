﻿#include "seditor.h"
#include "ui_seditor.h"
#include <modules/paint/sshapefactory.h>
#include <QDir>

SEditor::SEditor(QWidget *parent): QMainWindow(parent),
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

    pFragImg->setFragmentPath(path, file_info.completeBaseName());
    pFragImg->setCenterPoint(QPointF( (DEFAULT_CANVAS_SIZE / 2).width(), (DEFAULT_CANVAS_SIZE / 2).height()));
    mpCurDoc->getLayerManager().addLayer(pFragImg);

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

void SEditor::closeEvent(QCloseEvent *event)
{
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
    connect(ui->mActionLoadImage, &QAction::triggered, this, &SEditor::onActionLoadImageTriggered);
    connect(ui->mActionLoadFragments, &QAction::triggered, this, &SEditor::onActionLoadFragmentsTriggered);

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
    QCanvasArea* newCanvasArea = new QCanvasArea(CanvasSize);
    mpCanvasAreaVec.push_back(newCanvasArea);
    //创建新的工作区文档,保存文档指针
    SDocument* newDocument = new SDocument(newCanvasArea->canvas());
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
