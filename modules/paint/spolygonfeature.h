#ifndef SPOLYGONFEATURE_H
#define SPOLYGONFEATURE_H

#include "sfeature.h"
class SPolygonFeature : public SFeature
{
    Q_OBJECT
public:
    explicit SPolygonFeature(bool _selected = false, QPointF point = QPointF(),
                                const QString& _layerName = "",
                                const QString& _layerDiscription = "",
                                const QColor& _layerColor = "");
    virtual ~SPolygonFeature() {}
public:
    void addPoint(QPointF point);
    bool isValid();
public:
    virtual void paint(QPainter& painter, bool doTranslate, const QRectF& viewLogicalArea, double scaleValue, PaintTrigger trigger) const override;
    virtual bool contains(const QPointF& pt, bool isInBoundRect) const override;
    virtual void writeBinaryData(QDataStream& stream) const override;
    virtual void readBinaryData(QDataStream& stream) override;
};

#endif // SPOLYGONFEATURE_H
