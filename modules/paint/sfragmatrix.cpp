﻿#include "sfragmatrix.h"

SFragMatrix::SFragMatrix(size_t _rows, size_t _cols, QStringList _dataPath): SFragMatrix(_rows, _cols)
{
    Q_ASSERT((size_t)_dataPath.size() >= _rows * _cols);
    for(size_t i = 0; i < _rows * _cols; ++i)
        data[i].setImagePath(_dataPath[i]);
}

SFragMatrix::SFragMatrix(size_t _rows, size_t _cols, QString *_dataPath, size_t _count): SFragMatrix(_rows, _cols)
{
    Q_ASSERT(_count >= _rows * _cols);
    for(size_t i = 0; i < _rows * _cols; ++i)
        data[i].setImagePath(_dataPath[i]);
}

SFragMatrix::SFragMatrix(size_t _rows, size_t _cols, std::vector<QString> &_dataPath): SFragMatrix(_rows, _cols)
{
    Q_ASSERT(_dataPath.size() >= _rows * _cols);
    for(size_t i = 0; i < _rows * _cols; ++i)
        data[i].setImagePath(_dataPath[i]);
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
    //Q_ASSERT(_rows > 0 && _cols > 0);
    _allocAll();
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

void SFragMatrix::loadAll()const
{
    for(size_t i = 0; i < rows * cols; ++i)
        data[i].load();
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

void SFragMatrix::_releaseAll()
{
    delete []data;
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
    scaledEdgeFragWidth = originalWidth - scaledFragWidth * (cols - 1);
    scaledEdgeFragHeight = originalHeight - scaledFragHeight * (rows - 1);
    //计算图像中心坐标
    ptLevelCenter.setX(static_cast<double>(originalWidth) / 2.0);
    ptLevelCenter.setY(static_cast<double>(originalHeight) / 2.0);

    //设置分片影像缩放量与中心点
    for(size_t i = 0; i < rows; ++i)
        for(size_t j = 0; j < cols; ++j)
        {
            size_t index = i * cols + j;
            data[index].setCenterPoint(_centerAt(i, j));
            data[index].setScaleFactor(level, level);
        }

}