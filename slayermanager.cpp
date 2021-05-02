#include "slayermanager.h"

SLayerManager::SLayerManager()
{
    std::list<SObject*>::const_iterator iter = mLayerList.begin();
}

void SLayerManager::addLayer(SObject *obj)
{
    this->mLayerList.push_back(obj);
}

void SLayerManager::addLayers(std::vector<SObject *> objVec)
{
    for(SObject* obj : objVec)
        this->mLayerList.push_back(obj);
}

std::list<SObject*> & SLayerManager::getLayerList()
{
    return mLayerList;
}

SObject &SLayerManager::layerAt(size_t pos) const
{
    if(pos > mLayerList.size())
        throw "Array Index out of bound.";
    std::list<SObject*>::const_iterator iter = mLayerList.begin();
    for(size_t i = 0; i < pos; ++i)
        ++iter;
    return *(*iter);
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

            return *iter;
        }
    }
    return nullptr;
}

void SLayerManager::clearSelection()
{
    this->mSelectedLayerIterList.clear();
}
