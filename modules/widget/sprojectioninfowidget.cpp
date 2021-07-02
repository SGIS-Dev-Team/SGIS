#include "sprojectioninfowidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include "gdal_priv.h" 
#include "ogrsf_frmts.h"

ProjectionInfoWidget::ProjectionInfoWidget(QWidget *parent)
	: QWidget(parent)
{
	auto pFormLayout = new QFormLayout(this);
	pFormLayout->addRow("Projection Type:", mpProjectionTypeLabel = new QLabel("......"));
	pFormLayout->addRow("Spheroid Name:", mpSpheroidNameLabel = new QLabel("......"));
	pFormLayout->addRow("Datum Name:", mpDatumNameLabel = new QLabel("......"));
	pFormLayout->addRow("UTM Zone:", mpUTMZoneLabel = new QLabel("......"));
	pFormLayout->addRow("NORTH or SOUTH:", mpNORTHOrSOUTHLabel = new QLabel("......"));

	clear();
}

ProjectionInfoWidget::~ProjectionInfoWidget()
{

}

void ProjectionInfoWidget::fill(GDALDataset* pGDALDataset)
{
	auto sz = pGDALDataset->GetProjectionRef();
}

void ProjectionInfoWidget::clear()
{
	mpProjectionTypeLabel->clear();
	mpSpheroidNameLabel->clear();
	mpDatumNameLabel->clear();

	mpUTMZoneLabel->clear();
	mpNORTHOrSOUTHLabel->clear();
}
