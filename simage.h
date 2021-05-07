#ifndef SIMAGE_H
#define SIMAGE_H

#include "sshape.h"
#include "QImage"

//------------------------
//      SImage类
//      图像类
//      包含图像对象指针
//      和4个图像包围控制点
//------------------------

class SImage : public SObject
{
public: //测试完记得给回protected
    explicit SImage(PaintObject _type, QPixmap *image, bool _selected = true, QPoint center = QPoint(),
                    const QString& _layerName = "",
                    const QString& _layerDiscription = "",
                    const QColor& _layerColor = "");
    virtual ~SImage();
    /*-----虚函数重载-----*/
public:
    //绘制函数
    virtual void paint(QPainter &painter, bool doTranslate = true)const;
    //获取包围矩形
    virtual QPolygonF boundingRect()const;
    //是否包含某点
    virtual bool contains(const QPointF& pt)const;
    //输出与输入
    virtual void writeBinaryData(QDataStream& stream)const;
    virtual void readBinaryData(QDataStream& stream);

private:
    //应用变换到成员
    virtual void _applyTransform();

    /*-----属性-----*/

    /*-----成员变量-----*/
private:
    const QString Me = "ImageBase";

    //图像
    QPixmap *mpImage{nullptr};
    //图像在无缩放无旋转下的包围矩形
    QRectF mImageRect;
    //图像的包围控制点（变换后）
    QPointF mpBoundPt[4] {};

    /*-----成员函数-----*/
public:
    //[访问函数]

    //[修改函数]

    //[功能函数]
};

#endif // SIMAGE_H
