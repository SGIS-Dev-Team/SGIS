#ifndef SIMAGE_H
#define SIMAGE_H

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
public:
    explicit SImageMeta(int width, int height, int bandCount, GDALDataType dataType):
        mnWidth(width), mnHeight(height), mnBandCount(bandCount), mnDataType(dataType)
    {
        Q_ASSERT(mnWidth > 0 && mnHeight > 0 && mnBandCount >= 0);
        Q_ASSERT(mnDataType >= 0 && mnDataType < GDT_TypeCount);
    }
    explicit SImageMeta(const QSize &imageSize, int bandCount, GDALDataType dataType):
        mnWidth(imageSize.width()), mnHeight(imageSize.height()), mnBandCount(bandCount), mnDataType(dataType)
    {
        Q_ASSERT(mnWidth > 0 && mnHeight > 0 && mnBandCount >= 0);
        Q_ASSERT(mnDataType >= 0 && mnDataType < GDT_TypeCount);
    }
    explicit SImageMeta() = default;
    SImageMeta(const SImageMeta &meta) = default;
    SImageMeta &operator=(const SImageMeta &meta) = default;
    virtual ~SImageMeta() {}

public:
    inline int width() const {return mnWidth;}
    inline int height() const {return mnHeight;}
    inline QSize imageSize()const {return QSize(mnWidth, mnHeight);}
    inline int bandCount() const {return mnBandCount;}
    inline GDALDataType dataType() const {return mnDataType;}

    inline int &rWidth() {return mnWidth;}
    inline int &rHeight() {return mnHeight;}
    inline void setImageSize(int w, int h) {Q_ASSERT(w > 0 && h > 0); mnWidth = w; mnHeight = h;}
    inline void setImageSize(const QSize &sz) {Q_ASSERT(sz.width() > 0 && sz.height() > 0); mnWidth = sz.width(); mnHeight = sz.height();}
    inline int &rBandCount() {return mnBandCount;}
    inline GDALDataType &rDataType() {return mnDataType;}

    inline bool isEmpty()const {return mnWidth == 0 || mnHeight == 0 || mnBandCount == 0;}
    inline void clear() {mnWidth = 0; mnHeight = 0; mnBandCount = 0; mnDataType = GDT_Byte;}
};

//------------------------
//      SImage类
//      多波段图像类
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
    virtual void paint(QPainter &painter,
                       bool doTranslate = true,
                       const QRectF &viewLogicalArea = QRectF(),
                       double scaleValue = 0,
                       PaintTrigger trigger = User_Trigger)const;
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
    //数据类型
    GDALDataType mnDataType{};
    //加载的波段
    int mnRedBandIdx{1};
    int mnGreenBandIdx{1};
    int mnBlueBandIdx{1};
    //从图像文件中读取的区域
    QRect mLoadRect{0, 0, 0, 0};
    //从图像文件中读取的区域重采样大小
    QSize mImageSize{};

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
    //均衡化函数(三个波段的)
    std::shared_ptr<void> mpEqualizeFunc[3] {nullptr};
    //图像波段数据
    std::unique_ptr<uchar[], std::default_delete<uchar[]>> mpBandData[3] {nullptr};
    //图像数据(8位、已均衡化)
    std::unique_ptr<uchar[], std::default_delete<uchar[]>> mpImageData{nullptr};
    //锁
    QMutex mMutex;

    /*-----成员函数-----*/
public:

    //[访问函数]

    const QPixmap& getImage()const;
    const QString& getImagePath()const;
    bool isNull()const;
    void getHistEqFunc(std::shared_ptr<void> pEqFunc[])const;

    int getBandCount() const;
    void getBandIdices(int *pRGBIdx) const;

    int redBandIdx() const;
    int greenBandIdx() const;
    int blueBandIdx() const;
    bool isMultiBand()const;

    const QRect &getLoadRegionRect()const;
    const QSize &getLoadResampledSize()const;

    //[修改函数]

    // 区域读取函数：使用set后的参数读取
    void load(const QString &imagePath = "");

    // 区域读取函数
    void load(int x_off, int y_off, int x_span, int y_span, const QString &imagePath = "");

    // 区域读取函数
    void load(const QRect &rect, const QString &imagePath = "");

    // 区域读取函数，该函数可以对超大图像进行采样，保证内存能够容纳采样后图像数据即可
    void load(int x_off, int y_off, int x_span, int y_span,
              int image_width, int image_height,
              const QString &imagePath = "");

    // 区域读取函数，该函数可以对超大图像进行采样，保证内存能够容纳采样后图像数据即可
    void load(const QRect &rect, double resampling_ratio, const QString &imagePath = "");

    // 区域读取函数，该函数可以对超大图像进行采样，保证内存能够容纳采样后图像数据即可
    void load(int x_off, int y_off, int x_span, int y_span, double resampling_ratio, const QString &imagePath = "");

    /* 区域读取函数，该函数可以对超大图像进行采样，保证内存能够容纳采样后图像数据即可
     * @param   x_off           读取区域横向偏移量，即起始列号，为0时从图像最左一列开始读取
     * @param   y_off           读取区域纵向偏移量，即起始行号，为0时从图像最上一行开始读取
     * @param   x_span          区域横向跨度，即区域宽度（列数），为0时读取x_off开始的所有行
     * @param   y_span          区域横向跨度，即区域高度（行数），为0时读取y_off开始的所有列
     * @param   image_width     对区域采样的图像宽
     * @param   image_height    对区域采样的图像高
     * @param   pDataSet        已经打开的数据集
     */
    void load(int x_off, int y_off, int x_span, int y_span,
              int image_width, int image_height,
              GDALDataset *pDataSet);


    bool save(const QString& _savePath);
    //设置路径（不加载图片）
    void setImagePath(const QString& imagePath);
    void setImage(const QPixmap& image);
    //释放图像内存
    void releaseImage();
    //释放所有内存
    void releaseMemory();
    //改变波段
    void setRedBandIdx(int value, bool reload, std::shared_ptr<void> pHistEqFunc = nullptr);
    void setGreenBandIdx(int value, bool reload, std::shared_ptr<void> pHistEqFunc = nullptr);
    void setBlueBandIdx(int value, bool reload, std::shared_ptr<void> pHistEqFunc = nullptr);
    //设置波段：从1开始
    void setBandIndex(int channel, int bandIdx, bool reload, std::shared_ptr<void> pHistEqFunc = nullptr);
    //预设置载入波段
    void presetBandIndices(int r, int g, int b);
    void presetBandIndices(int r, int g, int b, std::shared_ptr<void> pHistEqFuncs[]);
    //该函数将判断哪些波段需要重新载入
    void setBandIndices(int r, int g, int b);
    void setBandIndices(int r, int g, int b, std::shared_ptr<void> pHistEqFuncs[]);
    //设置波段为默认值
    void setDefaultedBands();
    //设置为默认波段但不加载
    void presetDefaultedBands();
    //设置均衡化函数
    void setHistEqFunc(std::shared_ptr<void> pEqFunc[]);
    //设置图像读取区域
    void setLoadRegionRect(const QRect &rect);
    //设置图像读取区域采样大小
    void setLoadRegionResampledSize(const QSize &size);

    //[功能函数]

public:
    //读取特定波段数据
    static std::unique_ptr<uchar[], std::default_delete<uchar[]>> loadBand(int x_off, int y_off, int x_span, int y_span,
            int image_width, int image_height,
            const QString &imagePath, int bandIdx, GDALDataType dataType,
            std::shared_ptr<void> pEqFunc = nullptr);

    static std::unique_ptr<uchar[], std::default_delete<uchar[]>> loadBand(int x_off, int y_off, int x_span, int y_span,
            int image_width, int image_height,
            GDALDataset *pDataSet, int bandIdx, GDALDataType dataType,
            std::shared_ptr<void> pEqFunc = nullptr);

    //融合为逐像素存储图像
    static std::unique_ptr<uchar[], std::default_delete<uchar[]>> merge(const uchar *pBandData[], GDALDataType dataType, int pixelCount, int bandCount = 3);
    //计算直方图均衡化函数
    static std::shared_ptr<void> calcHistEqFunc(GDALDataType type, const uchar* pBandData, size_t count);
    //对波段执行直方图均衡化
    static void histEqualize(GDALDataType type, uchar *pBandData, size_t count, const void *pEqFunc);
    //对波段进行8位转换;算法:按数值范围比例缩放
    static std::unique_ptr<uchar[], std::default_delete<uchar[]>> to8bit(GDALDataType type, const uchar *pBandData, size_t count);
    //获取目标影像的描述数据
    static SImageMeta getMetaOf(QString imagePath);
    static SImageMeta getMetaOf(GDALDataset *pDataSet);

private:
    void _initializeWith(const SImage &theImage);
    //打开数据集
    static GDALDataset *getOpenDataSet(const QString &imagePath, GDALAccess access = GA_ReadOnly);
    //重新加载波段:channel取0（RED），1(GREEN)，2(BLUE)
    void _reloadChannel(int channel, int newBandIdx, std::shared_ptr<void> pHistEqFunc = nullptr);
    //更新图像原始坐标矩形和边界点
    void _updateImageRect();
    //使用图像数据更新图像
    void _updateImage();

};

template<typename T>
std::unique_ptr<uchar[], std::default_delete<uchar[]>> _to8bit(const T* pBandData, size_t count, T divisor);

#endif // SIMAGE_H
