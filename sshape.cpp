#include "sshape.h"
#include <QIcon>
#include <QPixmap>
SShape::SShape(PaintObject _type, bool _selected, QPointF center,
               const QString &_layerName,
               const QString &_layerDiscription,
               const QColor &_layerColor)
    : SObject(_type, _selected, center, _layerName, _layerDiscription, _layerColor)
{
    this->mBrush.setColor(QColor(128, 128, 128, 128));
}

SShape::~SShape()
{
}

void SShape::paint(QPainter &painter, bool doTranslate) const
{
    //保存原来的样式
    const QPen& oldPen = painter.pen();
    const QBrush& oldBrush = painter.brush();
    //设置为本形状样式
    painter.setPen(mPen);
    painter.setBrush(mBrush);
    //平移到中心点
    if(doTranslate)
        painter.translate(mPtCenter);
    //绘图
    painter.drawPath(mPathTransformed);

    //返回原点
    if(doTranslate)
        painter.translate(-mPtCenter);
    //还原样式
    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

QPolygonF SShape::boundingRect()const
{
    QRectF boundRect = mPath.boundingRect();

    QPointF topLeft = boundRect.topLeft(),
            topRight = boundRect.topRight(),
            bottomRight = boundRect.bottomRight(),
            bottomLeft = boundRect.bottomLeft();

    QVector<QPointF> ptVec;
    ptVec.push_back(mTransform.map(topLeft));
    ptVec.push_back(mTransform.map(topRight));
    ptVec.push_back(mTransform.map(bottomRight));
    ptVec.push_back(mTransform.map(bottomLeft));

    return QPolygonF(ptVec).translated(mPtCenter);
}

bool SShape::contains(const QPointF &pt)const
{
    return mPathTransformed.contains(this->AtoC(pt));
}

void SShape::writeBinaryData(QDataStream &stream)const
{

}

void SShape::readBinaryData(QDataStream &stream)
{

}

void SShape::_applyTransform()
{
    updatePath();
    mPathTransformed = mTransform.map(mPath);
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

void SShape::setTextureImage(const QPixmap& pixmap)
{
    if(pixmap.isNull())
        return;
    this->mTextureImage = pixmap;
    this->filler = Image;
    this->mBrush.setTexture(pixmap);
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
    _applyTransform();
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
    _applyTransform();
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
    _applyTransform();
}

void SShape::setCtrlPt(const QPointF &cpt, size_t idx, bool secondPt)
{
    assert(!(mbClose == false && idx == mVerticesVec.size() - 1));
    mControlPtVec[idx * 2 + secondPt] = this->AtoC(cpt);
    _applyTransform();
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
    _applyTransform();
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
    mPath.clear();
    //创建新绘制路径：位于(0,0)
    mPath.moveTo(mVerticesVec.front());
    //用贝塞尔曲线链接
    for(size_t idx = 0; idx < mVerticesVec.size() - 1; ++idx)
        mPath.cubicTo(mControlPtVec[idx * 2], mControlPtVec[idx * 2 + 1], mVerticesVec[idx + 1]);
    //封闭图形
    if(mbClose)
        mPath.cubicTo(mControlPtVec[mControlPtVec.size() - 2], mControlPtVec[mControlPtVec.size() - 1], mVerticesVec.front());

    mPath.closeSubpath();
}
