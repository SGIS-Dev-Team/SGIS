#include "slayermanager.h"

SLayerManager::SLayerManager()
{
    //初始化图层数据模型
    QStringList header;
    header << tr("Visible") << tr(" ") << tr("Name");
    mLayerModel.setHorizontalHeaderLabels(header);
}

void SLayerManager::addLayer(SObject *obj)
{
    if(!obj)
        throw "Object Pointer = nullptr";
    this->mLayerList.push_back(obj);

    this->mLayerModel.insertRow(0, createItem(obj));
}

void SLayerManager::addLayers(std::vector<SObject *> objVec)
{
    for(SObject* obj : objVec)
    {
        if(!obj)
            throw "Object Pointer = nullptr";
        this->mLayerList.push_back(obj);

        this->mLayerModel.insertRow(0, createItem(obj));
    }
}

void SLayerManager::replaceLayer(size_t pos, SObject* newLayer)
{
    *iterAt(pos) = newLayer;
}

void SLayerManager::replaceLayer(SObject *oldLayer, SObject *newLayer)
{
    *std::find(mLayerList.begin(), mLayerList.end(), oldLayer) = newLayer;
}

void SLayerManager::replaceLayer(std::list<SObject*>::iterator it, SObject *newLayer)
{
    *it = newLayer;
}

void SLayerManager::removeLayer(SObject *obj)
{
    if(!obj)
        return;
    std::list<SObject*>::iterator iter = std::find(mLayerList.begin(), mLayerList.end(), obj);

    if(iter == mLayerList.end())
        return;

    removeLayer(iter);
}

void SLayerManager::removeLayer(size_t pos)
{
    std::list<SObject*>::iterator iter = iterAt(pos);

    if(iter == mLayerList.end())
        return;

    removeLayer(iter);
}

void SLayerManager::removeLayer(std::list<SObject*>::iterator it)
{
    //从选中链表中删除图层
    mSelectedLayerIterList.remove(it);
    mLayerList.erase(it);

    if(*it)
        delete *it;
}

std::list<SObject*> & SLayerManager::getLayerList()
{
    return mLayerList;
}

SObject &SLayerManager::layerAt(size_t pos)
{
    return *(*iterAt(pos));
}

const SObject *SLayerManager::clickSelect(const QPoint & pt)
{
    std::list<SObject*>::reverse_iterator iter;
    //自顶向下遍历图层链表
    for(iter = mLayerList.rbegin(); iter != mLayerList.rend(); ++iter)
    {
        if((*iter)->contains(pt))
        {
            //若鼠标点所在的最顶层的图层不在选择链表中，则添加，否则去除
            bool deselected{false}; //是否进行了去除选择操作
            for(const std::list<SObject*>::iterator& selectedIter : mSelectedLayerIterList)
            {
                if(*selectedIter == *iter)
                {
                    mSelectedLayerIterList.remove(selectedIter);
                    (*selectedIter)->setSelected(false);
                    deselected = true;
                    break;
                }
            }
            if(!deselected)
            {
                mSelectedLayerIterList.push_back(iter.base());
                (*iter)->setSelected(true);
            }
            //发射选择改变信号
            emit selectStateChanged();
            return *iter;
        }
    }
    return nullptr;
}

void SLayerManager::clearSelection()
{
    this->mSelectedLayerIterList.clear();
    emit selectStateChanged();
}

std::list<SObject *>::iterator SLayerManager::iterAt(size_t pos)
{
    if(pos > mLayerList.size())
        throw "Array Index out of bound.";
    std::list<SObject*>::iterator iter = mLayerList.begin();
    for(size_t i = 0; i < pos; ++i)
        ++iter;
    return iter;
}

QStandardItem *SLayerManager::createItem(SObject *obj)
{
    if(!obj)
        throw "Object Pointer = nullptr";

    QStandardItem* newItem = new QStandardItem;
    newItem->setCheckable(true);
    newItem->setDragEnabled(true);
    newItem->setDropEnabled(true);

    //newItem->set

    return newItem;
}
