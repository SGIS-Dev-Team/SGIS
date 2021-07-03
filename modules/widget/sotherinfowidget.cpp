#include "sotherinfowidget.h"
#include "gdal_priv.h" 
#include "ogrsf_frmts.h"
OtherInfoWidget::OtherInfoWidget(QWidget *parent)
	: QWidget(parent)
{

}

OtherInfoWidget::~OtherInfoWidget()
{

}
void OtherInfoWidget::fill(GDALDataset* pGDALDataset)
{
}
