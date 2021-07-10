#ifndef STYLEWIDGET_H
#define STYLEWIDGET_H

#include <QWidget>
#include <QEvent>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QComboBox>
#include <QTabWidget>
#include <QLabel>
#include <QSpinBox>
#include "modules/doc/slayermanager.h"

class StyleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StyleWidget(QWidget* parent = nullptr);
    ~StyleWidget();



    /*-----信号-----*/
signals:

    /*-----槽函数-----*/
public slots:
    void onSelectStateChanged();


    /*-----成员变量-----*/
private:
    SLayerManager* mpLayerManager = nullptr;

    QComboBox* mpComboLineType;
    QComboBox* mpComboFill;
    QComboBox* mpComboEndPoint;
    QComboBox* mpComboJoint;
    QSpinBox* mpSpinBoxWidth;

    QLabel* mpLabelLineType;
    QLabel* mpLabelFill;
    QLabel* mpLabelEndPoint;
    QLabel* mpLabelJoint;
    QLabel* mpLabelWidth;

    /*-----成员函数-----*/
public:
    //[访问函数]
    const SLayerManager* layerManager()const;
    //[修改函数]
    void SetLayerManager(SLayerManager* layerManager);

    //[功能函数]
private:
    void _initializeConnections();
    void _disconnect();
    void changeToPointShape();
    void changeToLineShape();
    void changeToPolygonShape();
    void changeToRaster();


};

#endif // STYLEWIDGET_H
