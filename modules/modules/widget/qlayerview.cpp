#include "qlayerview.h"
QLayerView::QLayerView(QWidget *parent, SDocument *pCurrentDocument): QTreeView(parent)
{
    setDocument(pCurrentDocument);
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
    if(mpCurDoc)
        disconnect(this, &QLayerView::clicked, &mpCurDoc->getLayerManager(), &SLayerManager::onLayerViewClicked);
    this->mpCurDoc = pCurrentDocument;
    this->updateLayerModel();
}

void QLayerView::updateLayerModel()
{
    if(!mpCurDoc)
        return;
    QStandardItemModel* pModel = const_cast<QStandardItemModel*>(mpCurDoc->getLayerManager().getLayerModel());
    this->setModel(pModel);
    connect(this, &QLayerView::clicked, &mpCurDoc->getLayerManager(), &SLayerManager::onLayerViewClicked);
}
