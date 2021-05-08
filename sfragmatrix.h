#ifndef SFRAGMATRIX_H
#define SFRAGMATRIX_H

#include <simage.h>

class SFragMatrix
{
    /*-----构造函数与析构函数-----*/
public:
    explicit SFragMatrix(size_t _rows, size_t _cols, QStringList _dataPath);
    explicit SFragMatrix(size_t _rows, size_t _cols, QString *_dataPath, size_t _count);
    explicit SFragMatrix(size_t _rows, size_t _cols, std::vector<QString> &_dataPath);
    explicit SFragMatrix(size_t _rows, size_t _cols, SImage **_data, size_t _count);
    explicit SFragMatrix(size_t _rows, size_t _cols, std::vector<SImage *> &_data);
    explicit SFragMatrix(size_t _rows, size_t _cols);
    //拷贝构造函数将占有初始化数据的资源
    SFragMatrix(const SFragMatrix& mat);
    SFragMatrix(SFragMatrix&& mat)noexcept;
    ~SFragMatrix();

    //赋值运算符函数
    SFragMatrix& operator=(const SFragMatrix& mat);
    SFragMatrix& operator=(SFragMatrix&& mat)noexcept;

    /*-----成员变量-----*/
private:
    //数据
    SImage **data{};
    //数据路径
    QString *dataPath{};
    //行列数
    size_t rows{}, cols{};
    //用来存放金字塔层级参数
    size_t originalWidth{}, originalHeight{};
    size_t level{};
    size_t levelWidth{}, levelHeight{};
    size_t fragWidth{}, fragHeight{};

    //计算得到的该层的实际标准分片大小（缩放至level倍）
    size_t scaledFragWidth{};
    size_t scaledFragHeight{};
    //计算得到的该层的实际边缘分片大小（缩放至level倍）
    size_t scaledEdgeFragWidth{};
    size_t scaledEdgeFragHeight{};
    //计算得到的图像中心坐标
    QPointF ptLevelCenter{};

    /*-----成员函数-----*/
public:
    //[访问函数]
    //重载括号运算符
    inline SImage *&operator()(size_t row, size_t col);
    inline QString &pathAt(size_t row, size_t col);
    SFragMatrix block(size_t begin_row, size_t row_span, size_t begin_col, size_t col_span);
    inline SImage **getData()const;

    //元数据
    inline size_t Rows()const;
    inline size_t Cols()const;
    inline size_t OriginalWidth()const;
    inline size_t OriginalHeight()const;
    inline size_t Level()const;
    inline size_t LevelWidth()const;
    inline size_t LevelHeight()const;
    inline size_t FragWidth()const;
    inline size_t FragHeight()const;

    //[修改函数]
    void clear();
    //修改金字塔层级描述数据
    inline void setLevelMeta(size_t _original_width, size_t _original_height,
                             size_t _level,
                             size_t _level_width, size_t _level_height,
                             size_t _frag_width, size_t _frag_height);
    inline void setPathAt(size_t row, size_t col, const QString& path);
    inline void loadDataAt(size_t row, size_t col, const QString& _path = "");

    //[功能函数]
private:
    //根据元数据计算第row行第col列的分片影像的中心坐标
    QPointF _centerAt(size_t row, size_t col);
    //分配内存
    inline void _allocAll();
    //使用已有对象初始化
    inline void _initializeWith(const SFragMatrix& mat);
    //释放所有资源
    void _releaseAll();
};

SImage *&SFragMatrix::operator()(size_t row, size_t col)
{
    Q_ASSERT(row < rows && col < cols);
    return data[row * cols + col];
}

QString &SFragMatrix::pathAt(size_t row, size_t col)
{
    Q_ASSERT(row < rows && col < cols);
    return dataPath[row * cols + col];
}

SImage **SFragMatrix::getData() const
{
    return data;
}

size_t SFragMatrix::Rows()const
{
    return rows;
}

size_t SFragMatrix::Cols()const
{
    return cols;
}

size_t SFragMatrix::OriginalWidth() const
{
    return originalWidth;
}

size_t SFragMatrix::OriginalHeight() const
{
    return originalHeight;
}

size_t SFragMatrix::Level()const
{
    return level;
}

size_t SFragMatrix::LevelWidth()const
{
    return levelWidth;
}

size_t SFragMatrix::LevelHeight()const
{
    return levelHeight;
}

size_t SFragMatrix::FragWidth()const
{
    return fragWidth;
}

size_t SFragMatrix::FragHeight()const
{
    return fragHeight;
}

void SFragMatrix::setLevelMeta(size_t _original_width, size_t _original_height,
                               size_t _level,
                               size_t _level_width, size_t _level_height,
                               size_t _frag_width, size_t _frag_height)
{
    originalWidth = _original_width;
    originalHeight = _original_height;
    level = _level;
    levelWidth = _level_width;
    levelHeight = _level_height;
    fragWidth = _frag_width;
    fragHeight = _frag_height;

    //计算标准分片缩放后大小
    scaledFragWidth = fragWidth * level;
    scaledFragHeight = fragHeight * level;
    //计算边缘分片缩放后大小(全部为标准分片时，即为标准分片缩放后大小)
    scaledEdgeFragWidth = originalWidth - scaledEdgeFragWidth * (cols - 1);
    scaledEdgeFragHeight = originalHeight - scaledEdgeFragHeight * (rows - 1);
    //计算图像中心坐标
    ptLevelCenter.setX(static_cast<double>(originalWidth) / 2.0);
    ptLevelCenter.setY(static_cast<double>(originalHeight) / 2.0);
}

void SFragMatrix::setPathAt(size_t row, size_t col, const QString& path)
{
    this->pathAt(row, col) = path;
}

void SFragMatrix::loadDataAt(size_t row, size_t col, const QString &_path)
{
    QString &path = pathAt(row, col);
    if(!_path.isEmpty())
        path = _path;
    SImage*& pImage = this->operator()(row, col);
    if(pImage)
        delete pImage;
    Q_ASSERT(!path.isEmpty());
    pImage = new SImage(PaintObject::ImageBase, new QPixmap(_path), false, _centerAt(row, col));
    //缩放分片以使其填充图像区域
    pImage->scale(level, level);
}

void SFragMatrix::_allocAll()
{
    this->data = new SImage*[rows * cols];
    this->dataPath = new QString[rows * cols];
}

void SFragMatrix::_initializeWith(const SFragMatrix &mat)
{
    this->rows = mat.rows;
    this->cols = mat.cols;

    _allocAll();

    memcpy_s(this->data, rows * cols * sizeof(SImage*), mat.data, rows * cols * sizeof(SImage*));
    memcpy_s(this->dataPath, rows * cols * sizeof(QString), mat.dataPath, rows * cols * sizeof(QString));

    setLevelMeta(mat.originalWidth, mat.originalHeight, mat.level, mat.levelWidth, mat.levelHeight, mat.fragWidth, mat.fragHeight);
}


#endif // SFRAGMATRIX_H
