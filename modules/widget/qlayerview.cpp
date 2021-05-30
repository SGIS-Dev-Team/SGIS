#include "qlayerview.h"
QLayerView::QLayerView(QWidget *parent, std::shared_ptr<SDocument> pCurrentDocument): QTreeView(parent)
{
    setDocument(pCurrentDocument);
}

QLayerView::~QLayerView()
{

}

std::shared_ptr<SDocument> QLayerView::getDocument()
{
    return this->mpCurDoc;
}

void QLayerView::setDocument(std::shared_ptr<SDocument> pCurrentDocument)
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
