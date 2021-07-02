#ifndef SIMAGEMETA_H
#define SIMAGEMETA_H

#include "modules/paint/sobject.h"
#include "QImage"
#include <QMutex>
#include "gdal_priv.h"

//------------------------
//      SImageMeta类
//      图像元数据类
//------------------------

class SImageMeta
{
private:
    int mnWidth{0}, mnHeight{0}, mnBandCount{0};
    GDALDataType mnDataType{GDT_Byte};
    double mdGeoTransform[6] = {0};
    QString mqstrProjRef;
public:
    explicit SImageMeta(int width, int height, int bandCount, GDALDataType dataType):
        mnWidth(width), mnHeight(height), mnBandCount(bandCount), mnDataType(dataType)
    {
        Q_ASSERT(mnWidth > 0 && mnHeight > 0 && mnBandCount >= 0);
        Q_ASSERT(mnDataType >= 0 && mnDataType < GDT_TypeCount);
    }
    explicit SImageMeta(const QSize& imageSize, int bandCount, GDALDataType dataType):
        mnWidth(imageSize.width()), mnHeight(imageSize.height()), mnBandCount(bandCount), mnDataType(dataType)
    {
        Q_ASSERT(mnWidth > 0 && mnHeight > 0 && mnBandCount >= 0);
        Q_ASSERT(mnDataType >= 0 && mnDataType < GDT_TypeCount);
    }
    explicit SImageMeta(int width, int height, int bandCount, double* geoTransform, GDALDataType dataType, QString projRef = ""):
        mnWidth(width), mnHeight(height), mnBandCount(bandCount), mnDataType(dataType), mqstrProjRef(projRef)
    {
        Q_ASSERT(mnWidth > 0 && mnHeight > 0 && mnBandCount >= 0);
        Q_ASSERT(mnDataType >= 0 && mnDataType < GDT_TypeCount);
        memmove(mdGeoTransform, geoTransform, 6 * sizeof(double));
    }
    explicit SImageMeta(const QSize& imageSize, int bandCount, double* geoTransform,  GDALDataType dataType, QString projRef = ""):
        mnWidth(imageSize.width()), mnHeight(imageSize.height()), mnBandCount(bandCount), mnDataType(dataType), mqstrProjRef(projRef)
    {
        Q_ASSERT(mnWidth > 0 && mnHeight > 0 && mnBandCount >= 0);
        Q_ASSERT(mnDataType >= 0 && mnDataType < GDT_TypeCount);
        memmove(mdGeoTransform, geoTransform, 6 * sizeof(double));
    }



    explicit SImageMeta() = default;
    SImageMeta(const SImageMeta& meta) = default;
    SImageMeta& operator=(const SImageMeta& meta) = default;
    virtual ~SImageMeta() {}

public:
    inline int width() const {return mnWidth;}
    inline int height() const {return mnHeight;}
    inline QSize imageSize()const {return QSize(mnWidth, mnHeight);}
    inline int bandCount() const {return mnBandCount;}
    inline GDALDataType dataType() const {return mnDataType;}
    inline QString projRef()const {return mqstrProjRef;}
    inline double* geoTransform()const
    {
        double* dGeoTransform = new double[6];
        memmove(dGeoTransform, mdGeoTransform, 6 * sizeof(double));
        return dGeoTransform;
    }


    inline int& rWidth() {return mnWidth;}
    inline int& rHeight() {return mnHeight;}
    inline void setImageSize(int w, int h) {Q_ASSERT(w > 0 && h > 0); mnWidth = w; mnHeight = h;}
    inline void setImageSize(const QSize& sz) {Q_ASSERT(sz.width() > 0 && sz.height() > 0); mnWidth = sz.width(); mnHeight = sz.height();}
    inline int& rBandCount() {return mnBandCount;}
    inline GDALDataType& rDataType() {return mnDataType;}
    inline void setGeoTransform(double* GeoTransform) {memmove(mdGeoTransform, GeoTransform, 6 * sizeof(double));}
    inline QString& rProjRef() {return mqstrProjRef;}

    inline bool isEmpty()const {return mnWidth == 0 || mnHeight == 0 || mnBandCount == 0;}
    inline void clear() {mnWidth = 0; mnHeight = 0; mnBandCount = 0; mnDataType = GDT_Byte;}
};


#endif // SIMAGEMETA_H

