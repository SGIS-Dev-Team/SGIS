#ifndef PROJECTIONINFO_H
#define PROJECTIONINFO_H

#include <QWidget>
#include <QLabel>
class GDALDataset;
class ProjectionInfoWidget : public QWidget
{
	Q_OBJECT

public:
	ProjectionInfoWidget(QWidget *parent);
	~ProjectionInfoWidget();
public:
	void fill(GDALDataset* pGDALDataset);
	//
	void clear();
private:
	QLabel* mpProjectionTypeLabel;
	QLabel* mpSpheroidNameLabel;
	QLabel* mpDatumNameLabel;

	QLabel* mpUTMZoneLabel;
	QLabel* mpNORTHOrSOUTHLabel;
};

#endif // PROJECTIONINFO_H
