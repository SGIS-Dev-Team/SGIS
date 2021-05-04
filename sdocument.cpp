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
    const layer_list layerList = mLayerMgr.getLayerList();

    //自底向上绘制图层
    std::list<SObject*>::const_iterator iter = layerList.begin();
    for(iter = layerList.begin(); iter != layerList.end(); ++iter)
    {
        SObject* obj = *iter;
        if(obj->isVisible())
            obj->paint(painter);
    }

    //绘制选框
    //TODO::组合功能上线后重新调整逻辑
    //现在只绘制一个选框
    std::list<list_iterator> selectedLayerIterList = mLayerMgr.getSelectedLayerIterList();
    if(selectedLayerIterList.empty())
        return;
    const list_iterator firstSelectedIter = selectedLayerIterList.front();
    (*firstSelectedIter)->paintBoundRect(painter);
}







