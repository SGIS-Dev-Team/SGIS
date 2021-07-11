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

    auto iter = _iterAt(_posSwitch(index.row()));

    //处理图层可见性
    if (index.column() == 0)
    {
        QStandardItem* pItem = this->mLayerModel.itemFromIndex(index);
        (*iter)->setVisible(pItem->checkState() == Qt::Checked ? true : false);

        if (mbUpdateSignal)
            emit layersUpdated(this);
    }

    //在选择找到该项
    auto sltIter = std::find(mSelectedLayerIterList.begin(), mSelectedLayerIterList.end(), iter);

    //若选择链表中没有，但该图层被标记为选中
    //if (sltIter == mSelectedLayerIterList.end() && (*iter)->isSelected())


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
    int nRow = mLayerList.size() - _posOf(it);

    //这样插入会使得在objVec索引值靠前的图层最后在更低层
    for (SObject* obj : objVec)
    {
        Q_ASSERT(obj);
        //插入到图层链表
        list_iterator inserted_iter = mLayerList.insert(it, obj);
        //插入到图层模型
        mLayerModel.insertRow(nRow++, _createRowItem(obj));
        //增加选择链表结点
        if (obj->isSelected())
            mSelectedLayerIterList.push_back(inserted_iter);
    }

    if (mbUpdateSignal)
    {
        emit selectStateChanged();
        emit layersUpdated(this);
    }
}

void SLayerManager::replaceLayer(int pos, SObject* newLayer)
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

    if (mbUpdateSignal)
    {
        emit selectStateChanged();
        emit layersUpdated(this);
    }
}

void SLayerManager::removeSelectedLayers()
{
    std::vector<list_iterator> iterVec;
    iterVec.reserve(mSelectedLayerIterList.size());

    for (auto& iter : mSelectedLayerIterList)
        iterVec.push_back(iter);

    removeLayers(iterVec);
}

void SLayerManager::removeLayer(SObject* obj)
{
    Q_ASSERT(obj);
    list_iterator iter = std::find(mLayerList.begin(), mLayerList.end(), obj);
    Q_ASSERT(iter != mLayerList.end());

    removeLayers({iter});
}

void SLayerManager::removeLayer(int pos)
{
    removeLayers({_iterAt(pos)});
}

void SLayerManager::removeLayer(list_iterator it)
{
    removeLayers(std::vector<list_iterator>({it}));
}

void SLayerManager::removeLayers(std::vector<SObject*> objVec)
{
    removeLayers(_objVecToIterVec(objVec));
}

void SLayerManager::removeLayers(std::vector<int> posVec)
{
    removeLayers(_posVecToIterVec(posVec));
}

void SLayerManager::removeLayers(std::vector<list_iterator> iterVec)
{
    for (auto& iter : iterVec)
    {
        //删除数据模型中的图层行
        mLayerModel.removeRow(_posSwitch(_posOf(iter)));
        //若被选中，则清除选择链表中的该图层
        if ((*iter)->isSelected())
            mSelectedLayerIterList.remove(iter);
        //释放该图层的内存
        SObjectFactory::releaseObject(*iter);
        //清除图层链表中的该项
        mLayerList.erase(iter);
    }

    if (mbUpdateSignal)
    {
        emit selectStateChanged();
        emit layersUpdated(this);
    }
}

void SLayerManager::moveLayer(SObject* srcObj, SObject* dstObj)
{
    list_iterator srcIter = std::find(mLayerList.begin(), mLayerList.end(), srcObj);
    list_iterator dstIter = std::find(mLayerList.begin(), mLayerList.end(), dstObj);

    Q_ASSERT(srcIter != mLayerList.end());

    moveLayers({srcIter}, dstIter);
}

void SLayerManager::moveLayer(int srcPos, int dstPos)
{
    moveLayers({_iterAt(srcPos)}, _iterAt(dstPos));
}

void SLayerManager::moveLayer(const list_iterator& srcIter, const list_iterator& dstIter)
{
    moveLayers({srcIter}, dstIter);
}

void SLayerManager::moveLayers(const std::vector<SObject*>& srcObjVec, SObject* dstObj)
{
    moveLayers(_objVecToIterVec(srcObjVec), std::find(mLayerList.begin(), mLayerList.end(), dstObj));
}

void SLayerManager::moveLayers(const std::vector<int>& srcPosVec, int dstPos)
{
    moveLayers(_posVecToIterVec(srcPosVec), _iterAt(dstPos));
}

void SLayerManager::moveLayers(const std::vector<list_iterator>& srcIterVec, const list_iterator& dstIter)
{
    std::vector<SObject*> srcObjVec;
    srcObjVec.reserve(srcIterVec.size());

    for (auto& iter : srcIterVec)
        srcObjVec.push_back(*iter);

    _beginModifyLayers();
    insertLayers(srcObjVec, dstIter);
    removeLayers(srcIterVec);
    _endModifyLayers();
}

const SObject* SLayerManager::operator[](int pos) const
{
    return *_iterAt(pos);
}

SObject* SLayerManager::operator[](int pos)
{
    return *_iterAt(pos);
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
        SObject* pObj = *iter;

        if (pObj->isSelected())
        {
            pObj->setSelected(false);

            std::list<list_iterator>::iterator sltIter = std::find(mSelectedLayerIterList.begin(), mSelectedLayerIterList.end(), iter);

            Q_ASSERT(sltIter != mSelectedLayerIterList.end());

            mSelectedLayerIterList.erase(sltIter);
        }
        else
        {
            pObj->setSelected(true);

            mSelectedLayerIterList.push_back(iter);
        }
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

list_iterator SLayerManager::_iterAt(int pos)
{
    Q_ASSERT(pos < mLayerList.size() && pos >= 0);

    list_iterator iter = mLayerList.begin();
    for (size_t i = 0; i < pos; ++i)
        ++iter;
    return iter;
}

list_const_iterator SLayerManager::_iterAt(int pos) const
{
    Q_ASSERT(pos < mLayerList.size() && pos >= 0);

    list_const_iterator iter = mLayerList.begin();
    for (size_t i = 0; i < pos; ++i)
        ++iter;
    return iter;
}

int SLayerManager::_posOf(list_iterator it)const
{
    int pos{0};
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

void SLayerManager::setVisible(bool visible)
{
    for (auto& iter : mSelectedLayerIterList)
    {
        (*iter)->setVisible(visible);
        mLayerModel.item(_posSwitch(_posOf(iter)), 0)->setCheckState(visible ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    }
    emit layersUpdated(this);
}

QRectF SLayerManager::getBoundingRectOfSelected() const
{
    double xMin{DBL_MAX}, xMax{-DBL_MAX}, yMin{DBL_MAX}, yMax{-DBL_MAX};

    for (auto& iter : mSelectedLayerIterList)
    {
        QPolygonF boundingRect = (*iter)->boundingRect();

        foreach (const QPointF& pt, boundingRect)
        {
            if (pt.x() < xMin)
                xMin = pt.x();
            if (pt.x() > xMax)
                xMax = pt.x();
            if (pt.y() < yMin)
                yMin = pt.y();
            if (pt.y() > yMax)
                yMax = pt.y();
        }
    }

    return QRectF(QPointF(xMin, yMin), QPointF(xMax, yMax));
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

std::vector<list_iterator> SLayerManager::_objVecToIterVec(const std::vector<SObject*>& objVec)
{
    std::vector<list_iterator> iterVec;
    iterVec.reserve(objVec.size());

    for (auto& pObj : objVec)
    {
        list_iterator iter = std::find(mLayerList.begin(), mLayerList.end(), pObj);
        Q_ASSERT(iter != mLayerList.end());
        iterVec.push_back(iter);
    }

    return iterVec;
}

std::vector<list_iterator> SLayerManager::_posVecToIterVec(const std::vector<int>& posVec)
{
    std::vector<list_iterator> iterVec;
    iterVec.reserve(posVec.size());

    for (auto& pos : posVec)
    {
        Q_ASSERT(pos >= 0 && pos < mLayerList.size());
        iterVec.push_back(_iterAt(pos));
    }

    return iterVec;
}
