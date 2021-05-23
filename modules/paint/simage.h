#ifndef SIMAGE_H
#define SIMAGE_H

#include "modules/paint/sshape.h"
#include "QImage"
#include "gdal_priv.h"

//------------------------
//      SImage类
//      图像类
//      包含图像对象指针
//      和4个图像包围控制点
//------------------------

class SImage : public SObject
{
    /*-----构造函数与析构函数-----*/
public:

    //注意：该构造函数不会加载影像
    explicit SImage(const QString& _imagePath = "", bool _selected = false, QPointF center = QPointF(),
                    const QString& _layerName = "",
                    const QString& _layerDiscription = "",
                    const QColor& _layerColor = "");
    SImage(const SImage& theImage);
    virtual ~SImage();

    SImage &operator=(const SImage& theImage);

    /*-----虚函数重载-----*/
public:
    //绘制函数
    virtual void paint(QPainter &painter, bool doTranslate = true, QRectF viewLogicalArea = QRectF(), double scaleValue = 0)const;
    //获取包围矩形
    virtual QPolygonF boundingRect()const;
    //是否包含某点(画布坐标系)，若参数2为True,则只判断包围矩形是否包含某点
    virtual bool contains(const QPointF& pt, bool isInBoundRect = false)const;
    //输出与输入
    virtual void writeBinaryData(QDataStream& stream)const;
    virtual void readBinaryData(QDataStream& stream);

private:
    //应用变换的方法
    virtual void _applyTransform();
    //变换前的原始矩形，用于绘制选框矩形
    virtual QRectF _originalRect();

    /*-----属性-----*/
private:
    //波段数
    int mnBands{0};
    //加载的波段
    int mnRedBandIdx{1};
    int mnGreenBandIdx{1};
    int mnBlueBandIdx{1};

    /*-----成员变量-----*/
private:
    const QString Me = "ImageBase";
    //图像文件路径
    QString mStrImagePath{};
    //图像
    QImage *mpImage{nullptr};
    //图像在无缩放无旋转下的包围矩形
    QRectF mImageRect;
    //图像的包围控制点（变换后）
    QPointF mpBoundPt[4] {};
    //均衡化函数(三个波段的)
    std::shared_ptr<void> mpEqualizeFunc[3] {nullptr};

    /*-----成员函数-----*/
public:

    //[访问函数]

    inline const QImage& getImage();
    inline const QString& getImagePath();
    inline bool isNull()const;
    int redBandIdx() const;
    int greenBandIdx() const;
    int blueBandIdx() const;
    bool isMultiBand()const;
    void getHistEqFunc(std::shared_ptr<void> pEqFunc[]);
    int getBandCount() const;

    void getBandIdices(int *pRGBIdx) const;

    //[修改函数]

    //加载图片
    void load(const QString& _imagePath = "");
    bool save(const QString& _savePath);
    //设置路径（不加载图片）
    inline void setImagePath(const QString& imagePath);
    inline void setImage(const QImage& image);
    //释放图片内存
    inline void releaseImage();
    //改变波段
    void setRedBandIdx(int value);
    void setGreenBandIdx(int value);
    void setBlueBandIdx(int value);
    //设置波段：从1开始
    void setBandIndices(int r, int g, int b);
    //设置波段为默认值
    void setDefaultedBands();
    //设置均衡化函数
    void setHistEqFunc(std::shared_ptr<void> pEqFunc[]);

    //[功能函数]

public:
    //获取直方图均衡化函数
    static std::shared_ptr<void> calcHistEqFunc(GDALDataType type, void* pBandData, size_t count);
    //对波段执行直方图均衡化
    static void histEqualize(GDALDataType type, void* pBandData, size_t count, std::shared_ptr<void> pEqFunc);
    //对波段进行8位转换;算法:按数值范围比例缩放
    static uchar *to8bit(GDALDataType type, void* pBandData, size_t count);

private:
    void _initializeWith(const SImage& theImage);

};

const QImage &SImage::getImage()
{
    return *mpImage;
}

const QString &SImage::getImagePath()
{
    return mStrImagePath;
}

void SImage::setImagePath(const QString &imagePath)
{
    mStrImagePath = imagePath;
}

void SImage::setImage(const QImage &image)
{
    if(!image.isNull())
        mpImage = new QImage(image);
    mStrImagePath = nullptr;
}

void SImage::releaseImage()
{
    if(mpImage)
        delete mpImage;
    mpImage = nullptr;
}

bool SImage::isNull()const
{
    return !mpImage;
}


#endif // SIMAGE_H
