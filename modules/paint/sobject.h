#ifndef SOBJECT_H
#define SOBJECT_H

#include<QString>
#include<QPainter>
#include"modules/global.h"
#include<QIcon>

//----------------------------------------------------
//                  SObject
//                绘图对象虚基类
//  ***基本功能***
//  1. 图层命名、图层描述、图层颜色等图层属性
//  2. 平移（通过改变中心点坐标实现）、缩放、旋转
//  3. 生成预览图标(大小由global.h宏定义)
//  4. 坐标系转换（对象坐标系和逻辑绘图坐标系转换）
//
//  ***继承须知***
//  1. 实现所有的public纯虚函数
//  2. 注意坐标系的转换
//  3. 实现private纯虚函数_applyTransform对子类中的点进行坐标变换。
//  4. 实现paint虚函数时记得对painter进行平移变换到中心点，
//绘制完成后逆向平移。
//
//  ***其他***
//  在没有定义拷贝与移动函数前不要拷贝子类对象。
//
//                                      Shepard Liu
//                                      2021.05.07
//----------------------------------------------------

using namespace sgis;

class SObject
{
    friend class SObjectFactory;
    /*-----构造函数与析构函数-----*/
protected:
    explicit SObject(PaintObject _type, bool _selected = false, QPointF _center = QPointF(),
                     const QString& _layerName = "",
                     const QString& _layerDiscription = "",
                     const QColor& _layerColor = "");
    virtual ~SObject();

    /*-----虚函数-----*/
public:
    //绘制函数
    virtual void paint(QPainter &painter, bool doTranslate = true, QRectF viewLogicalArea = QRectF(), double scaleValue = 0)const = 0;
    //获取包围矩形（变换后），该矩形与相关Qt绘图类的boundingRect有所不同，是由原矩形进行缩放和旋转变换得到的。
    virtual QPolygonF boundingRect()const = 0;
    //是否包含某点(画布坐标系)，若参数2为True,则只判断包围矩形是否包含某点
    virtual bool contains(const QPointF& pt, bool isInBoundRect = false)const = 0;
    //变换后包围矩形是否与目标矩形有交集
    virtual bool intersect(const QRectF& rect)const;
    //输出与输入
    virtual void writeBinaryData(QDataStream& stream)const = 0;
    virtual void readBinaryData(QDataStream& stream) = 0;
    //绘制边框矩形（宽度和半径指定为实际像素）
    virtual void paintBoundRect(QPainter &painter, double scaleValue = 1, int lineWidth = BOUND_RECT_PEN_WIDTH, int radius = BOUND_RECT_CORNER_RADIUS);
    //图层预览图标（调用时生成）
    virtual QIcon icon()const;

private:
    //应用变换的方法
    virtual void _applyTransform() = 0;
    //变换前的原始矩形，用于绘制选框矩形
    virtual QRectF _originalRect() = 0;

    /*-----属性-----*/
protected:
    //[类型]
    const PaintObject type;
    //是否可见
    bool mbVisible{true};
    //是否被锁定
    bool mbLocked{false};
    //是否被选中
    bool mbSelected{true};
    //应用在对象中的变换:该变换不包含平移
    QTransform mTransform{};
    //旋转角（顺时针）单位为度
    double mdRotateAngle{0};
    //缩放量
    double mdSx{1.0}, mdSy{1.0};
    //中心点
    QPointF mPtCenter{};

    //图层命名
    QString mStrLayerName = "";
    //图层注释
    QString mStrLayerDiscription = "";
    //图层标识颜色
    QColor mLayerColor = QColor(255, 255, 255);

    /*-----成员变量-----*/
private:
    //[Me]
    const QString Me = "ObjectBase";

    /*-----成员函数-----*/
public:
    //[访问函数]
    bool isVisible()const;
    bool isLocked()const;
    bool isSelected()const;
    double rotateAngle()const;
    double scaleFactorX()const;
    double scaleFactorY()const;
    const QPointF centerPoint()const;
    const QString& layerName()const;
    const QString& layerDiscription()const;
    const QColor& layerColor()const;
    PaintObject getType();
    const QTransform &transform()const;

    //[修改函数]
    //变换
    void translate(double dx, double dy);
    void translate(const QPointF& pt);
    //参数doReCalc为True时,重新计算变换并应用变换到子类成员变量中
    void rotate(double angle, bool doReCalc = true);
    void scale(double sx, double sy, bool doReCalc = true);

    void setVisible(bool visible);
    void setLocked(bool lock);
    void setSelected(bool select);
    //设置旋转角：顺时针，单位为度
    void setRotateAngle(double angle, bool doReCalc = true);
    void setScaleFactor(double sx, double sy, bool doReCalc = true);
    void setCenterPoint(const QPointF& newCenterPt);
    void setLayerName(const QString& name);
    void setLayerDiscription(const QString& discription);
    void setLayerColor(const QColor& color);

    //[功能函数]
public:
    //按旋转和缩放重新计算变换，并且应用变换
    void reCalcTransfrom();

public:
    //中心坐标转实际坐标（控件坐标系）
    inline QPointF CtoA(const QPointF& pt)const;
    inline QPoint CtoA(const QPoint& pt)const;

    //实际坐标（控件坐标系）转中心坐标
    inline QPointF AtoC(const QPointF& pt)const;
    inline QPoint AtoC(const QPoint& pt)const;

    void _initializeWith(const SObject& theObj);
};

QPointF SObject::CtoA(const QPointF &pt) const
{
    return pt + mPtCenter;
}

QPoint SObject::CtoA(const QPoint &pt) const
{
    return QPoint(pt.x() + mPtCenter.x(), pt.y() + mPtCenter.y());
}

QPointF SObject::AtoC(const QPointF &pt) const
{
    return pt - mPtCenter;
}

QPoint SObject::AtoC(const QPoint &pt) const
{
    return QPoint(pt.x() - mPtCenter.x(), pt.y() - mPtCenter.y());
}


#endif // SOBJECT_H
