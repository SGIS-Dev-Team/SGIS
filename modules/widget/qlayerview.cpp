#include "qlayerview.h"
#include <QAction>
#include <QMenu>
#include <QMouseEvent>

QLayerView::QLayerView(QWidget* parent, std::shared_ptr<SDocument> pCurrentDocument): QTreeView(parent)
{
    setDocument(pCurrentDocument);
    _initialize();
}

QLayerView::~QLayerView()
{

}

void QLayerView::mouseReleaseEvent(QMouseEvent* event)
{
    QTreeView::mouseReleaseEvent(event);

    if (event->button() == Qt::RightButton)
    {
        Q_ASSERT(model());
        Q_ASSERT(mpCurDoc);
        //判断是否在图层上右键
        QModelIndex index = indexAt(event->pos());
        bool isValid = index.isValid();
        //设置菜单响应
        mpActionCopy            ->setEnabled(isValid);
        mpActionCut             ->setEnabled(isValid);
        mpActionSwitchVisible   ->setEnabled(isValid);
        mpActionRemove          ->setEnabled(isValid);
        mpActionScaleToFit      ->setEnabled(isValid);
        mpActionSetBands        ->setEnabled(isValid);
        mpActionAttribute       ->setEnabled(isValid);

        /*
         * [设置选择状态]
         * 若当前没有选中任何图层：
         *      若右键的位置在空白处，则不做任何选择操作
         *      若右键的位置在图层上，则选中该图层
         * 若当前选中了图层：
         *      若右键的位置在空白处，或者不在已经选中的任何图层上，则清除所有选择
         *      若右键的位置在已选中的图层上，则不做任何选择操作
         */
        SLayerManager& layerManager = mpCurDoc->getLayerManager();
        auto sltLayerIterList = layerManager.getSelectedLayerIterList();
        QItemSelectionModel* pSltModel = this->selectionModel();

        if (sltLayerIterList.size() == 0)
        {
            if (isValid)
            {
                pSltModel->select(index, QItemSelectionModel::SelectionFlag::Rows);

            }
        }
        else
        {
            if (!isValid || !pSltModel->isSelected(index))
            {
                pSltModel->clearSelection();
            }
        }


        mpContextMenu->exec();
    }
}

void QLayerView::onLayerManagerSelectChanged()
{

}

std::shared_ptr<SDocument> QLayerView::getDocument()
{
    return this->mpCurDoc;
}

void QLayerView::setDocument(std::shared_ptr<SDocument> pCurrentDocument)
{
    if (mpCurDoc)
        disconnect(this, &QLayerView::clicked, &mpCurDoc->getLayerManager(), &SLayerManager::onLayerViewClicked);
    this->mpCurDoc = pCurrentDocument;
    this->_updateLayerModel();
}

void QLayerView::_updateLayerModel()
{
    if (!mpCurDoc)
        return;
    QStandardItemModel* pModel = const_cast<QStandardItemModel*>(mpCurDoc->getLayerManager().getLayerModel());
    this->setModel(pModel);
    connect(this, &QLayerView::clicked, &mpCurDoc->getLayerManager(), &SLayerManager::onLayerViewClicked);
}

void QLayerView::_initialize()
{
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setSelectionModel(new QItemSelectionModel(model(), this));
    this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

    _initializeContextMenu();
    _initializeConnections();
}

void QLayerView::_initializeContextMenu()
{
    //上下文右键菜单选项初始化
    mpActionAddRaster       = new QAction(tr("Raster"));
    mpActionAddArchive      = new QAction(tr("Archive"));
    mpActionAddShapeFile    = new QAction(tr("Shape File"));

    mpSubMenuAdd            = new QMenu(tr("Add"));

    mpSubMenuAdd->addActions({mpActionAddRaster, mpActionAddArchive, mpActionAddShapeFile});

    mpActionCopy            = new QAction(tr("Copy"));
    mpActionCut             = new QAction(tr("Cut"));
    mpActionPaste           = new QAction(tr("Paste"));

    mpActionSwitchVisible   = new QAction(tr("Hide"));
    mpActionRemove          = new QAction(tr("Remove"));

    mpActionScaleToFit      = new QAction(tr("Scale to Fit"));
    mpActionSetBands        = new QAction(tr("Set Bands"));
    mpActionAttribute       = new QAction(tr("Attribute..."));

    //设置菜单
    mpContextMenu->addMenu(mpSubMenuAdd);
    mpContextMenu->addAction(mpActionCopy);
    mpContextMenu->addAction(mpActionCut);
    mpContextMenu->addAction(mpActionPaste);

    mpContextMenu->addSeparator();

    mpContextMenu->addAction(mpActionSwitchVisible);
    mpContextMenu->addAction(mpActionRemove);

    mpContextMenu->addSeparator();

    mpContextMenu->addAction(mpActionScaleToFit);
    mpContextMenu->addAction(mpActionSetBands);
    mpContextMenu->addAction(mpActionAttribute);
}

void QLayerView::_initializeConnections()
{
    connect(&mpCurDoc->getLayerManager(), &SLayerManager::selectStateChanged, this, &QLayerView::onLayerManagerSelectChanged);
}

