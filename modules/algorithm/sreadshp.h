#ifndef SREADSHP_H
#define SREADSHP_H
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "cpl_conv.h"
#include <QVariant>
#include <QRectF>

typedef std::map<int, std::vector<QVariant>> RecordMap;


//-----------------------------------
//          SReadShp
//          shp文件读取类
//  现在默认shp文件存储只有一个layer的要素
//  读取线要素暂时不会返回坐标
//-----------------------------------


class SReadShp
{
public:
    explicit SReadShp() = default;
    explicit SReadShp(QString path): mstrShpPath(path) {};
    virtual ~SReadShp() = default;

    /*-----虚函数重载-----*/
public:

    /*-----成员变量-----*/
private:
    //文件路径
    QString mstrShpPath;
    //图形类型
    OGRwkbGeometryType mGeoType = wkbUnknown;


    /*-----成员函数-----*/
public:
    //[访问函数]
    inline QString shpPath()const {return mstrShpPath;}
    inline OGRwkbGeometryType geoType() const {return mGeoType;}

    //[修改函数]
    inline void setShpPath(QString path) {mstrShpPath = path;}
    inline void setGeoType(OGRwkbGeometryType geoType) {mGeoType = geoType;}

    //[功能函数]
    inline bool isEmpty() {return mstrShpPath.isEmpty();}


    //获取要素。若isLatLon为false返回坐标为投影坐标，否则返回WGS84坐标(lat lon)
    //通过vector容器将属性表的行组织起来，每一行对应一个要素的具体信息
    //再用map将vector组织起来。若读取失败将返回空map
    //map<int,vector<QVariant>>:
    //其中map中0对应存储每列相应属性字段的名称QString的vector<QVariant>
    //vector<QVariant>:
    //  QString     QString     ...
    //  name1         name2   other name
    //其它编号1~n共n个，存储属性相应数据
    //vector<QVariant>:
    //  QPointF              Int           ...
    //  projcs(X,Y)/WGS84(Lat Lon)   OBJECTID    other attributes
    RecordMap getFeatureWithCoor(bool isLatLon = false);
    //获取层的坐标范围，若isLatLon为false返回坐标为投影坐标，否则返回WGS84坐标(lat lon)
    QRectF getboundingRect(bool isLatLon = false);
};


#endif // SREADSHP_H
