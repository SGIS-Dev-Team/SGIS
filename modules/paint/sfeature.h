#ifndef SFEATURE_H
#define SFEATURE_H

#include "sobject.h"
#include "modules/algorithm/scoordinate.h"

class SFeature : public SObject
{
    Q_OBJECT

    /*-----构造函数与析构函数-----*/
public:
    explicit SFeature(PaintObject _type, bool _selected = false, QPointF _center = QPointF(),
                      const QString& _layerName = "",
                      const QString& _layerDiscription = "",
                      const QColor& _layerColor = "");
    virtual ~SFeature();

    /*-----虚函数重载----*/
public:
    //返回包围矩形框（横平竖直的矩形）
    virtual QPolygonF boundingRect() const override;

    virtual QIcon icon() const override;
private:
    virtual void _applyTransform() override;
    virtual QRectF _originalRect() override;

    /*-----信号-----*/
signals:

    /*-----槽函数----*/
private slots:

    /*-----属性----*/
protected:

    /*-----成员变量-----*/
protected:
    //顶点的向量的向量，内层存放一个几何要素的顶点，外层是几何要素的集合
    std::vector<std::vector<QPointF>> mGeoPointsVecVec{};
    std::vector<std::vector<QPointF>> mItemPointsVecVec{};

    //样式
    QBrush mBrush{};
    QPen   mPen{};
    //坐标参考
    std::shared_ptr<SCoordinate> mpGeoRef{nullptr};

    bool mbModifying{false};

    //[Me]
    const QString Me = "ShapeLayerBase";

    /*-----成员函数-----*/
public:
    //[访问函数]
    const QBrush& getBrush()const {return mBrush;}
    QBrush& rBrush() {return mBrush;}

    const QPen& getPen()const {return mPen;}
    QPen& rPen() {return mPen;}

    std::shared_ptr<SCoordinate> getGeoReference() {return mpGeoRef;}

    const std::vector<std::vector<QPointF>>& getGeoPoints()const {return mGeoPointsVecVec;}
    std::vector<std::vector<QPointF>>& getGeoPoints() {return mGeoPointsVecVec;}

    //[修改函数]

    //修改顶点之前调用
    void beginModifyVertex() {mbModifying = true;}
    //修改顶点之后调用
    void endModifyVertex()   {if (mbModifying)_applyTransform(); mbModifying = false; }

    void setBrush(const QBrush& brush) {mBrush = brush;}
    void setPen(const QPen& pen) {mPen = pen;}

    void setGeoReference(std::shared_ptr<SCoordinate> pGeoRef) {mpGeoRef = pGeoRef;}

    void setGeoPoints(const std::vector<std::vector<QPointF>>& geoPtVecVec) {mGeoPointsVecVec = geoPtVecVec; _applyTransform();}

    //[功能函数]


};

#endif // SFEATURE_H
