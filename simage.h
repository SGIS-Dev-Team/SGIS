#ifndef SIMAGE_H
#define SIMAGE_H

#include "sshape.h"
#include "QImage"

class SImage : public SShape
{
public:
    SImage() = delete;
    explicit SImage(PaintObject _type, const QImage& image, bool _selected = true, QPoint center = QPoint(),
                    const QString& _layerName = "",
                    const QString& _layerDiscription = "",
                    const QColor& _layerColor = "");

private:
    const QString Me = "ImageBase";

protected:
    QRectF mImageRect{};

public:
    virtual void updatePath();
    virtual void paint(QPainter* painter)const;

};

#endif // SIMAGE_H
