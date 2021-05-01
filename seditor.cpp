#include "seditor.h"
#include "ui_seditor.h"

SEditor::SEditor(SLogger* _logger, QWidget *parent) :
    QMainWindow(parent), gbLogger(_logger),
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

void SEditor::onTabSwitched()
{

}

void SEditor::onCanvasMouseMoved(QPoint Log_pos)
{
    mpStatLblCursorPos->setText("(" + QString::number(Log_pos.x()) + "," + QString::number(Log_pos.y()) + ")");
}

void SEditor::onCanvasScaled(double value)
{
    mpStatLblCanvasScale->setText(QString::number(value * 100, 'f', 2) + "%");
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
    createTemplate();

    connect(ui->mActionZoomin, &QAction::triggered, this, &SEditor::onActionZoominTriggered);
    connect(ui->mActionZoomout, &QAction::triggered, this, &SEditor::onActionZoomoutTriggered);

}

void SEditor::createTemplate(const QSize &CanvasSize)
{
    //创建新的绘图区控件
    QCanvasArea* newCanvasArea = new QCanvasArea(CanvasSize);
    //保存绘图区控件指针
    mpCanvasAreaVec.push_back(newCanvasArea);
    //添加Tab页面并激活
    //TODO:解决重名问题
    int newIdx = ui->mTabWidget->addTab(newCanvasArea, tr("Untitled Template"));
    ui->mTabWidget->setCurrentIndex(newIdx);
    //设置为当前激活的绘图区
    mpCurCanvasArea = newCanvasArea;
    //链接绘图区信息显示
    connect(mpCurCanvasArea->canvas(), &QCanvas::mouseMoved, this, &SEditor::onCanvasMouseMoved);
    connect(mpCurCanvasArea->canvas(), &QCanvas::scaled, this, &SEditor::onCanvasScaled);
    onCanvasScaled(1);
}

void SEditor::createTemplate(const QString &templateFilePath)
{
}
