#ifndef SSHAPEFACTORY_H
#define SSHAPEFACTORY_H

//  SShapeFactory
//  形状工厂类
//  用于创建形状

#include "sobjectfactory.h"
#include "sshape.h"

enum class ShapeSet
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

class SShapeFactory
    : public SObjectFactory
{
private:
    SShapeFactory();
public:
    ~SShapeFactory();


    /*-----成员函数-----*/
public:
    //[功能函数]
    static SShape* createShape(ShapeSet type);
};

#endif // SSHAPEFACTORY_H
