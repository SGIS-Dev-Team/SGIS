#include "simageinfowidget.h"
#include <QEvent>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QScrollArea>
#include "gdal_priv.h" 

ImageInfoWidget::ImageInfoWidget(QWidget *parent)
	: QWidget(parent)
{
	//���ִ���
	auto pLayout = new QVBoxLayout(this);
	pLayout->setMargin(0);
	auto pScrollArea = new QScrollArea();
	pLayout->addWidget(pScrollArea);
	//
	auto pWidget = new QWidget();
	pLayout = new QVBoxLayout(pWidget);
	pLayout->setMargin(0);
	pLayout->addWidget(mpTabWidget = new QTabWidget(this));

	/*��Ӿ������������
	File Info  - �ļ���Ϣ
	Layer Info - ͼ����Ϣ
	Projection Info - ͶӰ��Ϣ
	*/
	mpTabWidget->addTab(mpFileInfoWidget = new FileInfoWidget(this), "File Info");
	mpTabWidget->addTab(mpLayerInfoWidget = new LayerInfoWidget(this), "Layer Info");
	mpTabWidget->addTab(mpProjectionInfoWidget = new ProjectionInfoWidget(this), "Projection Info");
	//other��Ϣ��䣬���ڸ���ʵ����Ҫ����
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
	//���ݴ����ͼƬ�ļ�·�� - ʹ��GDAL�����ͼƬ��Ϣ
	if (auto pDataset = (GDALDataset *)GDALOpen(strFilePath.toUtf8().data(), GA_ReadOnly))
	{
		//���ݶ�ȡ����Ϣ�ֱ���䵽UI
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
