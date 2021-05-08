#include "sfragmatrix.h"

SFragMatrix::SFragMatrix(size_t _rows, size_t _cols, QString *_dataPath, size_t _count): SFragMatrix(_rows, _cols)
{
    Q_ASSERT(_count >= _rows * _cols);
    for(size_t i = 0; i < _rows * _cols; ++i)
    {
        dataPath[i] = _dataPath[i];
        _dataPath[i].clear();
    }
}

SFragMatrix::SFragMatrix(size_t _rows, size_t _cols, std::vector<QString> &_dataPath): SFragMatrix(_rows, _cols)
{
    Q_ASSERT(_dataPath.size() >= _rows * _cols);
    for(size_t i = 0; i < _rows * _cols; ++i)
    {
        dataPath[i] = _dataPath[i];
        _dataPath[i].clear();
    }
}

SFragMatrix::SFragMatrix(size_t _rows, size_t _cols, SImage **_data, size_t _count): SFragMatrix(_rows, _cols)
{
    Q_ASSERT(_count >= _rows * _cols);
    for(size_t i = 0; i < _rows * _cols; ++i)
    {
        data[i] = _data[i];
        _data[i] = nullptr;
    }
}

SFragMatrix::SFragMatrix(size_t _rows, size_t _cols, std::vector<SImage *> &_data): SFragMatrix(_rows, _cols)
{
    Q_ASSERT(_data.size() >= _rows * _cols);
    for(size_t i = 0; i < _rows * _cols; ++i)
        data[i] = _data[i];
    _data.clear();
}

SFragMatrix::SFragMatrix(size_t _rows, size_t _cols): rows(_rows), cols(_cols)
{
    Q_ASSERT(_rows && _cols);
    _allocAll();
}

SFragMatrix::SFragMatrix(const SFragMatrix &mat)
{
    _initializeWith(mat);
}

SFragMatrix::SFragMatrix(SFragMatrix &&mat) noexcept
{
    _releaseAll();
    _initializeWith(mat);
    mat._releaseAll();
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

SFragMatrix &SFragMatrix::operator=(SFragMatrix &&mat) noexcept
{
    _releaseAll();
    _initializeWith(mat);
    mat._releaseAll();

    return *this;
}

SFragMatrix SFragMatrix::block(size_t begin_row, size_t row_span, size_t begin_col, size_t col_span)
{
    Q_ASSERT(begin_row + row_span <= rows && begin_col + col_span <= cols);
    SFragMatrix mat(row_span, col_span);

    for(size_t i = 0; i < row_span; ++i)
        for(size_t j = 0; j < col_span; ++j)
        {
            size_t index_mat = i * cols + j,
                   index_this = (i + begin_row) * cols + j + begin_col;
            mat.data[index_mat] = data[index_this];
            mat.dataPath[index_mat] = dataPath[index_this];
        }

    return mat;
}

void SFragMatrix::clear()
{
    for(size_t i = 0; i < rows * cols; ++i)
    {
        SImage*& pImage = data[i];
        if(pImage)
            delete pImage;
        pImage = nullptr;
        dataPath[i].clear();
    }
}

QPointF SFragMatrix::_centerAt(size_t row, size_t col)
{
    QPointF center(scaledEdgeFragWidth * col - ptLevelCenter.x(), scaledEdgeFragHeight * row - ptLevelCenter.y());

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
    clear();
    delete []data;
    delete []dataPath;
}
