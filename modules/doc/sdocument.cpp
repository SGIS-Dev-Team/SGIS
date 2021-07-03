#include "sdocument.h"


SDocument::SDocument(QCanvas * pCanvas)
{
    if(!pCanvas)
        return;
    setCanvas(pCanvas);
    pCanvas->setDocument(this);
    //初始化读取器线程
    mFragLoader.moveToThread(&mLoaderThread);
    mLoaderThread.start();

    _initializeConnections();
}

SDocument::SDocument(QCanvas * pCanvas, const QString &path): SDocument(pCanvas)
{

}

SDocument::~SDocument()
{
    _disconnectCanvas();
}

void SDocument::onImageLoaded(SImage *pImage)
{
    Q_UNUSED(pImage);
    mpCanvas->setPaintTrigger(SObject::Loaded_Trigger);
    emit updateCanvas();
}

void SDocument::setCanvas(QCanvas * pCanvas)
{
    _disconnectCanvas();
    this->mpCanvas = pCanvas;
}

SLayerManager &SDocument::getLayerManager()
{
    return mLayerMgr;
}

SFragLoader &SDocument::getFragLoader()
{
    return mFragLoader;
}

void SDocument::paint(QPainter &painter, const QRectF &viewArea, double scaleValue, SObject::PaintTrigger trigger)
{
    const layer_list layerList = mLayerMgr.getLayerList();

    //自底向上绘制图层
    std::list<SObject*>::const_iterator iter = layerList.begin();
    for(iter = layerList.begin(); iter != layerList.end(); ++iter)
    {
        SObject* obj = *iter;
        if(obj->isVisible())
            obj->paint(painter, true, viewArea, scaleValue, trigger);
    }

    //绘制选框
    //TODO::组合功能上线后重新调整逻辑
    //现在只单独绘制选框
    std::list<list_iterator> selectedLayerIterList = mLayerMgr.getSelectedLayerIterList();
    painter.setRenderHint(QPainter::Antialiasing);
    for(auto& iter : selectedLayerIterList)
        (*iter)->paintBoundRect(painter, scaleValue);

}

void SDocument::_initializeConnections()
{
    connect(&mFragLoader, &SFragLoader::imageLoaded, this, &SDocument::onImageLoaded);
    connect(this, &SDocument::updateCanvas, mpCanvas, &QCanvas::doUpdate);
}

void SDocument::_disconnectCanvas()
{
    if(!mpCanvas)
        disconnect(this, &SDocument::updateCanvas, mpCanvas, &QCanvas::doUpdate);
}







