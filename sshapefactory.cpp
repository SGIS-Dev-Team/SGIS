#include "sshapefactory.h"
#include <cassert>

SShape *SShapeFactory::createShape(SShapeFactory::ShapeSet type, const QPoint& pt)
{
    SShape* shape = new SShape(ShapeBase, true, pt);
    switch (type)
    {
    case Line:
        shape->addVertex(
        {
            QPointF(0, 0),
            QPointF(0, 0)
        });
        shape->setLayerName(tr("Line"));
        break;
    case Triangle:
        shape->addVertex(
        {
            QPointF(-0.5, 0.5),
            QPointF(0.5, 0.5),
            QPointF(0, 0.5)
        });
        shape->setLayerName(tr("Triangle"));
        break;
    case Rectangle:
        shape->addVertex(
        {
            QPointF(-0.2, -0.2),
            QPointF(0.2, -0.2),
            QPointF(0.5, 0.5),
            QPointF(-0.5, 0.5)
        });
        shape->setLayerName(tr("Rectangle"));
        break;
    case Pantagon:
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
    case Hexagon:
        shape->addVertex(
        {
            QPointF(0.0000000, -0.4000000),
            QPointF(-0.3464102, -0.2000000),
            QPointF(-0.3464102, 0.2000000),
            QPointF(0.0000000, 0.4000000),
            QPointF(0.3464102, 0.2000000),
            QPointF(0.3464102, -0.2000000)
        });
        shape->setLayerName(tr("Pantagon"));
        break;
    case Circle:
        shape->addVertex(
        {

        },
        {

        });
        break;
    case Curve:
        break;
    case Custom_Figure:
        break;
    default:
        //形状不存在
        assert(0);
        break;
    }
    return shape;
}
