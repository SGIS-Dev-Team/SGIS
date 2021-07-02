#ifndef TIFFINFOWIDGET_H
#define TIFFINFOWIDGET_H

#include <QWidget>
#include <QJsonObject>
#include "sfileinfowidget.h"
#include "slayerinfowidget.h"
#include "sprojectioninfowidget.h"
#include "sotherinfowidget.h"
class ImageInfoWidget : public QWidget
{
	Q_OBJECT

public:
	ImageInfoWidget(QWidget *parent = nullptr);
	~ImageInfoWidget();
public:
	void fillByImageFilePath(QString strFilePath);
private:
	QTabWidget* mpTabWidget;
	FileInfoWidget* mpFileInfoWidget;
	LayerInfoWidget* mpLayerInfoWidget;
	ProjectionInfoWidget* mpProjectionInfoWidget;
	OtherInfoWidget* mpOtherInfoWidget;

};

#endif // TIFFINFOWIDGET_H
