#include "qlayerview.h"
QLayerView::QLayerView(QWidget *parent, SDocument *pCurrentDocument)
{

}

QLayerView::~QLayerView()
{

}

SDocument *QLayerView::getDocument()
{
    return this->mpCurDoc;
}

void QLayerView::setDocument(SDocument *pCurrentDocument)
{
    this->mpCurDoc = pCurrentDocument;
}

void QLayerView::updateLayerModel()
{
    if(!mpCurDoc)
        return;

}
