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
    void styleChanged();
    /*-----槽函数-----*/
public slots:
    void onSelectStateChanged();
    void changeComeUp();
private slots:
    //过滤掉用于表达多值存在的-1值
    void onSpinBoxValueChanged(int value);



    /*-----成员变量-----*/
private:
    SLayerManager* mpLayerManager = nullptr;

    QComboBox* mpComboLineType;
    QComboBox* mpComboFill;
    QComboBox* mpComboCapStyle;
    QComboBox* mpComboJoint;
    QSpinBox* mpSpinBoxWidth;

    QLabel* mpLabelLineType;
    QLabel* mpLabelFill;
    QLabel* mpLabelEndPoint;
    QLabel* mpLabelJoint;
    QLabel* mpLabelWidth;

    /*-----属性-----*/
    //为true，是用户交互产生的信号
    //为false,是程序修改时产生的信号（应当忽视）
    bool mbRealChangeFlag = false;

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
    //设置enable属性
    void changeToPointShape();
    void changeToLineShape();
    void changeToPolygonShape();
    void changeToRaster();
    //设置图层的样式
    void setLayerPen(QPen& pen);
    void setLayerBrush(QBrush& brush);
    //根据图层样式，显示在窗口上,用以模拟PPT中选项的行为
    void setStylePen(QPen& pen);
    void setStyleBrush(QBrush& brush);

};

#endif // STYLEWIDGET_H
