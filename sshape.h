#ifndef SSHAPE_H
#define SSHAPE_H

#include "sobject.h"
#include <QPainterPath>
#include "initializer_list"

//  SShape
//  形状类
//  特性：
//  SShape至少有两个点
//  SShape在实现上是闭合图形
//  在绘制时将mbClose置false不绘制闭合线、不填充，以显示为不闭合图形
//  SShape的点坐标指定为中心偏移量

class SShape : public SObject
{
    friend class SShapeFactory;

    /*枚举常量定义*/
    enum Filler
    {
        Image,
        Gradient,
        Color,
        None
    };

    /*-----构造函数与析构函数-----*/
    //仅可使用形状工厂创建
protected:
    explicit SShape(bool _selected = true, QPointF center = QPointF(),
                    const QString& _layerName = "",
                    const QString& _layerDiscription = "",
                    const QColor& _layerColor = "");
    virtual ~SShape();

    /*-----虚函数重载-----*/
public:
    //绘制函数
    virtual void paint(QPainter &painter, bool doTranslate = true, QRectF viewLogicalArea = QRectF(), double scaleValue = 0)const;
    //获取包围矩形
    virtual QPolygonF boundingRect()const;
    //是否包含某点(画布坐标系)
    virtual bool contains(const QPointF& pt)const;
    //输出与输入
    virtual void writeBinaryData(QDataStream& stream)const;
    virtual void readBinaryData(QDataStream& stream);
    //应用变换
    virtual void _applyTransform();

    /*-----属性-----*/
protected:
    //形状顶点点向量,存储形状顶点对中心点的偏移量
    std::vector<QPointF> mVerticesVec{};
    //形状贝塞尔曲线控制点向量，存储每条线的两个三次贝塞尔曲线控制点
    std::vector<QPointF> mControlPtVec{};

    //绘制路径
    QPainterPath mPath;
    QPainterPath mPathTransformed;
    //样式
    QPen mPen{nullptr};
    QBrush mBrush{nullptr};

    //填充图像
    QPixmap mTextureImage;

    //是否闭合，这仅仅是一个标记
    bool mbClose{};
    //填充对象
    Filler filler = None;

    //更新路径标记
    bool mbDoUpdatePath{true};

    /*-----成员变量-----*/
private:
    const QString Me = "ShapeBase";

    /*-----成员函数-----*/
public:
    //[访问函数]

    //返回基于中心相对坐标的形状顶点坐标向量
    const std::vector<QPointF>& vertices()const;
    //返回基于中心相对坐标的形状连线控制点坐标向量
    const std::vector<QPointF>& ctrlPoints()const;
    //返回绘制路径
    const QPainterPath& path()const;

    const QPen& pen()const;
    const QBrush& brush()const;

    bool isClose()const;
    bool doUpdate()const;

    //返回基于控件坐标的形状顶点坐标
    QPointF vertexAt(size_t idx)const;
    QPointF ctrlPtAt(size_t idx)const;

    //[修改函数]

    //添加顶点并保证直线连接
    void addVertex(const QPointF& pt);
    //添加多个顶点并保证直线连接
    void addVertex(const QPointF* pt, size_t ptCount);
    //添加多个顶点并保证直线连接
    void addVertex(std::initializer_list<QPointF> pt);

    //设置填充图像
    void setTextureImage(const QPixmap &image);

protected:
    //添加顶点并为控制点分配内存
    void _addVertex(const QPointF& pt, const QPointF& cPt1, const QPointF& cPt2);
    //添加多个顶点并为控制点分配内存，控制点数为顶点数的两倍
    void _addVertex(const QPointF* pt, const QPointF* cpt, size_t ptCount);
    //添加多个顶点并为控制点分配内存，控制点数为顶点数的两倍
    void _addVertex(std::initializer_list<QPointF> pt, std::initializer_list<QPointF> cpt);

public:
    void setVertex(const QPointF& pt, size_t idx);
    //在idx前插入一个元素,idx = 0 ~ size
    void insertVertex(const QPointF& pt, size_t idx);
    //删除目标点，并返回其坐标
    QPointF removeVertex(size_t idx);

    /*  修改控制点
     * @param cpt 新控制点坐标
     * @param idx 第idx条线
     * @param secondPt 是否为该线第2个控制点
     */
    void setCtrlPt(const QPointF& cpt, size_t idx, bool secondPt);
    //目标将被拷贝
    void setPen(const QPen& pen);
    //目标将被拷贝
    void setBrush(const QBrush& brush);
    //设置填充模式
    void setFillRule(Qt::FillRule rule);

    QPen& rpen();
    QBrush& rbrush();

    void setClose(bool close);
    void setUpdate(bool update);

    //[功能函数]
    virtual void updatePath();
};

#endif // SSHAPE_H
