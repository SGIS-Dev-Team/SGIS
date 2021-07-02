#ifndef OTHERINFOWIDGET_H
#define OTHERINFOWIDGET_H

#include <QWidget>
class GDALDataset;
class OtherInfoWidget : public QWidget
{
	Q_OBJECT

public:
	OtherInfoWidget(QWidget *parent);
	~OtherInfoWidget();

	void fill(GDALDataset* pGDALDataset);
private:
	
};

#endif // OTHERINFOWIDGET_H
