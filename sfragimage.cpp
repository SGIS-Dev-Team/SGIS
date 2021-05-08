#include "sfragimage.h"

SFragImage::SFragImage(PaintObject _type, SFragLoader &_loader, bool _selected, QPointF center, const QString &_layerName, const QString &_layerDiscription, const QColor &_layerColor)
    : SObject(_type, _selected, center, _layerName, _layerDiscription, _layerColor)
{

}
