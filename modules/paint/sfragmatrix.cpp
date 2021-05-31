#include "sfragmatrix.h"

SFragMatrix::SFragMatrix(size_t _rows, size_t _cols, const QString &_dataPath): SFragMatrix(_rows, _cols)
{
    setLevelPath(_dataPath);
}

SFragMatrix::SFragMatrix(const QString &_dataPath)
{
    setLevelPath(_dataPath);
}

SFragMatrix::SFragMatrix(size_t _rows, size_t _cols, SImage *_data, size_t _count): SFragMatrix(_rows, _cols)
{
    Q_ASSERT(_count >= _rows * _cols);
    for(size_t i = 0; i < _rows * _cols; ++i)
        data[i] = _data[i];
}

SFragMatrix::SFragMatrix(size_t _rows, size_t _cols, std::vector<SImage> &_data): SFragMatrix(_rows, _cols)
{
    Q_ASSERT(_data.size() >= _rows * _cols);
    for(size_t i = 0; i < _rows * _cols; ++i)
        data[i] = _data[i];
}

SFragMatrix::SFragMatrix(size_t _rows, size_t _cols): rows(_rows), cols(_cols)
{
    _reAllocAll();
}

SFragMatrix::SFragMatrix(const SFragMatrix &mat)
{
    _initializeWith(mat);
}

SFragMatrix::~SFragMatrix()
{
    _releaseAll();
}

SFragMatrix &SFragMatrix::operator=(const SFragMatrix &mat)
{
    if(this == &mat)
        return *this;

    _releaseAll();
    _initializeWith(mat);

    return *this;
}


std::vector<SImage*> SFragMatrix::block(size_t begin_row, size_t row_span, size_t begin_col, size_t col_span)const
{
    Q_ASSERT(begin_row + row_span <= rows && begin_col + col_span <= cols);

    std::vector<SImage*> blockVec;
    blockVec.reserve(row_span * col_span);

    for(size_t i = 0; i < row_span; ++i)
        for(size_t j = 0; j < col_span; ++j)
            blockVec.push_back(data + (i + begin_row) * cols + j + begin_col);

    return blockVec;
}

#define SET_BETWEEN(X,A,B) X = X < A ? A : X > B ? B : X

std::vector<SImage*> SFragMatrix::block(QRectF rect)const
{
    //推算出左上角点和右下角点所在的影像行列号
    int row_begin, row_end, col_begin, col_end;
    //转换到左上角坐标系下
    rect.translate(ptLevelCenter);
    QPointF topLeft = rect.topLeft(), bottomRight = rect.bottomRight();
    row_begin = topLeft.y() / scaledFragHeight;
    col_begin = topLeft.x() / scaledFragWidth;
    row_end = bottomRight.y() / scaledFragHeight;
    col_end = bottomRight.x() / scaledFragWidth;
    //这里没有对边缘区块做处理
    SET_BETWEEN(row_begin, 0, static_cast<int>(rows) - 1);
    SET_BETWEEN(row_end, 0, static_cast<int>(rows) - 1);
    SET_BETWEEN(col_begin, 0, static_cast<int>(cols) - 1);
    SET_BETWEEN(col_end, 0, static_cast<int>(cols) - 1);

    return block(row_begin, row_end - row_begin + 1, col_begin, col_end - col_begin + 1);
}

void SFragMatrix::loadBlockArea(const QRectF &rect, SFragLoader &loader) const
{
    //确定需要加载的分块影像
    std::vector<SImage*> loadBlockVec = this->block(rect);

    //调用加载器线程加载图像
    if(!loadBlockVec.empty())
        loader.push_front(loadBlockVec.data(), loadBlockVec.size());
}

void SFragMatrix::loadAll()const
{
    for(size_t i = 0; i < rows * cols; ++i)
        data[i].load();
}

void SFragMatrix::setHistEqFunc(std::shared_ptr<void> pEqFunc[])
{
    for(size_t i = 0; i < rows * cols; ++i)
        data[i].setHistEqFunc(pEqFunc);
}

void SFragMatrix::setBandIndices(int r, int g, int b)
{
    for(size_t i = 0; i < rows * cols; ++i)
        data[i].setBandIndices(r, g, b, false);
}

void SFragMatrix::setLevelPath(const QString &path)
{
    strLevelPath = path;

    if(data)
        for(size_t i = 0; i < rows * cols; ++i)
            data[i].setImagePath(strLevelPath);
}

void SFragMatrix::paint(QPainter &painter, const QRectF& viewLgcArea_centered) const
{
#define USE_STRATEGY_11
#ifdef USE_STRATEGY_1
    std::vector<SImage*> blockVec = block(viewLgcArea_centered);
    for(auto &pImage : blockVec)
        pImage->paint(painter);
#else
    size_t dataSize = rows * cols;
    for(size_t i = 0; i < dataSize; ++i)
        data[i].paint(painter);
#endif
}

QPointF SFragMatrix::_centerAt(size_t row, size_t col)
{
    QPointF center(scaledFragWidth * col - ptLevelCenter.x(), scaledFragHeight * row - ptLevelCenter.y());

    if(col != cols - 1)
        center.rx() += static_cast<double>(scaledFragWidth) / 2.0;
    else
        center.rx() += static_cast<double>(scaledEdgeFragWidth) / 2.0;

    if(row != rows - 1)
        center.ry() += static_cast<double>(scaledFragHeight) / 2.0;
    else
        center.ry() += static_cast<double>(scaledEdgeFragHeight) / 2.0;

    return center;
}

QRect SFragMatrix::_fragRectAt(size_t row, size_t col)
{
    int left = fragWidth * col;
    int top = fragHeight * row;
    int width = (col == cols - 1 ? edgeFragWidth : fragWidth);
    int height = (row == rows - 1 ? edgeFragHeight : fragHeight);
    return QRect(left, top, width, height);
}

void SFragMatrix::_reAllocAll(size_t _rows, size_t _cols)
{
    rows = _rows;
    cols = _cols;
    _reAllocAll();
}

void SFragMatrix::_releaseAll()
{
    if(data)
        delete []data;
    data = nullptr;
}

void SFragMatrix::setLevelMeta(size_t _original_width,
                               size_t _original_height,
                               size_t _level_width,
                               size_t _level_height,
                               size_t _frag_width,
                               size_t _frag_height)
{
    originalWidth = _original_width;
    originalHeight = _original_height;
    levelWidth = _level_width;
    levelHeight = _level_height;
    fragWidth = _frag_width;
    fragHeight = _frag_height;

    //当前层放大多少倍得到原始影像大小
    double scaleFactorX = static_cast<double>(originalWidth) / levelWidth;
    double scaleFactorY = static_cast<double>(originalHeight) / levelHeight;

    //重新计算分片行列数
    size_t newCols = ceil(static_cast<double>(levelWidth) / fragWidth);
    size_t newRows = ceil(static_cast<double>(levelHeight) / fragHeight);

    if(newRows * newCols != rows * cols)
        _reAllocAll(newRows, newCols);

    rows = newRows;
    cols = newCols;

    setLevelPath(strLevelPath);

    //计算边缘分片大小
    edgeFragWidth = levelWidth - fragWidth * (cols - 1);
    edgeFragHeight = levelHeight - fragHeight * (rows - 1);
    //计算标准分片缩放后大小
    scaledFragWidth = fragWidth * scaleFactorX;
    scaledFragHeight = fragHeight * scaleFactorY;
    //计算边缘分片缩放后大小(全部为标准分片时，即为标准分片缩放后大小)
    scaledEdgeFragWidth = edgeFragWidth * scaleFactorX;
    scaledEdgeFragHeight = edgeFragHeight * scaleFactorY;
    //计算图像中心坐标
    ptLevelCenter.setX(static_cast<double>(originalWidth) / 2.0);
    ptLevelCenter.setY(static_cast<double>(originalHeight) / 2.0);

    //设置分片影像缩放量与中心点
    for(size_t i = 0; i < rows; ++i)
        for(size_t j = 0; j < cols; ++j)
        {
            SImage &rImg = this->operator()(i, j);
            rImg.setCenterPoint(_centerAt(i, j));
            rImg.setScaleFactor(scaleFactorX, scaleFactorY);
            QRect fragRect = _fragRectAt(i, j);
            rImg.setLoadRegionRect(fragRect);
            rImg.setLoadRegionResampledSize(fragRect.size());
        }
}



