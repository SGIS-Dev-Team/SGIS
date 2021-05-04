#include "sdocument.h"


SDocument::SDocument(QCanvas *canvas)
{
    if(!canvas)
        return;
    setCanvas(canvas);
    canvas->setDocument(this);
}

SDocument::SDocument(QCanvas *canvas, const QString &path): SDocument(canvas)
{

}

SDocument::~SDocument()
{

}

void SDocument::setCanvas(QCanvas *canvas)
{
    this->mpCanvas = canvas;
}

SLayerManager &SDocument::getLayerManager()
{
    return mLayerMgr;
}

void SDocument::paint(QPainter &painter)
{
    const std::list<SObject*>& layerList = mLayerMgr.getLayerList();
    std::list<SObject*>::const_iterator iter = layerList.begin();
    for(iter = layerList.begin(); iter != layerList.end(); ++iter)
    {
        SObject* obj = *iter;
        if(obj->isVisible())
            obj->paint(painter);
    }
}
