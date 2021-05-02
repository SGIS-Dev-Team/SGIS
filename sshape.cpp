#include "sshape.h"

SShape::SShape(PaintObject _type, bool _selected, QPoint center,
               const QString &_layerName,
               const QString &_layerDiscription,
               const QColor &_layerColor)
    : SObject(_type, _selected, center, _layerName, _layerDiscription, _layerColor) {}

SShape::~SShape() {}

void SShape::paint(QPainter &painter) const
{
    //保存原来的样式
    const QPen& oldPen = painter.pen();
    const QBrush& oldBrush = painter.brush();
    //设置为本形状样式
    painter.setPen(mPen);
    painter.setBrush(mBrush);
    //平移到中心点
    painter.translate(mPtCenter);
    //绘图
    painter.drawPath(mPath);

    //返回原点
    painter.translate(-mPtCenter);
    //还原样式
    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

QRectF SShape::rect()
{
    //统计所有点xmin，xmax，ymin，ymax
    double xMin = DBL_MAX, xMax = DBL_MIN, yMin = DBL_MAX, yMax = DBL_MIN;
    for(auto& pt : mVerticesVec)
    {
        if(pt.x() > xMax) xMax = pt.x();
        if(pt.x() < xMin) xMin = pt.x();
        if(pt.y() > yMax) yMax = pt.y();
        if(pt.y() < yMin) yMin = pt.y();
    }
    QRectF bound_rect(this->CtoA(QPointF(xMin, yMin)), this->CtoA(QPointF(xMax, yMax)));
    return bound_rect;

}

bool SShape::contains(const QPointF &pt)
{
    this->updatePath();
    return mPath.contains(pt);
}

void SShape::tranlate(double dx, double dy)
{
    mPtCenter.rx() += dx;
    mPtCenter.ry() += dy;
    updatePath();
}

void SShape::rotate(double angle)
{
    mdRotateAngle = fmod(mdRotateAngle + angle, 360);
    for(auto& pt : mVerticesVec)
    {
        pt.rx() = pt.rx() * cos(angle) - pt.ry() * sin(angle);
        pt.ry() = -pt.ry() * cos(angle) - pt.rx() * sin(angle);
    }
    for(auto& pt : mControlPtVec)
    {
        pt.rx() = pt.rx() * cos(angle) - pt.ry() * sin(angle);
        pt.ry() = -pt.ry() * cos(angle) - pt.rx() * sin(angle);
    }
    updatePath();
}

void SShape::scale(double sx, double sy)
{
    assert(sx > 0 && sy > 0);
    for(auto& pt : mVerticesVec)
    {
        pt.rx() *= sx;
        pt.ry() *= sy;
    }
    for(auto& pt : mControlPtVec)
    {
        pt.rx() *= sx;
        pt.ry() *= sy;
    }
    updatePath();
}

void SShape::writeBinaryData(QDataStream &stream)
{

}

void SShape::readBinaryData(QDataStream &stream)
{

}

const std::vector<QPointF> &SShape::vertices() const
{
    return mVerticesVec;
}

const std::vector<QPointF> &SShape::ctrlPoints() const
{
    return mControlPtVec;
}

const QPainterPath &SShape::path() const
{
    return mPath;
}

const QPen& SShape::pen() const
{
    return mPen;
}

const QBrush& SShape::brush() const
{
    return mBrush;
}

bool SShape::isClose() const
{
    return mbClose;
}

QPointF SShape::vertexAt(size_t idx) const
{
    return this->CtoA(mVerticesVec[idx]);
}

void SShape::addVertex(const QPointF &_pt)
{
    if(mVerticesVec.size() < 2)
    {
        mVerticesVec.push_back(this->AtoC(_pt));
        return;
    }
    //若是第一次到达2，构成封闭SShape后再插入
    else if(mVerticesVec.size() == 2)
    {
        mControlPtVec.reserve(4);
        const QPointF& fwd_pt = mVerticesVec[1];
        const QPointF& bwd_pt = mVerticesVec.front();
        QPointF sftFactor = (fwd_pt - bwd_pt) / 3.0;
        mControlPtVec.push_back(bwd_pt + sftFactor);
        mControlPtVec.push_back(bwd_pt + sftFactor * 2.0);
        mControlPtVec.push_back(mControlPtVec.back());
        mControlPtVec.push_back(mControlPtVec.front());
    }

    insertVertex(_pt, mVerticesVec.size());
}

void SShape::addVertex(const QPointF *pt, size_t ptCount)
{
    for(size_t i = 0; i < ptCount; ++i)
        addVertex(pt[i]);
}

void SShape::addVertex(std::initializer_list<QPointF> pt)
{
    for(const auto& _pt : pt)
        addVertex(_pt);
}

void SShape::setTextureImage(QImage *image)
{
    if(image->isNull())
        return;
    this->mTextureImage = image;
    this->filler = Image;
    this->mBrush.setTextureImage(*mTextureImage);
}


void SShape::_addVertex(const QPointF &pt, const QPointF& cPt1, const QPointF& cPt2)
{
    mVerticesVec.push_back(this->AtoC(pt));
    mControlPtVec.push_back(this->AtoC(cPt1));
    mControlPtVec.push_back(this->AtoC(cPt2));
}

void SShape::_addVertex(const QPointF *pt, const QPointF* cpt, size_t ptCount)
{
    //预分配内存
    mVerticesVec.reserve(mVerticesVec.size() + ptCount);
    mControlPtVec.reserve(mControlPtVec.size() + ptCount * 2);

    for(size_t i = 0; i < ptCount; ++i)
        mVerticesVec.push_back(pt[i]);
    for(size_t i = 0; i < ptCount * 2; ++i)
        mControlPtVec.push_back(cpt[i]);
}

void SShape::_addVertex(std::initializer_list<QPointF> pt, std::initializer_list<QPointF> cpt)
{
    //预分配内存
    mVerticesVec.reserve(mVerticesVec.size() + pt.size());
    mControlPtVec.reserve(mControlPtVec.size() + cpt.size());

    for(const auto& _pt : pt)
        mVerticesVec.push_back(_pt);
    for(const auto& _cpt : cpt)
        mControlPtVec.push_back(_cpt);
}

void SShape::setVertex(const QPointF & _pt, size_t idx)
{
    QPointF pt = this->AtoC(_pt);
    //确定近控制点和远控制点的偏移量
    QPointF sftFactor = (pt - mVerticesVec[idx]) / 3.0;
    //确定前控制点对和后控制点对
    mControlPtVec[idx * 2 + 0] += sftFactor * 2.0;
    mControlPtVec[idx * 2 + 1] += sftFactor;
    mControlPtVec[idx == 0 ? mControlPtVec.size() - 1 : idx * 2 - 1] += sftFactor * 2.0;
    mControlPtVec[idx == 0 ? mControlPtVec.size() - 2 : idx * 2 - 2] += sftFactor;
    updatePath();
}

void SShape::insertVertex(const QPointF & _pt, size_t idx)
{
    assert(idx <= mVerticesVec.size());
    assert(mVerticesVec.size() >= 2);

    QPointF pt = this->AtoC(_pt);
    //确定前点和后点
    QPointF& fwdPt = mVerticesVec[idx % mVerticesVec.size()];
    QPointF& bwdPt = mVerticesVec[idx == 0 ? mVerticesVec.size() - 1 : idx - 1];
    //确定三分点
    QPointF fwdSftFactor = (fwdPt - pt) / 3.0,
            bwdSftFactor = (pt - bwdPt) / 3.0;
    //修改前点和后点连线的控制点 为 插入点和后点的直线三分控制点
    size_t cidx = idx == 0 ? mControlPtVec.size() : idx * 2;
    mControlPtVec[cidx - 2] = bwdPt + bwdSftFactor;
    mControlPtVec[cidx - 1] = bwdPt + bwdSftFactor * 2.0;
    //插入插入点和前点连线的两个直线三分控制点
    mControlPtVec.insert(mControlPtVec.begin() + idx * 2, {pt + fwdSftFactor, pt + fwdSftFactor * 2.0});
    //插入顶点
    mVerticesVec.insert(mVerticesVec.begin() + idx, pt);
    updatePath();
}

QPointF SShape::removeVertex(size_t idx)
{
    assert(idx < mVerticesVec.size());
    assert(mVerticesVec.size() > 2);
    QPointF tmpPt = mVerticesVec[idx];
    //确定前点和后点
    QPointF& fwdPt = mVerticesVec[(idx + 1) % mVerticesVec.size()];
    QPointF& bwdPt = mVerticesVec[idx == 0 ? mVerticesVec.size() - 1 : idx - 1];
    //计算偏移量三等分因子
    QPointF sftFactor = (fwdPt - bwdPt) / 3.0;
    //修改前点和该点连线的控制点 为 后点与前点的直线三分控制点
    mControlPtVec[2 * idx] = bwdPt + sftFactor;
    mControlPtVec[2 * idx + 1] = bwdPt + sftFactor * 2.0;
    //移除后点与该点连线的控制点
    size_t cidx = idx == 0 ? mControlPtVec.size() : idx * 2;
    mControlPtVec.erase(mControlPtVec.begin() + cidx - 2, mControlPtVec.begin() + cidx - 1);
    return this->CtoA(tmpPt);
    updatePath();
}

void SShape::setCtrlPt(const QPointF &cpt, size_t idx, bool secondPt)
{
    assert(!(mbClose == false && idx == mVerticesVec.size() - 1));
    mControlPtVec[idx * 2 + secondPt] = this->AtoC(cpt);
    updatePath();
}

void SShape::setPen(const QPen & pen)
{
    mPen = pen;
}

void SShape::setBrush(const QBrush & brush)
{
    mBrush = brush;
}

void SShape::setFillRule(Qt::FillRule rule)
{
    mPath.setFillRule(rule);
}

QPen &SShape::rpen()
{
    return mPen;
}

QBrush &SShape::rbrush()
{
    return mBrush;
}

void SShape::setClose(bool close)
{
    assert(mVerticesVec.size() > 2);
    mbClose = close;
    updatePath();
}

bool SShape::doUpdate() const
{
    return mbDoUpdatePath;
}


void SShape::setUpdate(bool update)
{
    mbDoUpdatePath = update;
}

void SShape::updatePath()
{
    //创建新绘制路径：位于(0,0)
    mPath.moveTo(mVerticesVec.front());
    //用贝塞尔曲线链接
    for(size_t idx = 0; idx < mVerticesVec.size() - 1; ++idx)
        mPath.cubicTo(mControlPtVec[idx * 2], mControlPtVec[idx * 2 + 1], mVerticesVec[idx + 1]);
    //封闭图形
    if(mbClose)
    {
        //mPath.cubicTo(mControlPtVec[mControlPtVec.size() - 2], mControlPtVec[mControlPtVec.size() - 1], mVerticesVec.front());
        //mPath.closeSubpath();
    }
}
