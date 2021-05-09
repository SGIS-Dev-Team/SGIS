#ifndef SDOCUMENT_H
#define SDOCUMENT_H
#include <qcanvas.h>
#include <global.h>
#include <slayermanager.h>
#include <sfragloader.h>
class QCanvas;
class SDocument: public QObject
{
    Q_OBJECT
    /*-----构造函数与析构函数-----*/
public:
    //构造一个空文档，绑定一个画布对象
    explicit SDocument(QCanvas* canvas);
    //从文件构建文档，绑定一个画布对象
    explicit SDocument(QCanvas* canvas, const QString& path);
    ~SDocument();

    /*-----信号-----*/
signals:

    /*-----槽函数-----*/
private slots:
    void onImageLoaded(SImage* pImage);

    /*-----成员变量-----*/
private:
    //画布
    QCanvas *mpCanvas;
    //图层管理器
    SLayerManager mLayerMgr;
    //分片读取器
    SFragLoader mFragLoader;

    /*-----成员函数-----*/
public:

    //-----访问与修改函数-----//
    //指定画布
    void setCanvas(QCanvas * canvas);

    //获取画布
    QCanvas *getCanvas();

    //获取图层管理器
    SLayerManager &getLayerManager();

    //获取分片读取器
    SFragLoader &getFragLoader();

    //在画布上绘制图层
    void paint(QPainter &painter, const QRectF &viewArea, double scaleValue);

    //------功能函数------//
private:
    void _initializeConnections();
};

#endif // SDOCUMENT_H
