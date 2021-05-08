#ifndef SFRAGIMAGE_H
#define SFRAGIMAGE_H

#include "sobject.h"
#include "sfragmatrix.h"
#include "sfragloader.h"

class SFragImage : public SObject
{
    Q_OBJECT

    /*-----构造函数与析构函数-----*/
public:
    explicit SFragImage(PaintObject _type, SFragLoader& _loader, bool _selected = true, QPointF center = QPointF(),
                        const QString& _layerName = "",
                        const QString& _layerDiscription = "",
                        const QColor& _layerColor = "");

    /*-----虚函数重载-----*/
public:

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

#endif // SFRAGIMAGE_H
