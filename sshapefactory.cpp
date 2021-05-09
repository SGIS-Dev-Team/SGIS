#include "sshapefactory.h"
#include <cassert>

SShape *SShapeFactory::createShape(ShapeSet type)
{
    SShape* shape = new SShape(true);
    switch (type)
    {
    case ShapeSet::Line:
        shape->addVertex(
        {
            QPointF(0, 0),
            QPointF(0, 0)
        });
        shape->setLayerName(tr("Line"));
        break;
    case ShapeSet::Triangle:
        shape->addVertex(
        {
            QPointF(-0.5, 0.5),
            QPointF(0.5, 0.5),
            QPointF(0, 0.5)
        });
        shape->setLayerName(tr("Triangle"));
        break;
    case ShapeSet::Rectangle:
        shape->addVertex(
        {
            QPointF(-0.5, -0.2),
            QPointF(0.5, -0.2),
            QPointF(0.5, 0.2),
            QPointF(-0.5, 0.2)
        });
        shape->setLayerName(tr("Rectangle"));
        break;
    case ShapeSet::Pantagon:
        shape->addVertex(
        {
            QPointF(0.0000000, -0.4000000),
            QPointF(-0.3804226, -0.1236068),
            QPointF(-0.2351141, 0.3236068),
            QPointF(0.2351141, 0.3236068),
            QPointF(0.3804226, -0.1236068)
        });
        shape->setLayerName(tr("Pantagon"));
        break;
    case ShapeSet::Hexagon:
        shape->addVertex(
        {
            QPointF(0.0000000, -0.4000000),
            QPointF(-0.3464102, -0.2000000),
            QPointF(-0.3464102, 0.2000000),
            QPointF(0.0000000, 0.4000000),
            QPointF(0.3464102, 0.2000000),
            QPointF(0.3464102, -0.2000000)
        });
        shape->setLayerName(tr("Hexagon"));
        break;
    case ShapeSet::Circle:
        shape->addVertex(
        {

        }
        );
        break;
    case ShapeSet::Curve:
        break;
    case ShapeSet::Custom_Figure:
        break;
    default:
        //形状不存在
        Q_ASSERT(0);
        break;
    }
    return shape;
}
