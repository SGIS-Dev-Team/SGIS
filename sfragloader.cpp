#include "sfragloader.h"

SFragLoader::SFragLoader(size_t fragStackSize): muMaxFragCount(fragStackSize)
{
    mReadInStack.resize(muMaxFragCount);
}

void SFragLoader::run()
{
    if(mReadInStack.empty())
        return;
    //读取策略：
    //从栈顶向下读取，寻找第一个空指针，读入
    //读入后重新从栈顶读取，递归
    for(auto &ppImage : mReadInStack)
    {
        if(*ppImage == nullptr)
        {

        }
    }
}


void SFragLoader::push_front(SImage *&rpImage)
{
    //如果rpImage已有值，则不做任何事
    if(rpImage)
        return;

    //容量已满，则将栈底部弹出
    if(mReadInStack.size() == muMaxFragCount)
        mReadInStack.pop_back();

    //将参数加入栈顶
    mReadInStack.push_front(&rpImage);
}

void SFragLoader::push_front(SImage **ppImageArray[], size_t count)
{

}
