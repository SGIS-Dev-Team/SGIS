#ifndef LAYERINFOWIDGET_H
#define LAYERINFOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
class GDALDataset;
class LayerInfoWidget : public QWidget
{
	Q_OBJECT

public:
	LayerInfoWidget(QWidget *parent);
	~LayerInfoWidget();
public:
	void fill(GDALDataset* pGDALDataset);
	//
	void clear();
private:
	QLabel* mpWidthLabel;
	QLabel* mpHeightLabel;
	QLabel* mpTypeLabel;
	//
	QLabel* mpBlockWidthLabel;
	QLabel* mpBlockHeightLabel;
	QLabel* mpDataTypeLabel;

	//
	QLabel* mpCompressionLabel;
	QLabel* mpDataOrderLabel;
	//
	QLabel* mpPyramidLayerAlgorithmLabel;
};

#endif // LAYERINFOWIDGET_H
