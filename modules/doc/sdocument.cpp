#include "sdocument.h"


SDocument::SDocument(QCanvas *canvas)
{
    if(!canvas)
        return;
    setCanvas(canvas);
    canvas->setDocument(this);
    _initializeConnections();
}

SDocument::SDocument(QCanvas *canvas, const QString &path): SDocument(canvas)
{

}

SDocument::~SDocument()
{

}

void SDocument::onImageLoaded(SImage *pImage)
{
    const QRectF& viewAreaRect = mpCanvas->viewArea();
    double scaleValue = mpCanvas->scaleValue();
    mpCanvas->repaint(viewAreaRect.left() * scaleValue,
                      viewAreaRect.top() * scaleValue,
                      viewAreaRect.width() * scaleValue,
                      viewAreaRect.height() * scaleValue);
}

void SDocument::setCanvas(QCanvas *canvas)
{
    this->mpCanvas = canvas;
}

SLayerManager &SDocument::getLayerManager()
{
    return mLayerMgr;
}

SFragLoader &SDocument::getFragLoader()
{
    return mFragLoader;
}

void SDocument::paint(QPainter &painter, const QRectF &viewArea, double scaleValue)
{
    const layer_list layerList = mLayerMgr.getLayerList();

    //自底向上绘制图层
    std::list<SObject*>::const_iterator iter = layerList.begin();
    for(iter = layerList.begin(); iter != layerList.end(); ++iter)
    {
        SObject* obj = *iter;
        if(obj->isVisible())
            obj->paint(painter, true, viewArea, scaleValue);
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
}







