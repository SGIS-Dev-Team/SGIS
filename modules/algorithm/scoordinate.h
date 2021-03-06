#ifndef SCOORDINATE_H
#define SCOORDINATE_H

#include <QString>
#include <QPointF>
#include <QRegExp>
#include "modules/global.h"
//--------------------------
//      SCoordinate类
//    像素与地理坐标转换类
//    现阶段可默认地理坐标在画布上均匀、水平垂直分布
//    即只需要4个参数描述像素坐标与地理坐标之间的关系
//--------------------------


class SCoordinate
{
    /*-----构造函数与析构函数-----*/
public:
    explicit SCoordinate() = default;
    explicit SCoordinate(double x0, double y0, double scaleX, double scaleY, QString Info = "")
        : mdX0(x0), mdY0(y0), mdScaleX(scaleX), mdScaleY(scaleY), mqstrCoorInfo(Info), mbFlag(true) {}
    explicit SCoordinate(double x, double y, double gx, double gy, double dx, double dy, QString Info = "")
        : mdX0(gx - x * dx), mdY0(gy - y * dy), mdScaleX(dx), mdScaleY(dy), mqstrCoorInfo(Info), mbFlag(true)  {}
    explicit SCoordinate(QPointF Logic, QPointF Geo, double dx, double dy, QString Info = "")
        : mdX0(Geo.x() - Logic.x() * dx), mdY0(Geo.y() - Logic.y() * dy), mdScaleX(dx), mdScaleY(dy), mqstrCoorInfo(Info), mbFlag(true)  {}

    /*-----成员变量-----*/
private:
    //默认的地理坐标和像素坐标等价
    double mdX0 = 0, mdY0 = 0;
    double mdScaleX = 1, mdScaleY = -1;
    //TODO:可以改成坐标信息类
    QString mqstrCoorInfo;
    bool mbFlag = false;

    /*-----成员函数-----*/
public:

    //-----访问与修改函数-----//
    inline double x0() const {return mdX0;}
    inline double y0() const {return mdY0;}
    inline double deltaX() const {return mdScaleX;}
    inline double deltaY() const {return mdScaleY;}
    inline QString info() const {return mqstrCoorInfo;}

    inline void setX0(double x0) {mbFlag = true; mdX0 = x0;}
    inline void setY0(double y0) {mbFlag = true; mdY0 = y0;}
    inline void setDeltaX(double dx) {mbFlag = true; mdScaleX = dx;}
    inline void setDeltaY(double dy) {mbFlag = true; mdScaleY = dy;}
    inline void setInfo(QString info) {mbFlag = true; mqstrCoorInfo = info;}


    //------功能函数------//
public:
    void mapToGeo(double logicX, double logicY, double& geoX, double& geoY)
    {
        geoX = mdX0 + mdScaleX * logicX;
        geoY = mdY0 + mdScaleY * logicY;
    }

    void mapFromGeo(double geoX, double geoY, double& logicX, double& logicY)
    {
        logicX = (geoX - mdX0) / mdScaleX;
        logicY = (geoY - mdY0) / mdScaleY;
    }
    bool isEmpty() {return !mbFlag;}
    void clear() {mbFlag = false; mdX0 = 0, mdY0 = 0; mdScaleX = 1, mdScaleY = 1; mqstrCoorInfo = QString();}
    QString projCS()
    {
        QRegExp rx("(PROJCS\\[\")([\\w]*)(\")");
        if (rx.indexIn(mqstrCoorInfo) != -1)
        {
            QString temp = rx.cap(2);
            return "PROJCS: " + temp.replace(QRegExp("_"), " ");
        }
        else
            return QString("Unknown PROJCS");
    }
};


#endif // SCOORDINATE_H
