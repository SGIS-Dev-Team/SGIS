#ifndef SFRAGMATRIX_H
#define SFRAGMATRIX_H

#include <modules/paint/simage.h>

class SFragMatrix
{
    /*-----构造函数与析构函数-----*/
public:
    explicit SFragMatrix(size_t _rows, size_t _cols, QStringList _dataPath);
    explicit SFragMatrix(size_t _rows, size_t _cols, QString *_dataPath, size_t _count);
    explicit SFragMatrix(size_t _rows, size_t _cols, std::vector<QString> &_dataPath);
    explicit SFragMatrix(size_t _rows, size_t _cols, SImage *_data, size_t _count);
    explicit SFragMatrix(size_t _rows, size_t _cols, std::vector<SImage> &_data);
    explicit SFragMatrix(size_t _rows, size_t _cols);
    //拷贝构造函数
    SFragMatrix(const SFragMatrix& mat);
    ~SFragMatrix();

    //赋值运算符函数
    SFragMatrix& operator=(const SFragMatrix& mat);

    /*-----成员变量-----*/
private:
    //数据
    SImage *data{};
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
    inline SImage &operator()(size_t row, size_t col);
    //取出矩阵分块
    std::vector<SImage*> block(size_t begin_row, size_t row_span, size_t begin_col, size_t col_span)const;
    //使用中心坐标系
    std::vector<SImage*> block(QRectF rect)const;
    //获取数据
    inline SImage *getData()const;
    //是否为空
    inline bool isEmpty()const;

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
    //修改金字塔层级描述数据
    void setLevelMeta(size_t _original_width, size_t _original_height,
                      size_t _level,
                      size_t _level_width, size_t _level_height,
                      size_t _frag_width, size_t _frag_height);
    //加载全部影像数据
    void loadAll()const;

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

SImage &SFragMatrix::operator()(size_t row, size_t col)
{
    Q_ASSERT(row < rows && col < cols);
    return data[row * cols + col];
}

SImage *SFragMatrix::getData() const
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

void SFragMatrix::_allocAll()
{
    this->data = new SImage[rows * cols];
}

void SFragMatrix::_initializeWith(const SFragMatrix &mat)
{
    this->rows = mat.rows;
    this->cols = mat.cols;

    _allocAll();

    for(size_t i = 0; i < rows * cols; ++i)
        data[i] = mat.data[i];

    setLevelMeta(mat.originalWidth, mat.originalHeight, mat.level, mat.levelWidth, mat.levelHeight, mat.fragWidth, mat.fragHeight);
}

bool SFragMatrix::isEmpty()const
{
    return rows * cols == 0 ? true : false;
}

#endif // SFRAGMATRIX_H
