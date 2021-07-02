#include "slayerinfowidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "gdal_priv.h" 
#include "ogrsf_frmts.h"
LayerInfoWidget::LayerInfoWidget(QWidget *parent)
	: QWidget(parent)
{
	auto pVLayout = new QVBoxLayout(this);
	
	//
	auto pLineLayout = new QHBoxLayout();
	pLineLayout->addWidget(new QLabel("Width:"));
	pLineLayout->addWidget(mpWidthLabel = new QLabel(""));
	pLineLayout->addSpacing(10);
	pLineLayout->addWidget(new QLabel("Height:"));
	pLineLayout->addWidget(mpHeightLabel = new QLabel(""));
	pLineLayout->addSpacing(10);
	pLineLayout->addWidget(new QLabel("Type:"));
	pLineLayout->addWidget(mpTypeLabel = new QLabel(""));
	pVLayout->addLayout(pLineLayout);


	//
	pLineLayout = new QHBoxLayout();
	pLineLayout->addWidget(new QLabel("Block Width:"));
	pLineLayout->addWidget(mpBlockWidthLabel = new QLabel(""));
	pLineLayout->addSpacing(10);
	pLineLayout->addWidget(new QLabel("Block Height:"));
	pLineLayout->addWidget(mpBlockHeightLabel = new QLabel(""));
	pLineLayout->addSpacing(10);
	pLineLayout->addWidget(new QLabel("Data Type:"));
	pLineLayout->addWidget(mpDataTypeLabel = new QLabel(""));
	pVLayout->addLayout(pLineLayout);

	//
	pLineLayout = new QHBoxLayout();
	pLineLayout->addWidget(new QLabel("Compression:"));
	pLineLayout->addWidget(mpCompressionLabel = new QLabel(""));
	pLineLayout->addSpacing(10);
	pLineLayout->addWidget(new QLabel("Data Order:"));
	pLineLayout->addWidget(mpDataOrderLabel = new QLabel(""));
	pVLayout->addLayout(pLineLayout);

	//
	pLineLayout = new QHBoxLayout();
	pLineLayout->addWidget(new QLabel("Pyramid Layer Algorithm:"));
	pLineLayout->addWidget(mpPyramidLayerAlgorithmLabel = new QLabel());
	pVLayout->addLayout(pLineLayout);
	
	//
	pVLayout->addStretch();
	//
	clear();
}

LayerInfoWidget::~LayerInfoWidget()
{

}

void LayerInfoWidget::fill(GDALDataset* pGDALDataset)
{
	//根据从GDALDataset读取到的实际信息填充 - 这里具体 都是代表啥意义 得你完善一下

	//比如 --下面这个我查了下是指图像像素值宽高
	mpWidthLabel->setText(QString::number(pGDALDataset->GetRasterXSize()));
	mpHeightLabel->setText(QString::number(pGDALDataset->GetRasterYSize()));
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
	mpBlockWidthLabel->setText(strBlockWidth);
	mpBlockHeightLabel->setText(strBlockHeight);
	mpDataTypeLabel->setText(strDataType);
	mpDataOrderLabel->setText(strDataOrder);
}
void LayerInfoWidget::clear()
{
	mpWidthLabel->clear();
	mpHeightLabel->clear();
	mpTypeLabel->clear();
	//
	mpBlockWidthLabel->clear();
	mpBlockHeightLabel->clear();
	mpDataTypeLabel->clear();

	//
	mpCompressionLabel->clear();
	mpDataOrderLabel->clear();
	//
	mpPyramidLayerAlgorithmLabel->clear();
}
