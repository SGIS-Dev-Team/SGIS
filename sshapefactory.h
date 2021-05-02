#ifndef SSHAPEFACTORY_H
#define SSHAPEFACTORY_H

//  SShapeFactory
//  形状工厂类
//  用于创建形状

#include "sshape.h"

class SShapeFactory: QObject
{
    //常量和枚举
public:
    enum ShapeSet
    {
        Line,
        Triangle,
        Rectangle,
        Pantagon,
        Hexagon,

        Circle,
        Curve,

        Custom_Figure
    };

    /*-----成员函数-----*/
public:
    //[功能函数]
    static SShape* createShape(ShapeSet type, const QPoint& pt);
};

#endif // SSHAPEFACTORY_H
