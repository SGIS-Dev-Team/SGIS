#include "sfragloader.h"

SFragLoader::SFragLoader(size_t fragTempSize, size_t readInStackSize): muMaxFragCount(fragTempSize), muMaxReadInStackSize(readInStackSize)
{
}

SFragLoader::~SFragLoader()
{

}

void SFragLoader::doPaintFrag(QPainter &painter)
{
    paint(painter);
}

void SFragLoader::doLoadFrag()
{
    //循环读入栈顶分片
    while(!mReadInStack.empty())
    {
        SImage* pImage = mReadInStack.front();
        Q_ASSERT(pImage);

        if(pImage->isNull())
        {
            pImage->load();
            emit imageLoaded(pImage);

            //缓存队列超出则清除队首元素
            if(mFragTempQueue.size() == muMaxFragCount)
            {
                mFragTempQueue.front()->releaseImage();
                mFragTempQueue.pop_front();
                //qDebug() << "Temp Queue Full.";
            }
            mFragTempQueue.push_back(pImage);
        }
        mReadInStack.pop_front();
    }
}

void SFragLoader::push_front(SImage *pImage)
{
    //如果为空指针或已加载图像，则不做任何事
    if(!pImage || !pImage->isNull())
        return;

    //容量已满，则将栈底部弹出
    if(mReadInStack.size() == muMaxReadInStackSize)
        mReadInStack.pop_back();

    //将image加入栈顶
    mReadInStack.push_front(pImage);
}

void SFragLoader::push_front(SImage** ppImage, size_t count)
{
    //保证压入量不大于缓存数
    count = count > muMaxReadInStackSize ? muMaxReadInStackSize : count;

    for(int i = count - 1; i >= 0; --i)
    {
        if(!ppImage[i] || !ppImage[i]->isNull())
            continue;
        mReadInStack.push_front(ppImage[i]);
        if(mReadInStack.size() == muMaxReadInStackSize)
            mReadInStack.pop_back();
    }
}

void SFragLoader::releaseAll()
{
    for(auto& pImage : mFragTempQueue)
        if(pImage)
            pImage->releaseImage();
}

void SFragLoader::paint(QPainter &painter)
{
    for(auto& pImage : mFragTempQueue)
        if(pImage)
            pImage->paint(painter);
}
