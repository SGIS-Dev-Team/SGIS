#ifndef SFRAGIMAGE_H
#define SFRAGIMAGE_H

#include "modules/paint/sobject.h"
#include "modules/paint/sfragmatrix.h"
#include "modules/paint/sfragloader.h"
#include "modules/doc/simagestreammeta.h"

//----------------------------------------
//              SFragImage
//             分片大型图像类
//  paint虚函数需要传入画布显示范围和缩放量
//  目前使用逻辑分片策略
//
//
//
//
//----------------------------------------


class SFragImage : public SObject
{
    Q_OBJECT
    /*-----构造函数与析构函数-----*/
public:
    explicit SFragImage(SFragLoader& _loader,
                        bool _selected = true,
                        QPointF center = QPointF(),
                        const QString& _layerName = "",
                        const QString& _layerDiscription = "",
                        const QColor& _layerColor = "");
    explicit SFragImage(const SImageStreamMeta &_streamMeta,
                        SFragLoader &_loader,
                        bool _selected = true,
                        QPointF center = QPointF(),
                        const QString& _layerName = "",
                        const QString& _layerDiscription = "",
                        const QColor& _layerColor = "");
    virtual ~SFragImage();
    /*-----虚函数重载-----*/
public:
    //绘制函数
    virtual void paint(QPainter &painter,
                       bool doTranslate = true,
                       const QRectF & viewLogicalArea = QRectF(),
                       double scaleValue = 0,
                       PaintTrigger trigger = User_Trigger)const;
    //获取包围矩形（变换后），该矩形与相关Qt绘图类的boundingRect有所不同，是由原矩形进行缩放和旋转变换得到的。
    virtual QPolygonF boundingRect()const;
    //是否包含某点，若参数2为True,则只判断包围矩形是否包含某点
    virtual bool contains(const QPointF& pt, bool isInBoundRect = false)const;
    //输出与输入
    virtual void writeBinaryData(QDataStream& stream)const;
    virtual void readBinaryData(QDataStream& stream);
    //改变缩略图标生成策略
    virtual QIcon icon()const;

private:
    //应用变换的方法
    virtual void _applyTransform();
    //变换前的原始矩形，用于绘制选框矩形
    virtual QRectF _originalRect();

    /*-----信号-----*/
signals:
    void paintFrag(QPainter &painter)const;
    void loadFrag()const;

    /*-----槽函数-----*/
private slots:
    void onOverviewsReady(QString pyramidDir);

    /*-----属性-----*/
protected:
    //实际边界角点
    QPointF mpBoundPt[4] {};
    //原始图像边界矩形（变换前）
    QRectF mImageRect{};
    //是否常驻顶层缩略图
    bool mbHoldTopPyramid{false};

    /*-----成员变量-----*/
protected:
    //[Me]
    const QString Me = "Frag Image Base";
    //每一层的金字塔分片图像: 索引值为金字塔层级倒数取1/2为底的对数
    std::vector<SFragMatrix> mFragMatVec{};
    //加载器
    SFragLoader& mFragLoader;
    //文件夹路径
    QString mStrDirPath;

    /*-----成员函数-----*/
public:
    //[访问函数]
    inline int width()const;
    inline int height()const;
    inline const QSize &size()const;
    inline SFragLoader &getFragLoader() {return this->mFragLoader;}
    inline const SFragLoader &getFragLoader()const {return this->mFragLoader;}

    //[修改函数]
    //设置影像金字塔文件路径，影像文件名（不要带格式后缀）
    void setPyramidDir(const QString& dirPath);
    void setHistEqFunc(std::shared_ptr<void> pEqFunc[]);
    void setBandIndices(int r, int g, int b);
    //常驻顶层金字塔图像
    void setHoldTopPyramidEnabled(bool hold);

    //[功能函数]
public:
    //读取元数据
    void loadMeta(const QString &pyramidDir = "");
private:
    //初始化链接
    void _initializeConnections();
    void _destroyConnections();
};

#endif // SFRAGIMAGE_H
