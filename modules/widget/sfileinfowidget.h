#ifndef FILEINFOWIDGET_H
#define FILEINFOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
class GDALDataset;
class FileInfoWidget : public QWidget
{
	Q_OBJECT

public:
	FileInfoWidget(QWidget *parent);
	~FileInfoWidget();

	//
	void fill(GDALDataset* pGDALDataset);
	//
	void clear();
private:
	QLabel* mpLayerNameLabel;
	QLabel* mpFileTypeLabel;
	QLabel* mpLastModifyTimeLabel;
	QLabel* mpLayerCountLabel;
	QComboBox* mpAuxiliaryFilesComboBox;
	QLabel* mpFileSizeLabel;
};

#endif // FILEINFOWIDGET_H
