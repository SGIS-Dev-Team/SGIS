#include "slayermanager.h"
#include "modules/paint/sobjectfactory.h"

SLayerManager::SLayerManager()
{
    //初始化图层数据模型
    QStringList header;
    header << tr("Visible") << tr(" ");
    mLayerModel.setHorizontalHeaderLabels(header);
}

void SLayerManager::onLayerViewClicked(const QModelIndex &index)
{
    if(index.model() != &this->mLayerModel)
        return;
    //处理图层可见性
    if(index.column() == 0)
    {
        QStandardItem* pItem = this->mLayerModel.itemFromIndex(index);
        SObject* obj = *_iterAt(_posSwitch( index.row() ) );
        obj->setVisible(pItem->checkState() == Qt::Checked ? true : false);
        emit layersUpdated(this);
    }
}

void SLayerManager::addLayer(SObject *obj)
{
    Q_ASSERT(obj != nullptr);
    //更新图层链表
    this->mLayerList.push_back(obj);
    //更新图层数据模型
    this->mLayerModel.insertRow(0, _createRowItem(obj));
    //更新选中图层迭代器链表
    if(obj->isSelected())
        this->mSelectedLayerIterList.push_back(--mLayerList.end());
    //发射图层更新信号
    emit layersUpdated(this);
}

void SLayerManager::addLayers(std::vector<SObject *> objVec)
{
    for(SObject* obj : objVec)
    {
        Q_ASSERT(obj != nullptr);
        this->mLayerList.push_back(obj);
        this->mLayerModel.insertRow(0, _createRowItem(obj));
        if(obj->isSelected())
            this->mSelectedLayerIterList.push_back(mLayerList.rbegin().base());
    }
    emit layersUpdated(this);
}

void SLayerManager::replaceLayer(size_t pos, SObject* newLayer)
{
    list_iterator iter = _iterAt(pos);
    replaceLayer(iter, newLayer);
}

void SLayerManager::replaceLayer(SObject *oldLayer, SObject *newLayer)
{
    list_iterator iter = std::find(mLayerList.begin(), mLayerList.end(), oldLayer);
    replaceLayer(iter, newLayer);
}

void SLayerManager::replaceLayer(list_iterator it, SObject *newLayer)
{
    SObjectFactory::releaseObject(*it);
    *it = newLayer;
    this->mLayerModel.setItem(static_cast<int>(_posSwitch(_posOf(it))), 0, *_createRowItem(newLayer).toVector().data());
    emit layersUpdated(this);
}

void SLayerManager::removeLayer(SObject *obj)
{
    if(!obj)
        return;
    list_iterator iter = std::find(mLayerList.begin(), mLayerList.end(), obj);

    if(iter == mLayerList.end())
        return;

    removeLayer(iter);
}

void SLayerManager::removeLayer(size_t pos)
{
    list_iterator iter = _iterAt(pos);

    if(iter == mLayerList.end())
        return;

    removeLayer(iter);
}

void SLayerManager::removeLayer(list_iterator it)
{
    //从选中链表中删除图层
    mSelectedLayerIterList.remove(it);
    mLayerList.erase(it);

    SObjectFactory::releaseObject(*it);
    //从数据模型中删除图层
    mLayerModel.removeRow(_posSwitch(_posOf(it)));
    emit layersUpdated(this);
}

const layer_list & SLayerManager::getLayerList()const
{
    return mLayerList;
}

const std::list<list_iterator> &SLayerManager::getSelectedLayerIterList() const
{
    return mSelectedLayerIterList;
}

const QStandardItemModel *SLayerManager::getLayerModel()const
{
    return &mLayerModel;
}

SObject *SLayerManager::getTopLayerOn(const QPointF &pt, bool onlySelected, bool inBoundRect) const
{
    layer_list::const_reverse_iterator riter;

    for(riter =  mLayerList.crbegin(); riter != mLayerList.crend(); ++riter)
        if((!onlySelected || (*riter)->isSelected()) && (*riter)->contains(QPointF(pt), inBoundRect))
            return *riter;

    return nullptr;
}

SObject *SLayerManager::layerAt(size_t pos)
{
    return *_iterAt(pos);
}


void SLayerManager::clickSelect(const QPointF &pt, bool doMultiSelect)
{
    layer_list::reverse_iterator iter;
    bool isPtInObject{false};
    //自顶向下遍历图层链表
    for(iter = mLayerList.rbegin(); iter != mLayerList.rend(); ++iter)
    {
        if((*iter)->contains(pt))
        {
            isPtInObject = true;

            //若是单选，已经选中多项时，不做任何事
            if(!doMultiSelect && mSelectedLayerIterList.size() > 1)
                break;
            //若鼠标点所在的最顶层的图层在选择链表中，则去除；
            //若鼠标点所在的最顶层的图层不在选择链表中: 若是多选则添加，若是单选但选择列表为空则添加，否则清空选择
            bool isObjInSelectList{false}; //该图层是否已经在选择列表中
            for(const list_iterator& selectedIter : mSelectedLayerIterList)
            {
                if(*selectedIter == *iter)
                {
                    isObjInSelectList = true;

                    (*selectedIter)->setSelected(false);
                    mSelectedLayerIterList.remove(selectedIter);
                    break;
                }
            }

            if(!isObjInSelectList)
            {
                if(!doMultiSelect && mSelectedLayerIterList.size() != 0)
                    mSelectedLayerIterList.clear();

                mSelectedLayerIterList.push_back(--iter.base());
                (*iter)->setSelected(true);
            }
            break;
        }
    }

    if(!isPtInObject && !doMultiSelect)
        mSelectedLayerIterList.clear();

    //如果不是[多选且点空]，则发射选择改变信号
    if(!(doMultiSelect && !isPtInObject))
        emit selectStateChanged();
}

void SLayerManager::clearSelection()
{
    this->mSelectedLayerIterList.clear();
    emit selectStateChanged();
}

list_iterator SLayerManager::_iterAt(size_t pos)
{
    Q_ASSERT(pos < mLayerList.size());

    list_iterator iter = mLayerList.begin();
    for(size_t i = 0; i < pos; ++i)
        ++iter;
    return iter;
}

size_t SLayerManager::_posOf(list_iterator it)
{
    size_t pos{0};
    while(it != mLayerList.begin())
    {
        ++pos;
        --it;
    }
    return pos;
}

QList<QStandardItem *> SLayerManager::_createRowItem(SObject *obj)
{
    Q_ASSERT(obj != nullptr);

    QStandardItem* itemCheck = new QStandardItem;
    QStandardItem* itemLayerIcon = new QStandardItem(obj->icon(), obj->layerName());
    itemCheck->setCheckable(true);
    itemCheck->setCheckState(Qt::Checked);


    QList<QStandardItem *> rowData;
    rowData << itemCheck << itemLayerIcon;
    return rowData;
}

