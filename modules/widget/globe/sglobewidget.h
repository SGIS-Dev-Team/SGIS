#ifndef SGLOBEWIDGET_H
#define SGLOBEWIDGET_H

#include <modules/global.h>

#include <QOpenGLWidget>
#include <osgViewer/Viewer>
#include <QInputEvent>
#include <osgViewer/Viewer>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/Sky>

#include "modules/global.h"

class SGlobeWidget : public QOpenGLWidget
{
    Q_OBJECT

    /*-----构造函数与析构函数-----*/
public:
    explicit SGlobeWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~SGlobeWidget();

    /*-----虚函数重载-----*/
protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    // 处理鼠标单击事件
    virtual void mousePressEvent(QMouseEvent* event)override;
    virtual void mouseReleaseEvent(QMouseEvent* event)override;

    // 处理鼠标双击事件
    virtual void mouseMoveEvent(QMouseEvent* event)override;

    // 处理鼠标中轮滑动事件
    virtual void wheelEvent(QWheelEvent* event)override;

    virtual bool event(QEvent* event)override;

    /*-----信号-----*/
signals:

    /*-----槽函数-----*/
private slots:

    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
protected:
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> mpGraphicsWindow;
    osg::ref_ptr<osgViewer::Viewer> mpViewer;
    qreal mdScaleX, mdScaleY;

    /*-----成员函数-----*/
public:
    //[访问函数]

    //[修改函数]

    //[功能函数]
    void drawLine(const std::vector<double> coors);

private:
    void _initialize();
    void _initializeConnections();

};


#endif // SGLOBEWIDGET_H
