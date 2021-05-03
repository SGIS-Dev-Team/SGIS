#include "qlayerview.h"
QLayerView::QLayerView(QWidget *parent, SDocument *pCurrentDocument)
{
    mpCurDoc = pCurrentDocument;
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
    this->updateLayerModel();
}

void QLayerView::updateLayerModel()
{
    if(!mpCurDoc)
        return;
    this->setModel(const_cast<QStandardItemModel*>(mpCurDoc->getLayerManager().getLayerModel()));
}
