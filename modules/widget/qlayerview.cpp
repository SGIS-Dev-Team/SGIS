#include "qlayerview.h"
#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include "modules/paint/sfragimage.h"

QLayerView::QLayerView(QWidget* parent): QTreeView(parent)
{
    _initialize();
}

QLayerView::QLayerView(QWidget* parent, std::shared_ptr<SDocument> pCurrentDocument): QTreeView(parent)
{
    setDocument(pCurrentDocument);
    _initialize();
}

QLayerView::~QLayerView()
{
    if (mpContextMenu)delete mpContextMenu;
}

void QLayerView::mouseReleaseEvent(QMouseEvent* event)
{
    QTreeView::mouseReleaseEvent(event);

    if (event->button() == Qt::RightButton)
    {
        Q_ASSERT(model());
        Q_ASSERT(mpCurDoc);
        //判断是否在图层上右键
        mContextIndex = indexAt(event->pos());
        bool isValid = mContextIndex.isValid();
        //设置图层相关菜单项响应
        mpActionCopy            ->setEnabled(isValid);
        mpActionCut             ->setEnabled(isValid);
        mpActionSwitchVisible   ->setEnabled(isValid);
        mpActionRemove          ->setEnabled(isValid);
        mpActionScaleToFit      ->setEnabled(isValid);
        mpActionAttribute       ->setEnabled(isValid);

        /*
         * [设置选择状态]
         * 若当前没有选中任何图层：
         *      若右键的位置在空白处，则不做任何选择操作
         *      若右键的位置在图层上，则选中该图层
         *
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
                layerManager.select(mContextIndex.row());
            }
        }
        else
        {
            if (!isValid || !pSltModel->isSelected(mContextIndex))
            {
                layerManager.clearSelection();
            }
        }

        /*
         * [根据选中的图层确定选项状态]
         * 无选中：不变
         * 选中一或多项:
         *      只有全部图层为可见时，SwitchVisible显示Hide
         *      仅选中一个栅格图层时，SetBands才响应
         */

        if (sltLayerIterList.size() >= 1)
        {
            bool isAllVisible{true};

            for (auto& iter : sltLayerIterList)
            {
                SObject* pObj = *iter;
                if (pObj->isVisible() == false)
                    isAllVisible = false;
            }

            mpActionSwitchVisible->setText(isAllVisible ? tr("Hide") : tr("Show"));

            //是否选中单个栅格图像
            bool isSingleRaster = sltLayerIterList.size() == 1 && (*sltLayerIterList.front())->getType() == PaintObject::Raster;

            mpSubMenuSetRedBand     ->setEnabled(isSingleRaster);
            mpSubMenuSetGreenBand   ->setEnabled(isSingleRaster);
            mpSubMenuSetBlueBand    ->setEnabled(isSingleRaster);

            /*
             * [处理多波段影像波段选择菜单]
             * 由于编译期不知道影像有多少波段，需要动态创建菜单选项
             * 使用Lambda语句动态定义槽函数响应触发事件
             *
             */

            if (isSingleRaster)
            {
                //为三个波段选择菜单添加选项
                QMenu* ppBandMenu[3] {mpSubMenuSetRedBand, mpSubMenuSetGreenBand, mpSubMenuSetBlueBand};
                SFragImage* pRaster = dynamic_cast<SFragImage*>(*sltLayerIterList.front());
                SImageMeta rasterMeta = pRaster->imageMeta();

                for (int i = 0; i < 3; ++i)
                {
                    //清空原有的选项
                    ppBandMenu[i]->clear();
                    //创建波段选项
                    QList<QAction*> actionList;
                    for (int band = 1; band <= rasterMeta.bandCount(); ++i)
                    {
                        QAction* pActionBand = new QAction(tr("Band ") + QString::number(band), ppBandMenu[i]);
                        actionList.push_back(pActionBand);
                        //使用lambda体处理信号
                        connect(pActionBand, &QAction::triggered, this, [ = ]() {_setBand(i, band);});
                    }
                    //当前选中的波段选项前加上标记
                    QAction* pCurrentBandAction = actionList[pRaster->bandIndex(i) - 1];
                    pCurrentBandAction->setCheckable(true);
                    pCurrentBandAction->setChecked(true);
                    //添加至菜单
                    ppBandMenu[i]->addActions(actionList);
                }
            }
        }

        /*
         * [确定粘贴可用性]
         * 要求剪贴板状态不为None
         * 要求剪贴板上有图层
         */
        mpActionPaste->setEnabled(mLayerClipStatus != LayerClipStatus::None && !mLayerClipList.isEmpty());

        //执行菜单
        mpContextMenu->exec();
    }
}

void QLayerView::onLayerManagerSelectChanged()
{
    Q_ASSERT(model());
    Q_ASSERT(mpCurDoc);

    SLayerManager& layerManager = mpCurDoc->getLayerManager();
    std::vector<int> idxVec = layerManager.getSelectedRowIndices();

    this->clearSelection();

    QItemSelection selection;

    for (auto& idx : idxVec)
    {
        selection.append(QItemSelectionRange(this->model()->index(idx, 0), this->model()->index(idx, this->model()->columnCount() - 1)));
    }

    this->selectionModel()->select(selection, QItemSelectionModel::SelectionFlag::SelectCurrent);
}

void QLayerView::onActionAddRasterTriggered()
{
    //如果后续没有其他更改可以直接把triggered信号和import信号链接起来
    emit importRaster();
}

void QLayerView::onActionAddShapeFileTriggered()
{
    emit importShapeFile();
}

void QLayerView::onActionCopyTriggered()
{
    QItemSelectionModel* pSltModel = this->selectionModel();
    QModelIndexList sltIdxList = pSltModel->selectedRows();

    if (sltIdxList.isEmpty())
    {
        mLayerClipList.clear();
        mLayerClipStatus = None;
    }
    else
    {
        mLayerClipList = sltIdxList;
        mLayerClipStatus = Copied;
    }
}

void QLayerView::onActionCutTriggered()
{
    QItemSelectionModel* pSltModel = this->selectionModel();
    QModelIndexList sltIdxList = pSltModel->selectedRows();

    if (sltIdxList.isEmpty())
    {
        mLayerClipList.clear();
        mLayerClipStatus = None;
    }
    else
    {
        mLayerClipList = sltIdxList;
        mLayerClipStatus = Cut;
    }
}

void QLayerView::onActionPasteTriggered()
{
    Q_ASSERT(model());
    Q_ASSERT(mpCurDoc);

    QAbstractItemModel* pModel = model();

    if (mLayerClipStatus == Cut)
    {
        std::vector<int> posVec;
        posVec.reserve(mLayerClipList.size());

        foreach (const auto& index, mLayerClipList)
            posVec.push_back(index.row());

        mpCurDoc->getLayerManager().moveLayers(posVec, mContextIndex.isValid() ? pModel->rowCount() - mContextIndex.row() : 0);
    }

    mLayerClipList.clear();
    mLayerClipStatus = None;
}

void QLayerView::onActionSwitchVisibleTriggered()
{
    Q_ASSERT(model());
    Q_ASSERT(mpCurDoc);

    //判断是否应设置为可见
    mpCurDoc->getLayerManager().setVisible(mpActionSwitchVisible->text() == tr("Show"));

    //清除选择
    mpCurDoc->getLayerManager().clearSelection();
}

void QLayerView::onActionRemoveTriggered()
{
    Q_ASSERT(model());
    Q_ASSERT(mpCurDoc);

    mpCurDoc->getLayerManager().removeSelectedLayers();

    mpCurDoc->getLayerManager().clearSelection();
}

void QLayerView::onActionScaleToFitTriggered()
{
    emit fitViewToArea(mpCurDoc->getLayerManager().getBoundingRectOfSelected());
}

void QLayerView::onActionAttributeTriggered()
{

}

void QLayerView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    QTreeView::selectionChanged(selected, deselected);

    Q_ASSERT(model());
    Q_ASSERT(mpCurDoc);

    std::vector<int> changedRowIndexVec;

    foreach (const auto& index, selected.indexes())
    {
        if (std::find(changedRowIndexVec.begin(), changedRowIndexVec.end(), index.row()) == changedRowIndexVec.end())
            changedRowIndexVec.push_back(index.row());
    }

    mpCurDoc->getLayerManager().select(changedRowIndexVec);
}

std::shared_ptr<SDocument> QLayerView::getDocument()
{
    return this->mpCurDoc;
}

void QLayerView::setDocument(std::shared_ptr<SDocument> pCurrentDocument)
{
    if (mpCurDoc)
        _disconnectDocument();

    this->mpCurDoc = pCurrentDocument;
    this->_updateLayerModel();

    _connectDocument();
}

void QLayerView::_updateLayerModel()
{
    if (!mpCurDoc)
        return;
    //QStandardItemModel* pModel = const_cast<QStandardItemModel*>(mpCurDoc->getLayerManager().getLayerModel());
    this->setModel(mpCurDoc->getLayerManager().getLayerModel());
}

void QLayerView::_setBand(int which, int band)
{
    Q_ASSERT(mpCurDoc);
    Q_ASSERT(which == 0 || which == 1 || which == 2);

    SFragImage* pRaster = dynamic_cast<SFragImage*>(*mpCurDoc->getLayerManager().getSelectedLayerIterList().front());

    switch (which)
    {
    case 0:
        pRaster->setBandIndices(band, pRaster->greenBandIndex(), pRaster->blueBandIndex());
        break;
    case 1:
        pRaster->setBandIndices(pRaster->redBandIndex(), band, pRaster->blueBandIndex());
        break;
    case 2:
        pRaster->setBandIndices(pRaster->redBandIndex(), pRaster->greenBandIndex(), band);
        break;
    }
}


void QLayerView::_initialize()
{
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

    _initializeContextMenu();
    _initializeConnections();
}

void QLayerView::_initializeContextMenu()
{
    //上下文右键菜单选项初始化
    mpContextMenu           = new QMenu();

    mpSubMenuAdd            = new QMenu(tr("Add"),              mpContextMenu);

    mpActionAddRaster       = new QAction(tr("Raster"),         mpSubMenuAdd);
    mpActionAddShapeFile    = new QAction(tr("Shape File"),     mpSubMenuAdd);

    mpSubMenuAdd->addActions({mpActionAddRaster, mpActionAddShapeFile});

    mpActionCopy            = new QAction(tr("Copy"),           mpContextMenu);
    mpActionCut             = new QAction(tr("Cut"),            mpContextMenu);
    mpActionPaste           = new QAction(tr("Paste"),          mpContextMenu);

    mpActionSwitchVisible   = new QAction(tr("Hide"),           mpContextMenu);
    mpActionRemove          = new QAction(tr("Remove"),         mpContextMenu);

    mpActionScaleToFit      = new QAction(tr("Scale to Fit"),   mpContextMenu);

    mpSubMenuSetRedBand     = new QMenu(tr("Red Band"),         mpContextMenu);
    mpSubMenuSetGreenBand   = new QMenu(tr("Green Band"),       mpContextMenu);
    mpSubMenuSetBlueBand    = new QMenu(tr("Blue Band"),        mpContextMenu);

    mpActionAttribute       = new QAction(tr("Attribute..."),   mpContextMenu);


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

    mpContextMenu->addSection(tr("Raster Bands"));

    mpContextMenu->addMenu(mpSubMenuSetRedBand);
    mpContextMenu->addMenu(mpSubMenuSetGreenBand);
    mpContextMenu->addMenu(mpSubMenuSetBlueBand);

    mpContextMenu->addSeparator();

    mpContextMenu->addAction(mpActionAttribute);

    //响应信号
    connect(mpActionAddRaster,      &QAction::triggered, this, &QLayerView::onActionAddRasterTriggered);
    connect(mpActionAddShapeFile,   &QAction::triggered, this, &QLayerView::onActionAddShapeFileTriggered);
    connect(mpActionCopy,           &QAction::triggered, this, &QLayerView::onActionCopyTriggered);
    connect(mpActionCut,            &QAction::triggered, this, &QLayerView::onActionCutTriggered);
    connect(mpActionPaste,          &QAction::triggered, this, &QLayerView::onActionPasteTriggered);
    connect(mpActionSwitchVisible,  &QAction::triggered, this, &QLayerView::onActionSwitchVisibleTriggered);
    connect(mpActionRemove,         &QAction::triggered, this, &QLayerView::onActionRemoveTriggered);
    connect(mpActionScaleToFit,     &QAction::triggered, this, &QLayerView::onActionScaleToFitTriggered);
    connect(mpActionAttribute,      &QAction::triggered, this, &QLayerView::onActionAttributeTriggered);
}

void QLayerView::_initializeConnections()
{
    _connectDocument();
}

void QLayerView::_connectDocument()
{
    if (!mpCurDoc)
        return;
    connect(&mpCurDoc->getLayerManager(), &SLayerManager::selectStateChanged, this, &QLayerView::onLayerManagerSelectChanged);
    connect(this, &QLayerView::clicked, &mpCurDoc->getLayerManager(), &SLayerManager::onLayerViewClicked);
}

void QLayerView::_disconnectDocument()
{
    if (!mpCurDoc)
        return;
    disconnect(&mpCurDoc->getLayerManager(), &SLayerManager::selectStateChanged, this, &QLayerView::onLayerManagerSelectChanged);
    disconnect(this, &QLayerView::clicked, &mpCurDoc->getLayerManager(), &SLayerManager::onLayerViewClicked);
}

