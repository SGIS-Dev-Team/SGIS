#ifndef SPOINTFEATURE_H
#define SPOINTFEATURE_H

#include "sfeature.h"
class SPointFeature : public SFeature
{
    Q_OBJECT
public:
    explicit SPointFeature(bool _selected = false, QPointF point = QPointF(),
                                const QString& _layerName = "",
                                const QString& _layerDiscription = "",
                                const QColor& _layerColor = "");
    virtual ~SPointFeature() {}
public:
    void addPoint(QPointF point);
public:
    virtual void paint(QPainter& painter, bool doTranslate, const QRectF& viewLogicalArea, double scaleValue, PaintTrigger trigger) const override;
    virtual bool contains(const QPointF& pt, bool isInBoundRect) const override;
    virtual void writeBinaryData(QDataStream& stream) const override;
    virtual void readBinaryData(QDataStream& stream) override;
};

#endif // SPOINTFEATURE_H
