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
    pGridLayout->addWidget(mpLabelEndPoint = new QLabel(tr("End Point")), 1, 0);
    pGridLayout->addWidget(mpLabelJoint = new QLabel(tr("Joint")), 2, 0);
    pGridLayout->addWidget(mpLabelWidth = new QLabel(tr("Width")), 3, 0);
    pGridLayout->addWidget(mpLabelFill = new QLabel(tr("Fill")), 4, 0);

    pGridLayout->addWidget(mpComboLineType = new QComboBox(), 0, 1);
    pGridLayout->addWidget(mpComboCapStyle = new QComboBox(), 1, 1);
    pGridLayout->addWidget(mpComboJoint = new QComboBox(), 2, 1);
    pGridLayout->addWidget(mpSpinBoxWidth = new QSpinBox(), 3, 1);
    pGridLayout->addWidget(mpComboFill = new QComboBox(), 4, 1);

    //标签的对齐方式


    //添加样式选择部分
    //线型
    mpComboLineType->addItem(QString(tr("SolidLine")), static_cast<int>(Qt::SolidLine));
    mpComboLineType->addItem(QString(tr("DashLine")), static_cast<int>(Qt::DashLine));
    mpComboLineType->addItem(QString(tr("DotLine")), static_cast<int>(Qt::DotLine));
    mpComboLineType->addItem(QString(tr("DashDotLine")), static_cast<int>(Qt::DashDotLine));
    mpComboLineType->addItem(QString(tr("DashDotDotLine")), static_cast<int>(Qt::DashDotDotLine));
    mpComboLineType->addItem(QString(tr("CustomDashLine")), static_cast<int>(Qt::CustomDashLine));

    //填充
    mpComboFill->addItem(QString(tr("SolidPattern")), static_cast<int>(Qt::SolidPattern));
    mpComboFill->addItem(QString(tr("Dense2Pattern")), static_cast<int>(Qt::Dense2Pattern));
    mpComboFill->addItem(QString(tr("BDiagPattern")), static_cast<int>(Qt::BDiagPattern));
    mpComboFill->addItem(QString(tr("FDiagPattern")), static_cast<int>(Qt::FDiagPattern));
    mpComboFill->addItem(QString(tr("CrossPattern")), static_cast<int>(Qt::CrossPattern));
    mpComboFill->addItem(QString(tr("DiagCrossPattern")), static_cast<int>(Qt::DiagCrossPattern));

    //端点
    mpComboCapStyle->addItem(QString(tr("FlatCap")), static_cast<int>(Qt::FlatCap));
    mpComboCapStyle->addItem(QString(tr("SquareCap")), static_cast<int>(Qt::SquareCap));
    mpComboCapStyle->addItem(QString(tr("RoundCap")), static_cast<int>(Qt::RoundCap));

    //连接点
    mpComboJoint->addItem(QString(tr("MiterJoin")), static_cast<int>(Qt::MiterJoin));
    mpComboJoint->addItem(QString(tr("BevelJoin")), static_cast<int>(Qt::BevelJoin));
    mpComboJoint->addItem(QString(tr("RoundJoin")), static_cast<int>(Qt::RoundJoin));

    //线宽
    mpSpinBoxWidth->setMaximum(10);
    mpSpinBoxWidth->setMinimum(-1);
    mpSpinBoxWidth->setValue(3);

    //全部不可选
    mpComboLineType->setEnabled(false);
    mpComboFill->setEnabled(false);
    mpComboCapStyle->setEnabled(false);
    mpComboJoint->setEnabled(false);
    mpSpinBoxWidth->setEnabled(false);

    _initializeConnections();


}


StyleWidget::~StyleWidget()
{

}

void StyleWidget::_initializeConnections()
{
    connect(mpComboLineType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StyleWidget::changeComeUp);
    connect(mpComboFill, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StyleWidget::changeComeUp);
    connect(mpComboCapStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StyleWidget::changeComeUp);
    connect(mpComboJoint, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StyleWidget::changeComeUp);

    connect(mpSpinBoxWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, &StyleWidget::onSpinBoxValueChanged);
}

void StyleWidget::_disconnect()
{

}

void StyleWidget::onSpinBoxValueChanged(int value)
{
    if (value <= 0)
        mpSpinBoxWidth->setValue(1);
    else
        changeComeUp();
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
    mpComboCapStyle->setEnabled(false);
    mpComboJoint->setEnabled(false);
    mpSpinBoxWidth->setEnabled(false);
}

void StyleWidget::changeToLineShape()
{
    //fill不可选
    mpComboLineType->setEnabled(true);
    mpComboFill->setEnabled(false);
    mpComboCapStyle->setEnabled(true);
    mpComboJoint->setEnabled(true);
    mpSpinBoxWidth->setEnabled(true);
}

void StyleWidget::changeToPolygonShape()
{
    //全部可选
    mpComboLineType->setEnabled(true);
    mpComboFill->setEnabled(true);
    mpComboCapStyle->setEnabled(true);
    mpComboJoint->setEnabled(true);
    mpSpinBoxWidth->setEnabled(true);
}

void StyleWidget::changeToRaster()
{
    //全部不可选

    mpComboLineType->setEnabled(false);
    mpComboFill->setEnabled(false);
    mpComboCapStyle->setEnabled(false);
    mpComboJoint->setEnabled(false);
    mpSpinBoxWidth->setEnabled(true);
}

void StyleWidget::setLayerPen(QPen& pen)
{
    if (!mpComboLineType->isEnabled())
        return;
    if (mpComboLineType->currentIndex() != -1)
    {
        pen.setStyle(static_cast<Qt::PenStyle>((mpComboLineType->currentData().toInt())));
    }
    if (mpComboCapStyle->currentIndex() != -1)
    {
        pen.setCapStyle(static_cast<Qt::PenCapStyle>((mpComboCapStyle->currentData().toInt())));
    }
    if (mpComboJoint->currentIndex() != -1)
    {
        pen.setJoinStyle(static_cast<Qt::PenJoinStyle>((mpComboJoint->currentData().toInt())));
    }
    if (mpSpinBoxWidth->value() != -1)
    {
        pen.setWidth(mpSpinBoxWidth->value());
    }
}

void StyleWidget::setLayerBrush(QBrush& brush)
{
    if (!mpComboFill->isEnabled())
        return;
    if (mpComboFill->currentIndex() != -1)
    {
        brush.setStyle(static_cast<Qt::BrushStyle>(mpComboFill->currentData().toInt()));
    }
}

void StyleWidget::setStylePen(QPen& pen)
{
    int index = -1;
    if (!mpComboLineType->isEnabled())
    {
        mpComboLineType->setEnabled(true);
        mpComboCapStyle->setEnabled(true);
        mpComboJoint->setEnabled(true);
        mpSpinBoxWidth->setEnabled(true);

        index = mpComboLineType->findData(static_cast<int>(pen.style()));
        mpComboLineType->setCurrentIndex(index);
        index = mpComboCapStyle->findData(static_cast<int>(pen.capStyle()));
        mpComboCapStyle->setCurrentIndex(index);
        index = mpComboJoint->findData(static_cast<int>(pen.joinStyle()));
        mpComboJoint->setCurrentIndex(index);
        mpSpinBoxWidth->setValue(pen.width());
    }
    else
    {
        if (mpComboLineType->currentIndex() == -1)
        {}
        else if (static_cast<Qt::PenStyle>(mpComboLineType->currentData().toInt()) != pen.style())
        {
            mpComboLineType->setCurrentIndex(-1);
        }
        if (mpComboCapStyle->currentIndex() == -1)
        {}
        else if (static_cast<Qt::PenCapStyle>(mpComboCapStyle->currentData().toInt()) != pen.capStyle())
        {
            mpComboCapStyle->setCurrentIndex(-1);
        }
        if (mpComboJoint->currentIndex() == -1)
        {}
        else if (static_cast<Qt::PenJoinStyle>(mpComboJoint->currentData().toInt()) != pen.joinStyle())
        {
            mpComboJoint->setCurrentIndex(-1);
        }
        if (mpSpinBoxWidth->value() == -1)
        {}
        else if (mpSpinBoxWidth->value() != pen.width())
        {
            mpSpinBoxWidth->setValue(-1);
        }
    }
}

void StyleWidget::setStyleBrush(QBrush& brush)
{
    int index = -1;
    if (!mpComboFill->isEnabled())
    {
        mpComboFill->setEnabled(true);

        index = mpComboFill->findData(static_cast<int>(brush.style()));
        mpComboFill->setCurrentIndex(index);
    }
    else if (mpComboFill->currentIndex() == -1)
    {
        return;
    }
    else if (static_cast<Qt::BrushStyle>(mpComboFill->currentData().toInt()) != brush.style())
    {
        mpComboFill->setCurrentIndex(-1);
    }
}

void StyleWidget::onSelectStateChanged()
{
    //程序进行的修改
    mbRealChangeFlag = false;


    //从layermanager中获取图层信息
    if (mpLayerManager == nullptr)
    {
        return;
    }
    const std::list<list_iterator>Iterlist = mpLayerManager->getSelectedLayerIterList();
    list_iterator tempIter;
    SObject* pSObj;
    changeToRaster();
    for (auto begin = Iterlist.cbegin(); begin != Iterlist.cend(); begin++)
    {
        //访问图层
        pSObj =  **begin;
        if (pSObj->getType() == PointFeature)
        {
            setStyleBrush(pSObj->getBrush());
        }
        if (pSObj->getType() == LineStringFeature)
        {
            setStylePen(pSObj->getPen());
        }
        if (pSObj->getType() == PolygonFeature)
        {
            setStylePen(pSObj->getPen());
            setStyleBrush(pSObj->getBrush());
        }
    }

    //程序修改结束
    mbRealChangeFlag = true;

}


void StyleWidget::changeComeUp()
{
    if (!mbRealChangeFlag)
        return;
    //修改选中图层的样式
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
        //访问图层
        pSObj =  **begin;
        if (pSObj->getType() == PointFeature)
        {
            setLayerBrush(pSObj->rBrush());
        }
        if (pSObj->getType() == LineStringFeature)
        {
            setLayerPen(pSObj->rPen());
        }
        if (pSObj->getType() == PolygonFeature)
        {
            setLayerPen(pSObj->rPen());
            setLayerBrush(pSObj->rBrush());
        }
    }


    emit StyleWidget::styleChanged();
}


