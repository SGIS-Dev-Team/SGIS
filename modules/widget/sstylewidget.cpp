#include "sstylewidget.h"
#include <QPainter>


StyleWidget::StyleWidget(QWidget* parent) : QWidget(parent)
{
    //布局代码
    auto pLayout = new QVBoxLayout(this);
    pLayout->setMargin(0);
    auto pScrollArea = new QScrollArea();
    pLayout->addWidget(pScrollArea);

    auto pGridLayout = new QGridLayout(pScrollArea);
    pGridLayout->setMargin(1);

    pGridLayout->addWidget(mpLabelLineType = new QLabel(tr("Line Type")), 0, 0);
    pGridLayout->addWidget(mpLabelFill = new QLabel(tr("Fill")), 1, 0);
    pGridLayout->addWidget(mpLabelEndPoint = new QLabel(tr("End Point")), 2, 0);
    pGridLayout->addWidget(mpLabelJoint = new QLabel(tr("Joint")), 3, 0);
    pGridLayout->addWidget(mpLabelWidth = new QLabel(tr("Width")), 4, 0);

    pGridLayout->addWidget(mpComboLineType = new QComboBox(), 0, 1);
    pGridLayout->addWidget(mpComboFill = new QComboBox(), 1, 1);
    pGridLayout->addWidget(mpComboEndPoint = new QComboBox(), 2, 1);
    pGridLayout->addWidget(mpComboJoint = new QComboBox(), 3, 1);
    pGridLayout->addWidget(mpSpinBoxWidth = new QSpinBox(), 4, 1);

    //标签的对齐方式


    //添加样式选择部分
    //线型
    mpComboLineType->addItem(QString(tr("SolidLine")), int(Qt::SolidLine));
    mpComboLineType->addItem(QString(tr("DashLine")), int(Qt::DashLine));
    mpComboLineType->addItem(QString(tr("DotLine")), int(Qt::DotLine));
    mpComboLineType->addItem(QString(tr("DashDotLine")), int(Qt::DashDotLine));
    mpComboLineType->addItem(QString(tr("DashDotDotLine")), int(Qt::DashDotDotLine));
    mpComboLineType->addItem(QString(tr("CustomDashLine")), int(Qt::CustomDashLine));
    mpComboLineType->addItem(QString(), QVariant());
    //填充
    mpComboFill->addItem(QString(tr("SolidPattern")), int(Qt::SolidLine));
    mpComboFill->addItem(QString(tr("Dense2Pattern")), int(Qt::Dense2Pattern));
    mpComboFill->addItem(QString(tr("BDiagPattern")), int(Qt::BDiagPattern));
    mpComboFill->addItem(QString(tr("FDiagPattern")), int(Qt::FDiagPattern));
    mpComboFill->addItem(QString(tr("CrossPattern")), int(Qt::CrossPattern));
    mpComboFill->addItem(QString(tr("DiagCrossPattern")), int(Qt::DiagCrossPattern));
    mpComboFill->addItem(QString(), QVariant());
    //端点
    mpComboEndPoint->addItem(QString(tr("FlatCap")), int(Qt::FlatCap));
    mpComboEndPoint->addItem(QString(tr("SquareCap")), int(Qt::SquareCap));
    mpComboEndPoint->addItem(QString(tr("RoundCap")), int(Qt::RoundCap));
    mpComboEndPoint->addItem(QString(), QVariant());
    //连接点
    mpComboJoint->addItem(QString(tr("MiterJoin")), int(Qt::MiterJoin));
    mpComboJoint->addItem(QString(tr("BevelJoin")), int(Qt::BevelJoin));
    mpComboJoint->addItem(QString(tr("RoundJoin")), int(Qt::RoundJoin));
    mpComboJoint->addItem(QString(), QVariant());
    //线宽
    mpSpinBoxWidth->setMaximum(10);
    mpSpinBoxWidth->setMinimum(1);
    mpSpinBoxWidth->setValue(3);

    //全部不可选
    mpComboLineType->setEnabled(false);
    mpComboFill->setEnabled(false);
    mpComboEndPoint->setEnabled(false);
    mpComboJoint->setEnabled(false);
    mpSpinBoxWidth->setEnabled(false);

}

void StyleWidget::_initializeConnections()
{

}

void StyleWidget::_disconnect()
{

}

StyleWidget::~StyleWidget()
{

}

const SLayerManager* StyleWidget::layerManager() const
{
    return mpLayerManager;
}

void StyleWidget::SetLayerManager(SLayerManager* layerManager)
{
    mpLayerManager = layerManager;
}

void StyleWidget::changeToPointShape()
{
    //只有fill可选
    mpComboLineType->setEnabled(false);
    mpComboFill->setEnabled(true);
    mpComboEndPoint->setEnabled(false);
    mpComboJoint->setEnabled(false);
    mpSpinBoxWidth->setEnabled(false);
}

void StyleWidget::changeToLineShape()
{
    //fill不可选
    mpComboLineType->setEnabled(true);
    mpComboFill->setEnabled(false);
    mpComboEndPoint->setEnabled(true);
    mpComboJoint->setEnabled(true);
    mpSpinBoxWidth->setEnabled(true);
}

void StyleWidget::changeToPolygonShape()
{
    //全部可选
    mpComboLineType->setEnabled(true);
    mpComboFill->setEnabled(true);
    mpComboEndPoint->setEnabled(true);
    mpComboJoint->setEnabled(true);
    mpSpinBoxWidth->setEnabled(true);
}

void StyleWidget::changeToRaster()
{
    //全部不可选
    mpComboLineType->setEnabled(false);
    mpComboFill->setEnabled(false);
    mpComboEndPoint->setEnabled(false);
    mpComboJoint->setEnabled(false);
    mpSpinBoxWidth->setEnabled(false);
}

void StyleWidget::onSelectStateChanged()
{
    //从layermanager中获取图层信息
    if (mpLayerManager == nullptr)
    {
        return;
    }
    const std::list<list_iterator>Iterlist = mpLayerManager->getSelectedLayerIterList();
    list_iterator tempIter;
    SObject* pSObj;
    for (auto begin = Iterlist.cbegin(); begin != Iterlist.cend(); begin++)
    {
        tempIter = *begin;
        //访问图层
        pSObj =  *tempIter;
        if (pSObj->getType() == Raster)
        {
            changeToRaster();
        }
    }

}

