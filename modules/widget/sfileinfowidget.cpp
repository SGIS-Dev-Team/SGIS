#include "sfileinfowidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QDateTime>
#include "gdal_priv.h" 
#include "ogrsf_frmts.h"
FileInfoWidget::FileInfoWidget(QWidget *parent)
	: QWidget(parent)
{
	auto pVLayout = new QVBoxLayout(this);
	//
	auto pLineLayout = new QHBoxLayout(this);
	pLineLayout->addWidget(new QLabel("Layer Name:"));
	pLineLayout->addWidget(mpLayerNameLabel = new QLabel(""));
	pLineLayout->addSpacing(10);
	pLineLayout->addWidget(new QLabel("File Type:"));
	pLineLayout->addWidget(mpFileTypeLabel = new QLabel(""));
	pVLayout->addLayout(pLineLayout);

	//
	pLineLayout = new QHBoxLayout();
	pLineLayout->addWidget(new QLabel("Last Modified:"));
	pLineLayout->addWidget(mpLastModifyTimeLabel = new QLabel(""));
	pLineLayout->addSpacing(10);
	pLineLayout->addWidget(new QLabel("Number of Layers:"));
	pLineLayout->addWidget(mpLayerCountLabel = new QLabel(""));
	pVLayout->addLayout(pLineLayout);

	//
	pLineLayout = new QHBoxLayout();
	pLineLayout->addWidget(new QLabel("Image/Auxiliary File(s):"));
	pLineLayout->addWidget(mpAuxiliaryFilesComboBox = new QComboBox());
	pLineLayout->addSpacing(10);
	pLineLayout->addWidget(new QLabel("File Size:"));
	pLineLayout->addWidget(mpFileSizeLabel = new QLabel(""));
	pVLayout->addLayout(pLineLayout);

	//
	pVLayout->addStretch();

	clear();
}

FileInfoWidget::~FileInfoWidget()
{

}
//
QString formatFileSize(quint64 size)
{
	if (size < 1024 * 1024)
	{
		return QString().sprintf("%.1f  K", (float)size / (1024.0 * 1024));
	} 
	else if (size < 1024 * 1024 * 1024)
	{
		return QString().sprintf("%.1f M", (float)size / (1024.0 * 1024));
	}
	return QString().sprintf("%.1f G", (float)size / (1024.0 * 1024 * 1024));
}

void FileInfoWidget::fill(GDALDataset* pGDALDataset)
{
	//根据从GDALDataset读取到的实际信息填充
	//图层名称
	auto nLayerCount = pGDALDataset->GetLayerCount();
	auto pLayer = nLayerCount == 0 ? nullptr : pGDALDataset->GetLayer(0);
	auto strLayerName = pLayer != nullptr ? QString::fromUtf8(pLayer->GetName()) : "";
	mpLayerNameLabel->setText(strLayerName);
	
	//图片格式
	auto pGDALDriver = pGDALDataset->GetDriver();
	auto strType = pGDALDriver != nullptr ? QString::fromUtf8(pGDALDriver->GetDescription()) : "";
	mpFileTypeLabel->setText(strType);
	
	//得到图片文件路径 - 读取文件大小及最后修改时间
	QFileInfo fileInfo(pGDALDataset->GetFileList()[0]);
	//显示时间 格式可以调整 （ddd MMMM hh:mm:ss yyyy）
	mpLastModifyTimeLabel->setText(fileInfo.lastModified().toString("ddd MMMM hh:mm:ss yyyy"));
	//
	mpFileSizeLabel->setText(formatFileSize(fileInfo.size()));
	//
	mpLayerCountLabel->setText(QString::number(nLayerCount));
}

void FileInfoWidget::clear()
{
	mpLayerNameLabel->clear();
	mpFileTypeLabel->clear();
	mpLastModifyTimeLabel->clear();
	mpLayerCountLabel->clear();
	mpAuxiliaryFilesComboBox->clear();
	mpFileSizeLabel->clear();
}
