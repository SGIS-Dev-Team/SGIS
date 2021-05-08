#ifndef SFRAGLOADER_H
#define SFRAGLOADER_H

#include <QThread>
#include <deque>
#include <simage.h>
#include "global.h"

class SFragLoader : public QThread
{
    /*-----构造函数与析构函数-----*/
public:
    explicit SFragLoader(size_t fragStackSize);
    ~SFragLoader();

    /*-----虚函数重载-----*/
public:
    void run()override;

    /*-----信号-----*/
signals:

    /*-----槽函数-----*/
private slots:

    /*-----成员变量-----*/
protected:
    //最大分片栈大小，该大小决定了所有使用该分片加载类对象的共享内存大小
    size_t muMaxFragCount{DEFAULT_FRAG_STACK_SIZE};
    //双向队列用作栈使用
    //读入栈：用于存放读入序列，队列的容量由muMaxFragCount确定
    std::deque<SImage**> mReadInStack{};
    //缓存队列：用于存放已经加载的分片，队列的容量由muMaxFragCount确定
    std::deque<SImage**> mTempStack{};
    /*-----成员函数-----*/
public:
    //[访问函数]
    inline size_t stackSize();
    //[修改函数]
    inline void clearReadInStack();
    inline void setStackSize(size_t size);
    //单个压入读取栈
    void push_front(SImage*& rpImage);
    //多个压入读取栈，栈顶元素将是ppImageArray的第一个元素
    void push_front(SImage** ppImageArray[], size_t count);
    //[功能函数]
};

size_t SFragLoader::stackSize()
{
    return muMaxFragCount;
}

void SFragLoader::setStackSize(size_t size)
{
    muMaxFragCount = size;
}

void SFragLoader::clearReadInStack()
{
    mReadInStack.clear();
}
#endif // SFRAGLOADER_H
