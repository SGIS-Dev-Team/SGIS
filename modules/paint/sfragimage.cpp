#include "sfragimage.h"
#include "QFile"
#include "QTextStream"
#include <qdir.h>
#include <ctime>

SFragImage::SFragImage(SFragLoader &_loader, bool _selected, QPointF center, const QString &_layerName, const QString &_layerDiscription, const QColor &_layerColor)
    : SObject(PaintObject::FragImageBase, _selected, center, _layerName, _layerDiscription, _layerColor), mFragLoader(_loader)
{

}

SFragImage::~SFragImage()
{

}

void SFragImage::paint(QPainter &painter, bool doTranslate, const QRectF &viewLogicalArea, double scaleValue, PaintTrigger trigger)const
{
    //检测视图显示区域是否与图像区域重合
    if(!intersect(viewLogicalArea))
        return;

    //确定金字塔层级
    double level = log(scaleValue) / log(0.5);
    if(level < 0) level = 0;
    if(level > this->mFragMatVec.size() - 1) level = this->mFragMatVec.size() - 1;
    size_t idx = round(level);

    //加载当前层区域的分片
    if(trigger == User_Trigger)
        mFragMatVec[idx].loadBlockArea(viewLogicalArea.translated(-mPtCenter), mFragLoader);

    //保存原来的变换
    QTransform oldTransform = painter.transform();

    //平移到中心点
    if(doTranslate)
        painter.translate(mPtCenter);

    //执行变换
    painter.setTransform(mTransform * painter.transform());

    //-----绘图-----//
    //从下往上绘制各层金字塔影像
    CLOCK_START(1)
//    for(int i = mFragMatVec.size() - 1; i >= 0; --i)
//        mFragMatVec[i].paint(painter, viewLogicalArea.translated(-mPtCenter));
    //绘制已加载的所有分片
    if(mbHoldTopPyramid)
        mFragMatVec.back().paint(painter);

    CLOCK_STOP(1)
    //-----绘图-----//

    //还原变换
    painter.setTransform(oldTransform);
}

void SFragImage::setFragmentPath(const QString &dirPath)
{
    this->mStrDirPath = dirPath;
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
    if(mFragMatVec.empty())
        return;

    //计算并设置顶层金字塔均衡化函数
    SImage *pImage = mFragMatVec.back().getData();
    pImage->load();

    std::shared_ptr<void> pEqFunc[3];
    pImage->getHistEqFunc(pEqFunc);

    for(auto &mat : mFragMatVec)
        mat.setHistEqFunc(pEqFunc);

    if(!mbHoldTopPyramid)
        pImage->releaseImage();
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
    //打开元数据文本文件
    QFile file(mStrDirPath + '/' + QFileInfo(mStrDirPath).fileName() + "_Meta.txt");
    if(!file.open(QFile::ReadOnly))
    {
        Q_ASSERT(0);
        SLogger::getLogger()->addEntry(Me, SLogger::LocalError, "Loading meta : meta text not exists.");
    }
    QString metaString(file.readAll());
    file.close();

    //读入原始影像数据和金字塔层数
    QTextStream stream(&metaString);

    QString buf;
    stream.readLine();  //读掉"#"文件注释
    stream.readLine();

    int nLevel;
    int nBaseWidth, nBaseHeight;
    stream >> buf >> nLevel >> buf >> nBaseWidth >> buf >> nBaseHeight;

    mFragMatVec.reserve(nLevel);
    stream.readLine();  //将最后一行的换行符读取掉
    for(int i = 0; i < nLevel; ++i)
    {
        //生成该层分片影像列表
        QString levelPath = stream.readLine();
        int levelWidth = nBaseWidth / pow(2, i);
        int levelHeight = nBaseHeight / pow(2, i);

        mFragMatVec.emplace_back(levelPath);
        mFragMatVec.back().setLevelMeta(nBaseWidth, nBaseHeight, levelWidth, levelHeight);
    }

    if(mbHoldTopPyramid)
        mFragMatVec.back().loadAll();

    //计算原始图像包围矩形
    this->mImageRect.setRect(-nBaseWidth / 2.0, -nBaseHeight / 2.0, nBaseWidth, nBaseHeight);

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
    Q_UNUSED(isInBoundRect);
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
}

QRectF SFragImage::_originalRect()
{
    return mImageRect;
}
