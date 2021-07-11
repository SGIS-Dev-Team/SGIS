#include "slayermanager.h"
#include "modules/paint/sobjectfactory.h"

SLayerManager::SLayerManager()
{
    //初始化图层数据模型
    QStringList header;
    header << tr("Visible") << tr("Name") << tr("Type");
    mLayerModel.setHorizontalHeaderLabels(header);
}

SLayerManager::~SLayerManager()
{
    for (auto& pObj : mLayerList)
        SObjectFactory::releaseObject(pObj);
    mLayerList.clear();
    mLayerModel.clear();
    mSelectedLayerIterList.clear();
}

void SLayerManager::onLayerViewClicked(const QModelIndex& index)
{
    if (index.model() != &this->mLayerModel)
        return;
    //处理图层可见性
    if (index.column() == 0)
    {
        QStandardItem* pItem = this->mLayerModel.itemFromIndex(index);
        SObject* obj = *_iterAt(_posSwitch(index.row()));
        obj->setVisible(pItem->checkState() == Qt::Checked ? true : false);
        emit layersUpdated(this);
    }
}

void SLayerManager::addLayer(SObject* obj)
{
    insertLayers({obj}, mLayerList.end());
}

void SLayerManager::addLayers(const std::vector<SObject*>& objVec)
{
    insertLayers(objVec, mLayerList.end());
}

void SLayerManager::insertLayer(SObject* obj, int pos)
{
    insertLayers({obj}, _iterAt(pos));
}

void SLayerManager::insertLayer(SObject* obj, list_iterator it)
{
    insertLayers({obj}, it);
}

void SLayerManager::insertLayers(const std::vector<SObject*>& objVec, int pos)
{
    insertLayers(objVec, _iterAt(pos));
}

void SLayerManager::insertLayers(const std::vector<SObject*>& objVec, list_iterator it)
{
    int nRow = _posSwitch(_posOf(it));

    for (SObject* obj : objVec)
    {
        Q_ASSERT(obj);
        //这样插入会使得在objVec索引值靠前的图层最后在更低层
        list_iterator inserted_iter = mLayerList.insert(it, obj);
        mLayerModel.insertRow(nRow++, _createRowItem(obj));

        if (obj->isSelected())
            mSelectedLayerIterList.push_back(inserted_iter);
    }

    clearSelection();
    emit layersUpdated(this);
}

void SLayerManager::replaceLayer(size_t pos, SObject* newLayer)
{
    list_iterator iter = _iterAt(pos);
    replaceLayer(iter, newLayer);
}

void SLayerManager::replaceLayer(SObject* oldLayer, SObject* newLayer)
{
    list_iterator iter = std::find(mLayerList.begin(), mLayerList.end(), oldLayer);
    replaceLayer(iter, newLayer);
}

void SLayerManager::replaceLayer(list_iterator it, SObject* newLayer)
{
    SObjectFactory::releaseObject(*it);
    *it = newLayer;
    int pos = _posSwitch(_posOf(it));

    this->mLayerModel.removeRow(pos);
    this->mLayerModel.insertRow(pos, _createRowItem(newLayer));

    clearSelection();
    emit layersUpdated(this);
}

void SLayerManager::removeLayer(SObject* obj)
{
    if (!obj)
        return;
    list_iterator iter = std::find(mLayerList.begin(), mLayerList.end(), obj);

    if (iter == mLayerList.end())
        return;

    removeLayer(iter);
}

void SLayerManager::removeLayer(size_t pos)
{
    list_iterator iter = _iterAt(pos);

    if (iter == mLayerList.end())
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

    clearSelection();
    emit layersUpdated(this);
}

const layer_list& SLayerManager::getLayerList()const
{
    return mLayerList;
}

const std::list<list_iterator>& SLayerManager::getSelectedLayerIterList() const
{
    return mSelectedLayerIterList;
}

std::vector<int> SLayerManager::getSelectedRowIndices() const
{
    std::vector<int> idxVec;
    idxVec.reserve(mSelectedLayerIterList.size());

    for (const auto& iter : mSelectedLayerIterList)
    {
        idxVec.emplace_back(_posSwitch(_posOf(iter)));
    }

    return idxVec;
}

const QStandardItemModel* SLayerManager::getLayerModel()const
{
    return &mLayerModel;
}

SObject* SLayerManager::getTopLayerOn(const QPointF& pt, bool onlySelected, bool inBoundRect) const
{
    layer_list::const_reverse_iterator riter;

    for (riter =  mLayerList.crbegin(); riter != mLayerList.crend(); ++riter)
        if ((!onlySelected || (*riter)->isSelected()) && (*riter)->contains(QPointF(pt), inBoundRect))
            return *riter;

    return nullptr;
}

SObject* SLayerManager::layerAt(size_t pos)
{
    return *_iterAt(pos);
}

void SLayerManager::clickSelect(const QPointF& pt, bool doMultiSelect)
{
    layer_list::reverse_iterator iter;
    bool isPtInObject{false};
    //自顶向下遍历图层链表
    for (iter = mLayerList.rbegin(); iter != mLayerList.rend(); ++iter)
    {
        if ((*iter)->contains(pt))
        {
            isPtInObject = true;

            //若是单选，已经选中多项时，不做任何事
            if (!doMultiSelect && mSelectedLayerIterList.size() > 1)
                break;
            //若鼠标点所在的最顶层的图层在选择链表中，则去除；
            //若鼠标点所在的最顶层的图层不在选择链表中: 若是多选则添加，若是单选但选择列表为空则添加，否则清空选择
            bool isObjInSelectList{false}; //该图层是否已经在选择列表中
            for (const list_iterator& selectedIter : mSelectedLayerIterList)
            {
                if (*selectedIter == *iter)
                {
                    isObjInSelectList = true;

                    (*selectedIter)->setSelected(false);
                    mSelectedLayerIterList.remove(selectedIter);
                    break;
                }
            }

            if (!isObjInSelectList)
            {
                if (!doMultiSelect && mSelectedLayerIterList.size() != 0)
                    mSelectedLayerIterList.clear();

                mSelectedLayerIterList.push_back(--iter.base());
                (*iter)->setSelected(true);
            }
            break;
        }
    }

    if (!isPtInObject && !doMultiSelect)
        mSelectedLayerIterList.clear();

    //如果不是[多选且点空]，则发射选择改变信号
    if (!(doMultiSelect && !isPtInObject))
        emit selectStateChanged();
}

void SLayerManager::select(const std::vector<int>& indexVec)
{
    for (auto& idx : indexVec)
    {
        auto iter = _iterAt(_posSwitch(idx));
        (*iter)->setSelected(true);
        mSelectedLayerIterList.push_back(iter);
    }
    emit selectStateChanged();
}

void SLayerManager::select(int index)
{
    select(std::vector<int>({index}));
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
    for (size_t i = 0; i < pos; ++i)
        ++iter;
    return iter;
}

size_t SLayerManager::_posOf(list_iterator it)const
{
    size_t pos{0};
    while (it != mLayerList.begin())
    {
        ++pos;
        --it;
    }
    return pos;
}

QList<QStandardItem*> SLayerManager::_createRowItem(SObject* obj)
{
    Q_ASSERT(obj != nullptr);

    QStandardItem* itemCheck = new QStandardItem;
    QStandardItem* itemLayerIcon = new QStandardItem(obj->icon(), obj->layerName());
    QStandardItem* itemLayerType = new QStandardItem(SObject::typeString(obj->getType()));
    itemCheck->setCheckable(true);
    itemCheck->setCheckState(Qt::Checked);

    QList<QStandardItem*> rowData;
    rowData << itemCheck << itemLayerIcon << itemLayerType;

    return rowData;
}

void SLayerManager::bringForward()
{
    _reOrderLayerList(
        [](SLayerManager * mgr)->list_iterator
    {
        list_iterator insertPos = mgr->mSelectedLayerIterList.back();
        if (++insertPos != mgr->mLayerList.end())
            ++insertPos;
        return insertPos;
    });
}

void SLayerManager::sendBackward()
{
    _reOrderLayerList(
        [](SLayerManager * mgr)->list_iterator
    {
        list_iterator insertPos = mgr->mSelectedLayerIterList.front();
        if (insertPos != mgr->mLayerList.begin())
            --insertPos;
        return insertPos;
    });
}

void SLayerManager::bringToFront()
{
    _reOrderLayerList(
        [](SLayerManager * mgr)->list_iterator{return mgr->mLayerList.end();});
}

void SLayerManager::sendToBack()
{
    _reOrderLayerList(
        [](SLayerManager * mgr)->list_iterator{return mgr->mLayerList.begin();});
}

void SLayerManager::_sortSelectList()
{
    std::map<int, list_iterator> sltIterMap;
    for (auto& iter : mSelectedLayerIterList)
        sltIterMap[_posOf(iter)] = iter;

    mSelectedLayerIterList.clear();

    for (auto& value : sltIterMap)
        mSelectedLayerIterList.push_back(value.second);
}

void SLayerManager::_reOrderLayerList(list_iterator(*getInsertPos)(SLayerManager*))
{
    if (mSelectedLayerIterList.empty()) return;
    //先按层序排序
    _sortSelectList();
    //找到目标层的迭代器
    list_iterator insertPosIt = getInsertPos(this);
    //缓存链表
    std::list<list_iterator> sltIterTmpList;
    //执行插入操作，并保存迭代器在缓存链表中
    for (auto& iter : mSelectedLayerIterList)
        sltIterTmpList.push_back(mLayerList.insert(insertPosIt, *iter));
    //清除原来的图层：和上面的语句合写会导致迭代器非法化，影响图层下移两个函数的实现
    for (auto& iter : mSelectedLayerIterList)
        mLayerList.erase(iter);
    mSelectedLayerIterList = sltIterTmpList;
}
