#ifndef SREADSHP_H
#define SREADSHP_H
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "cpl_conv.h"
#include <QVariant>



typedef std::map<int, std::vector<QVariant>> RecordMap;


//-----------------------------------
//          SReadShp
//          shp文件读取类
//  现在默认shp文件存储只有一个layer的点要素
//-----------------------------------


class SReadShp
{
public:
    explicit SReadShp() = default;
    explicit SReadShp(QString path): mstrShpPath(path) {};

    virtual ~SReadShp();

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


    //获取要素。
    //通过vector容器将属性表的行组织起来，每一行对应一个要素的具体信息
    //再用map将vector组织起来。若读取失败将返回空map
    //vector<QVariant>:
    //  QPointF       Int           ...
    //  点的二维坐标  点的OBJECTID  其他属性
    //其中vector容器的第0个存储每列相应属性字段的名称QString
    RecordMap getFeature();

};


#endif // SREADSHP_H
