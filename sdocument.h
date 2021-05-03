#ifndef SDOCUMENT_H
#define SDOCUMENT_H
#include <qcanvas.h>
#include <global.h>
#include <slayermanager.h>

class QCanvas;
class SDocument
{
    /*-----构造函数与析构函数-----*/
public:
    //构造一个空文档，绑定一个画布对象
    explicit SDocument(QCanvas* canvas);
    //从文件构建文档，绑定一个画布对象
    explicit SDocument(QCanvas* canvas, const QString& path);
    ~SDocument();

    /*-----成员变量-----*/
private:
    //画布
    QCanvas * mpCanvas;
    //图层管理器
    SLayerManager mLayerMgr;

    /*-----成员函数-----*/
public:

    //-----访问与修改函数-----//
    //指定画布
    void setCanvas(QCanvas * canvas);

    //获取画布
    QCanvas * getCanvas();

    //获取图层管理器
    SLayerManager& getLayerManager();

    //在画布上绘制图层
    void paint(QPainter & painter);
};

#endif // SDOCUMENT_H
