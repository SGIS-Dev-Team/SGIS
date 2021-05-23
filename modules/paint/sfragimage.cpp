#include "sfragimage.h"
#include "QFile"
#include "QTextStream"

SFragImage::SFragImage(SFragLoader &_loader, bool _selected, QPointF center, const QString &_layerName, const QString &_layerDiscription, const QColor &_layerColor)
    : SObject(PaintObject::FragImageBase, _selected, center, _layerName, _layerDiscription, _layerColor), mFragLoader(_loader)
{

}

SFragImage::~SFragImage()
{

}

void SFragImage::paint(QPainter &painter, bool doTranslate, QRectF viewLogicalArea, double scaleValue)const
{
    //检测视图显示区域是否与图像区域重合
    if(!intersect(viewLogicalArea))
        return;

    //确定金字塔层级
    double level = log(scaleValue) / log(0.5);
    if(level < 0) level = 0;
    if(level > this->mFragMatVec.size() - 1) level = this->mFragMatVec.size() - 1;
    size_t idx = round(level);

    const SFragMatrix& fragMat = mFragMatVec[idx];
    //确定需要加载的分块影像
    std::vector<SImage*> loadBlockVec = fragMat.block(viewLogicalArea.translated(-mPtCenter));

    //调用加载器线程加载图像
    if(!loadBlockVec.empty())
        mFragLoader.push_front(loadBlockVec.data(), loadBlockVec.size());
    mFragLoader.start();

    //保存原来的变换
    QTransform oldTransform = painter.transform();

    //平移到中心点
    if(doTranslate)
        painter.translate(mPtCenter);

    //执行变换
    painter.setTransform(mTransform * painter.transform());

    //-----绘图-----//
    SImage* pFragImage = fragMat.getData();
    size_t imgCount = fragMat.Rows() * fragMat.Cols();
    for(size_t i = 0; i < imgCount; ++i)
        pFragImage[i].paint(painter);
    //-----绘图-----//

    //还原变换
    painter.setTransform(oldTransform);
}

void SFragImage::setFragmentPath(const QString &folder, const QString &imageFileName)
{
    this->mStrFolderPath = folder;
    this->mStrFileName = imageFileName;
}

void SFragImage::setHistEqFunc(std::shared_ptr<void> pEqFunc[])
{
    for(auto& mat : mFragMatVec)
        mat.setHistEqFunc(pEqFunc);
}

void SFragImage::setBandIndices(int r, int g, int b)
{
    for(auto& mat : mFragMatVec)
        mat.setBandIndices(r, g, b);
}

void SFragImage::setHoldTopPyramidEnabled(bool hold)
{
    mbHoldTopPyramid = hold;
    if(mFragMatVec.empty())
        return;
    if(hold)
        this->mFragMatVec.back().loadAll();
    else
        this->mFragMatVec.back().getData()->releaseImage();
}

void SFragImage::loadMeta()
{
    QFile file(mStrFolderPath + '/' + mStrFileName + "_Meta.txt");
    if(!file.open(QFile::ReadOnly))
        qDebug() << "Cannot Open Meta File\n";
    QString metaString(file.readAll());
    file.close();

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
            fragPathList << levelFolderPath + '_' + QString::number(i + 1) + ".tif";

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

    _applyTransform();
}


QPolygonF SFragImage::boundingRect() const
{
    return QPolygonF(QVector<QPointF>(
    {mpBoundPt[0], mpBoundPt[1], mpBoundPt[2], mpBoundPt[3]} ))
    .translated(mPtCenter);
}

bool SFragImage::contains(const QPointF &pt, bool isInBoundRect) const
{
    return this->boundingRect().containsPoint(pt, Qt::WindingFill);
}

void SFragImage::writeBinaryData(QDataStream &stream) const
{

}

void SFragImage::readBinaryData(QDataStream &stream)
{

}

QIcon SFragImage::icon() const
{
    //读取顶层金字塔的图像绘制缩略图
    QPixmap iconPixmap(LAYER_ICON_SIZE);
    iconPixmap.fill(Qt::transparent);
    QPainter iconPainter(&iconPixmap);

    //确定形状的外接矩形尺寸
    QRectF bound_rect = this->boundingRect().boundingRect();
    //平移外界矩形中心点到画布中心
    iconPainter.translate(iconPixmap.rect().center());
    //使形状填充画布
    iconPainter.scale(iconPixmap.width() / bound_rect.width(), iconPixmap.height() / bound_rect.height());

    //执行变换
    iconPainter.setTransform(mTransform * iconPainter.transform());

    SFragMatrix fragMat = mFragMatVec.back();
    //-----绘图-----//
    SImage* pFragImage = fragMat.getData();

    if(pFragImage->isNull())
        pFragImage->load();

    pFragImage->paint(iconPainter);

    if(!mbHoldTopPyramid)
        pFragImage->releaseImage();

    //-----绘图-----//

    //执行逆变换
    iconPainter.setTransform(mTransform.inverted() * iconPainter.transform());

    return QIcon(iconPixmap);
}

void SFragImage::_applyTransform()
{
    mpBoundPt[0] = mTransform.map(mImageRect.topLeft());
    mpBoundPt[1] = mTransform.map(mImageRect.topRight());
    mpBoundPt[2] = mTransform.map(mImageRect.bottomRight());
    mpBoundPt[3] = mTransform.map(mImageRect.bottomLeft());

    mImageSize.setWidth(mImageRect.width() * mdSx);
    mImageSize.setHeight(mImageRect.height() * mdSy);
}

QRectF SFragImage::_originalRect()
{
    return mImageRect;
}
