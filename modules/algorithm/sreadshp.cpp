#include "sreadshp.h"
#include <QPolygonF>

RecordMap SReadShp::getFeatureWithCoor(bool isLatLon)
{

    RecordMap result;
    std::vector<QVariant> tempVecQVar;
    int nIndex = 0;//表的行数，表示有多少个要素（从0开始，0代表要素属性的名称）

    //驱动注册和相关配置
    GDALAllRegister();
    GDALDataset* pDS;
    CPLSetConfigOption("SHAPE_ENCODING", ""); //解决中文乱码问题，但在qt里不知道会不会有问题？

    //含中文QString->char*
    QByteArray qbarr = mstrShpPath.toUtf8();// 不能两句合起来写
    //读取shp文件
    //这一步是类似打开tiff影像一样，打开一个dataset
    pDS = static_cast<GDALDataset*>(GDALOpenEx(qbarr.constData(), GDAL_OF_VECTOR, NULL, NULL, NULL));
    if (pDS == nullptr)
    {
        printf("Open failed");
        return RecordMap();
    }

    //从这步开始，获取shp文件中的特定一个层(目前我们默认只有一个层,所以不用遍历)
    //用GetLayerCount()和GetLayer()可以遍历层，下标是从0开始的
    OGRLayer* poLayer;
    pDS->GetLayerCount();
    poLayer = pDS->GetLayer(0); //读取层
    poLayer->ResetReading();

    //获取层的定义，读取属性字段
    //xxxDefn是用来获取类型、名称等信息的
    OGRFeature* poFeature = nullptr;
    OGRFeatureDefn* poFeatDefn = poLayer->GetLayerDefn();
    OGRFieldDefn* poFieldDefn = nullptr;

    if (isLatLon)
    {
        tempVecQVar.push_back(QString("Lat and Lon"));
    }
    else
    {
        tempVecQVar.push_back(QString("Projected Coordinate"));
    }

    //遍历字段名
    //GetFieldCount()和GetFieldDefn()可以遍历字段名
    //GetFieldDefn()有一个重载，也可以通过字段名获取字段
    int nFieldCount = poFeatDefn->GetFieldCount();  //获得字段的数目，不包括前两个字段（FID,Shape),
    for (int nField = 0; nField < nFieldCount; nField++)
    {
        poFieldDefn = poFeatDefn->GetFieldDefn(nField);
        tempVecQVar.push_back(QString(poFieldDefn->GetNameRef()));
    }
    result.insert(RecordMap::value_type(nIndex++, tempVecQVar));


    //用于获取坐标的转换
    poFeature = poLayer->GetNextFeature();//GetNextFeature()使用后用OGRFeature::DestroyFeature()删除，需在layer或dataset删除前删除
    OGRGeometry* poGeometry = poFeature->GetGeometryRef();
    OGRSpatialReference* prosrs = poGeometry->getSpatialReference();
    OGRSpatialReference geosrs = OGRSpatialReference();
    geosrs.SetWellKnownGeogCS("WGS84");
    OGRCoordinateTransformation* trans = OGRCreateCoordinateTransformation(prosrs, &geosrs);// 需要用OCTDestroyCoordinateTransformation()删除
    //重置读取的Feature
    OGRFeature::DestroyFeature(poFeature);
    poLayer->ResetReading();


    //每次读取一个要素的所有信息,直到GetNextFeature()返回NULL
    while ((poFeature = poLayer->GetNextFeature()) != nullptr)
    {
        tempVecQVar = std::vector<QVariant>
                      ();
        //获取几何属性
        OGRGeometry* poGeometry = poFeature->GetGeometryRef();
        if (poGeometry == nullptr)
        {
            OCTDestroyCoordinateTransformation(trans);
            OGRFeature::DestroyFeature(poFeature);
            GDALClose(pDS);
            return RecordMap();
        }
        //判断是不是点
        mGeoType = poGeometry->getGeometryType();
        switch (poGeometry->getGeometryType())
        {
        case wkbPoint:
        {
            //强制类型转换成相应子类，读取点坐标
            OGRPoint* poPoint = static_cast<OGRPoint*>(poGeometry);

            //这里是投影坐标
            double x = poPoint->getX();
            double y = poPoint->getY();
            if (isLatLon)
            {
                //转换成经纬度
                trans->Transform(1, &x, &y);
                tempVecQVar.push_back(QPointF(x, y));
            }
            else
            {
                //这里是投影坐标
                tempVecQVar.push_back(QPointF(x, y));
            }
            break;
        }
        case wkbLineString:
        {
            //强制类型转换成相应子类，读取线段结点坐标
            OGRLineString* poLineString = static_cast<OGRLineString*>(poGeometry);

            int nNumPoint = poLineString->getNumPoints();
            double x = 0, y = 0;

            if (isLatLon)
            {
                QPolygonF tempPolygonLatLon;
                for (int i = 0; i < nNumPoint; i++)
                {
                    //这里是投影坐标
                    x = poLineString->getX(i);
                    y = poLineString->getY(i);

                    //再转换成经纬度
                    trans->Transform(1, &x, &y);
                    tempPolygonLatLon << QPointF(x, y);
                }
                tempVecQVar.push_back(tempPolygonLatLon);
            }
            else
            {
                QPolygonF tempPolygonXY;
                for (int i = 0; i < nNumPoint; i++)
                {
                    //这里是投影坐标
                    x = poLineString->getX(i);
                    y = poLineString->getY(i);
                    tempPolygonXY << QPointF(x, y);
                }
                tempVecQVar.push_back(tempPolygonXY);
            }
            break;
        }
        case wkbPolygon:
        {
            //强制类型转换成相应子类，读取面边界结点坐标
            OGRPolygon* poPolygon = static_cast<OGRPolygon*>(poGeometry);
            OGRLinearRing* poBoundary = poPolygon->getExteriorRing();//不需要delete
            int nNumPoint = poBoundary->getNumPoints();
            double x = 0, y = 0;

            if (isLatLon)
            {
                QPolygonF tempPolygonLatLon;
                for (int i = 0; i < nNumPoint; i++)
                {
                    //这里是投影坐标
                    x = poBoundary->getX(i);
                    y = poBoundary->getY(i);

                    //再转换成经纬度
                    trans->Transform(1, &x, &y);
                    tempPolygonLatLon << QPointF(x, y);
                }
                tempVecQVar.push_back(tempPolygonLatLon);
            }
            else
            {
                QPolygonF tempPolygonXY;
                for (int i = 0; i < nNumPoint; i++)
                {
                    //这里是投影坐标
                    x = poBoundary->getX(i);
                    y = poBoundary->getY(i);
                    tempPolygonXY << QPointF(x, y);
                }
                tempVecQVar.push_back(tempPolygonXY);
            }
            break;
        }
        default:
            break;
        }

        //遍历每个属性（不包括FID和shape）
        for (int nField = 0; nField < nFieldCount; nField++)
        {
            poFieldDefn = poFeatDefn->GetFieldDefn(nField);
            switch (poFieldDefn->GetType())
            {
            //整型
            case OFTInteger:
            {
                tempVecQVar.push_back(poFeature->GetFieldAsInteger(nField));
                break;
            }
            //小数
            case OFTReal:
            {
                tempVecQVar.push_back(poFeature->GetFieldAsDouble(nField));
                break;
            }
            //字符串
            case OFTString:
            {
                tempVecQVar.push_back(QString(poFeature->GetFieldAsString(nField)));
                break;
            }
            default:
            {
                tempVecQVar.push_back(QString(poFeature->GetFieldAsString(nField)));
                break;
            }
            }
        }
        result.insert(RecordMap::value_type(nIndex++, tempVecQVar));
        OGRFeature::DestroyFeature(poFeature);//记得删除
    }
    OCTDestroyCoordinateTransformation(trans);//记得删除
    GDALClose(pDS);
    return result;
}

QRectF SReadShp::getboundingRect()
{
    //驱动注册和相关配置
    GDALAllRegister();
    GDALDataset* pDS;
    CPLSetConfigOption("SHAPE_ENCODING", "");  //解决中文乱码问题，但在qt里不知道会不会有问题？

    //含中文QString->char*
    QByteArray qbarr = mstrShpPath.toUtf8();// 不能两句合起来写
    //读取shp文件
    //这一步是类似打开tiff影像一样，打开一个dataset
    pDS = static_cast<GDALDataset*>(GDALOpenEx(qbarr.constData(), GDAL_OF_VECTOR, NULL, NULL, NULL));
    if (pDS == nullptr)
    {
        printf("Open failed");
        return QRectF();
    }

    //获取shp文件中的特定一个层
    OGRLayer* poLayer;
    pDS->GetLayerCount();
    poLayer = pDS->GetLayer(0); //读取层
    //获取范围
    OGREnvelope extent;
    if (poLayer->GetExtent(&extent) == OGRERR_FAILURE)
    {
        GDALClose(pDS);
        return QRectF();
    }
    GDALClose(pDS);
    return QRectF(QPointF(extent.MinX, extent.MaxY), QPointF(extent.MaxX, extent.MinY));
}
