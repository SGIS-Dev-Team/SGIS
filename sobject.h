﻿#ifndef SOBJECT_H
#define SOBJECT_H

#include<QString>
#include<QPainter>
#include"global.h"
#include<QIcon>

//---------------------------------------------
//  SObject
//  绘图对象虚基类
//  规定基类及其子类在实现中点的坐标使用中心相对坐标
//---------------------------------------------

using namespace sgis;

class SObject
{
    friend class SObjectFactory;
    /*-----构造函数与析构函数-----*/
protected:
    explicit SObject(PaintObject _type, bool _selected = true, QPoint center = QPoint(),
                     const QString& _layerName = "",
                     const QString& _layerDiscription = "",
                     const QColor& _layerColor = "");
    virtual ~SObject();

    /*-----虚函数-----*/
public:
    //绘制函数
    virtual void paint(QPainter &painter, bool doTranslate = true)const = 0;
    //获取包围矩形
    virtual QRectF rect()const = 0;
    //是否包含某点
    virtual bool contains(const QPointF& pt)const = 0;
    //变换
    virtual void translate(double dx, double dy) = 0;
    virtual void translate(const QPointF& pt) = 0;
    virtual void rotate(double angle) = 0;
    virtual void scale(double sx, double sy) = 0;
    //输出与输入
    virtual void writeBinaryData(QDataStream& stream)const = 0;
    virtual void readBinaryData(QDataStream& stream) = 0;
    //图层预览图标（调用时生成）
    virtual const QIcon &icon() = 0;


    //绘制边框矩形
    virtual void paintBoundRect(QPainter &painter, bool doTranslate = true);

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
    //旋转角（顺时针）单位为度
    bool mdRotateAngle{};
    //中心点
    QPointF mPtCenter{};

    //图层预览图标
    QIcon mIcon;
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
    bool rotateAngle()const;
    const QPointF centerPoint()const;
    const QString& layerName()const;
    const QString& layerDiscription()const;
    const QColor& layerColor()const;

    //[修改函数]
    void setVisible(bool visible);
    void setLocked(bool lock);
    void setSelected(bool select);
    //设置旋转角：顺时针，单位为度
    void setRotateAngle(double angle);
    void setCenterPoint(const QPointF& newCenterPt);
    void setLayerName(const QString& name);
    void setLayerDiscription(const QString& discription);
    void setLayerColor(const QColor& color);

    //[功能函数]
    //中心坐标转实际坐标（控件坐标系）
    inline QPointF CtoA(const QPointF& pt)const;
    inline QPoint CtoA(const QPoint& pt)const;

    //实际坐标（控件坐标系）转中心坐标
    inline QPointF AtoC(const QPointF& pt)const;
    inline QPoint AtoC(const QPoint& pt)const;

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