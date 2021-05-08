#include "sfragimage.h"
#include "QFile"
#include "QTextStream"

SFragImage::SFragImage(PaintObject _type, SFragLoader &_loader, bool _selected, QPointF center, const QString &_layerName, const QString &_layerDiscription, const QColor &_layerColor)
    : SObject(_type, _selected, center, _layerName, _layerDiscription, _layerColor), mFragLoader(_loader)
{

}

void SFragImage::paint(QPainter &painter, bool doTranslate, QRectF viewLogicalArea, double scaleValue)const
{
    //确定金字塔层级
    double level = log(scaleValue) / log(0.5);
    if(level < 0)level = 0;
    if(level > this->mFragMatVec.size()) level = this->mFragMatVec.size();

    size_t idx = round(level);

    //平移到中心点
    if(doTranslate)
        painter.translate(mPtCenter);

    //执行变换
    painter.setTransform(mTransform * painter.transform());
    //绘图
    const SFragMatrix& fragMat = mFragMatVec[idx];
    SImage** ppFragImage = fragMat.getData();
    size_t imgCount = fragMat.Rows() * fragMat.Cols();
    for(size_t i = 0; i < imgCount; ++i)
        if(ppFragImage[i])
            ppFragImage[i]->paint(painter);
    //执行逆变换
    painter.setTransform(mTransform.inverted() * painter.transform());

    //返回原点
    if(doTranslate)
        painter.translate(-mPtCenter);
}

void SFragImage::setFragmentPath(const QString &folder, const QString &imageFileName)
{
    this->mStrFolderPath = folder;
    this->mStrFileName = imageFileName;
}

void SFragImage::_loadMeta()
{
    QFile file(mStrFolderPath + '/' + mStrFileName + "_Meta.txt");
    if(!file.open(QFile::ReadOnly))
        qDebug() << "Cannot Open Meta File\n";
    QString metaString(file.readAll());

    QTextStream stream(&metaString);
    QString buf;

    //读入原始影像数据和金字塔层数
    int nLevel;
    int originalWidth, originalHeight;
    stream >> buf >> nLevel;
    stream >> buf >> originalWidth;
    stream >> buf >> originalHeight;

    for(int i = 0; i < nLevel; ++i)
    {
        //读入金字塔层描述数据
        size_t level, levelWidth, levelHeight, fragWidth, fragHeight, fragRows, fragCols;
        stream >> buf >> level;
        stream >> buf >> levelWidth;
        stream >> buf >> levelHeight;
        stream >> buf >> fragWidth;
        stream >> buf >> fragHeight;
        stream >> buf >> fragRows;
        stream >> buf >> fragCols;
        //生成该层分片影像列表
        QStringList fragPathList;
        QString levelFolderPath = mStrFolderPath + '/' + QString::number(level) + '/' + mStrFileName;
        for(size_t i = 0; i < fragRows * fragCols; ++i)
            fragPathList << levelFolderPath + '_' + QString::number(i + 1);

        SFragMatrix mat(fragRows, fragCols, fragPathList);
        mat.setLevelMeta(originalWidth, originalHeight, level, levelWidth, levelHeight, fragWidth, fragHeight);
        mFragMatVec.push_back(mat);
    }

    QPointF topLeft(-originalWidth / 2, -originalHeight / 2);
    QPointF topRight(originalWidth / 2, -originalHeight / 2);
    QPointF bottomLeft(-originalWidth / 2, originalHeight / 2);
    QPointF bottomRight(originalWidth / 2, originalHeight / 2);
    //确定包围控制点
    mpBoundPt[0] = topLeft;
    mpBoundPt[1] = topRight;
    mpBoundPt[2] = bottomRight;
    mpBoundPt[3] = bottomLeft;
    //确定外围矩形
    this->mImageRect.setTopLeft(topLeft);
    this->mImageRect.setBottomRight(bottomRight);
}


QPolygonF SFragImage::boundingRect() const
{
    return QPolygonF(QVector<QPointF>(
    {mpBoundPt[0], mpBoundPt[1], mpBoundPt[2], mpBoundPt[3]} ))
    .translated(mPtCenter);
}

bool SFragImage::contains(const QPointF &pt) const
{
    QPainterPath path;
    path.moveTo(mpBoundPt[0]);
    path.lineTo(mpBoundPt[1]);
    path.lineTo(mpBoundPt[2]);
    path.lineTo(mpBoundPt[3]);
    path.closeSubpath();

    return path.contains(this->AtoC(pt));
}

void SFragImage::writeBinaryData(QDataStream &stream) const
{

}

void SFragImage::readBinaryData(QDataStream &stream)
{

}

void SFragImage::_applyTransform()
{
    mpBoundPt[0] = mTransform.map(mImageRect.topLeft());
    mpBoundPt[1] = mTransform.map(mImageRect.topRight());
    mpBoundPt[2] = mTransform.map(mImageRect.bottomRight());
    mpBoundPt[3] = mTransform.map(mImageRect.bottomLeft());
}
