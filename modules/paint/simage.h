#ifndef SIMAGE_H
#define SIMAGE_H

#include "modules/paint/sshape.h"
#include "QImage"

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
    explicit SImage(const QString& _imagePath = "", bool _selected = true, QPointF center = QPointF(),
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
    //是否包含某点(画布坐标系)
    virtual bool contains(const QPointF& pt)const;
    //输出与输入
    virtual void writeBinaryData(QDataStream& stream)const;
    virtual void readBinaryData(QDataStream& stream);

private:
    //应用变换到成员
    virtual void _applyTransform();

    /*-----属性-----*/
private:

    /*-----成员变量-----*/
private:
    const QString Me = "ImageBase";
    //图像文件路径
    QString mStrImagePath{};
    //图像
    QPixmap *mpImage{nullptr};
    //图像在无缩放无旋转下的包围矩形
    QRectF mImageRect;
    //图像的包围控制点（变换后）
    QPointF mpBoundPt[4] {};

    /*-----成员函数-----*/
public:
    //[访问函数]
    inline const QPixmap& getPixmap();
    inline const QString& getImagePath();
    inline bool isNull()const;

    //[修改函数]
    //加载图片
    void load(const QString& _imagePath = "");
    //设置路径（不加载图片）
    inline void setImagePath(const QString& imagePath);
    inline void setPixmap(const QPixmap& pixmap);
    //释放图片内存
    inline void releaseImage();

    //[功能函数]

private:
    void _initializeWith(const SImage& theImage);
};

const QPixmap &SImage::getPixmap()
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

void SImage::setPixmap(const QPixmap &pixmap)
{
    if(!pixmap.isNull())
        mpImage = new QPixmap(pixmap);
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
