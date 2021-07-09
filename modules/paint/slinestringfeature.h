#ifndef SLINESTRINGFEATURE_H
#define SLINESTRINGFEATURE_H

#include "sfeature.h"

class SLineStringFeature : public SFeature
{
    Q_OBJECT

    /*-----构造函数与析构函数-----*/
public:
    explicit SLineStringFeature(PaintObject _type, bool _selected = false, QPointF _center = QPointF(),
                                const QString& _layerName = "",
                                const QString& _layerDiscription = "",
                                const QColor& _layerColor = "");
    virtual ~SLineStringFeature() {}

    /*-----虚函数重载-----*/
public:
    virtual void paint(QPainter& painter, bool doTranslate, const QRectF& viewLogicalArea, double scaleValue, PaintTrigger trigger) const override;
    virtual bool contains(const QPointF& pt, bool isInBoundRect) const override;
    virtual void writeBinaryData(QDataStream& stream) const override;
    virtual void readBinaryData(QDataStream& stream) override;

    /*-----信号-----*/
signals:

    /*-----槽函数-----*/
private slots:

    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
protected:


    /*-----成员函数-----*/
public:
    //[访问函数]

    //[修改函数]

    //[功能函数]

};

#endif // SLINESTRINGFEATURE_H
