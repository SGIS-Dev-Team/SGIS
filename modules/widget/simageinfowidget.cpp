#include "simageinfowidget.h"
#include <QEvent>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QScrollArea>
#include "gdal_priv.h" 

ImageInfoWidget::ImageInfoWidget(QWidget *parent)
	: QWidget(parent)
{
	//布局代码
	auto pLayout = new QVBoxLayout(this);
	pLayout->setMargin(0);
	auto pScrollArea = new QScrollArea();
	pLayout->addWidget(pScrollArea);
	//
	auto pWidget = new QWidget();
	pLayout = new QVBoxLayout(pWidget);
	pLayout->setMargin(0);
	pLayout->addWidget(mpTabWidget = new QTabWidget(this));

	/*添加具体的描述窗口
	File Info  - 文件信息
	Layer Info - 图层信息
	Projection Info - 投影信息
	*/
	mpTabWidget->addTab(mpFileInfoWidget = new FileInfoWidget(this), "File Info");
	mpTabWidget->addTab(mpLayerInfoWidget = new LayerInfoWidget(this), "Layer Info");
	mpTabWidget->addTab(mpProjectionInfoWidget = new ProjectionInfoWidget(this), "Projection Info");
	//other信息填充，后期根据实际需要完善
	//m_pTabWidget->addTab(m_pOtherInfoWidget = new OtherInfoWidget(this), "Other Info");
	//

	pScrollArea->setWidget(pWidget);
	pWidget->setMinimumSize(200, 100);

#if 0
	m_pTabWidget->setTabPosition(QTabWidget::West);
	QTabBar *pTabBar = m_pTabWidget->tabBar();
	//
	m_pTabWidget->addTab(m_pFileInfoWidget = new FileInfoWidget(this), "");
	pTabBar->setTabButton(pTabBar->count() - 1, QTabBar::LeftSide, new QLabel("File Info"));
	//
	m_pTabWidget->addTab(m_pLayerInfoWidget = new LayerInfoWidget(this), "");
	pTabBar->setTabButton(pTabBar->count() - 1, QTabBar::LeftSide, new QLabel("Layer Info"));
	//
	m_pTabWidget->addTab(m_pProjectionInfoWidget = new ProjectionInfoWidget(this), "");
	pTabBar->setTabButton(pTabBar->count() - 1, QTabBar::LeftSide, new QLabel("Projection Info"));
	//
	m_pTabWidget->addTab(m_pOtherInfoWidget = new OtherInfoWidget(this), "");
	pTabBar->setTabButton(pTabBar->count() - 1, QTabBar::LeftSide, new QLabel("Other Info"));
#endif

}

ImageInfoWidget::~ImageInfoWidget()
{

}

void ImageInfoWidget::fillByImageFilePath(QString strFilePath)
{
	//根据传入的图片文件路径 - 使用GDAL库加载图片信息
	if (auto pDataset = (GDALDataset *)GDALOpen(strFilePath.toUtf8().data(), GA_ReadOnly))
	{
		//根据读取到信息分别填充到UI
		mpFileInfoWidget->fill(pDataset);
		mpLayerInfoWidget->fill(pDataset);
		mpProjectionInfoWidget->fill(pDataset);
		//mpOtherInfoWidget->fill(pDataset);
		GDALClose(pDataset);
	}
	else
	{
		mpFileInfoWidget->clear();
		mpLayerInfoWidget->clear();
		mpProjectionInfoWidget->clear();
	}
}
