#ifndef SFRAGLOADER_H
#define SFRAGLOADER_H

#include <QThread>
#include <QMutex>
#include <deque>
#include <modules/paint/simage.h>
#include "modules/global.h"

//加载器线程类
class SFragLoader : public QThread
{
    Q_OBJECT
    /*-----构造函数与析构函数-----*/
public:
    explicit SFragLoader(size_t fragTempSize = DEFAULT_FRAG_TEMP_SIZE, size_t readInStackSize = DEFAULT_READ_IN_STACK_SIZE);
    ~SFragLoader();

    /*-----虚函数重载-----*/
public:
    void run()override;

    /*-----信号-----*/
signals:
    void imageLoaded(SImage* pImage);

    /*-----槽函数-----*/
private slots:

    /*-----成员变量-----*/
protected:
    //最大分片缓存队列大小，该大小决定了所有使用该分片加载类对象的共享内存大小
    size_t muMaxFragCount{};
    //最大读取栈大小
    size_t muMaxReadInStackSize{};
    //双向队列用作栈使用
    //读入栈：用于存放读入序列，队列的容量由muMaxFragCount确定，处于栈顶(front)的对象将被优先读入
    std::deque<SImage*> mReadInStack{};
    //缓存队列：用于存放已经加载的分片，队列的容量由muMaxFragCount确定，处于队首(front)的对象将被优先清除
    std::deque<SImage*> mFragTempQueue{};


    /*-----成员函数-----*/
public:
    //[访问函数]
    inline size_t stackSize();
    //[修改函数]
    inline void clearReadInStack();
    inline void setStackSize(size_t size);
    //单个压入读取栈
    void push_front(SImage *image);
    //多个压入读取栈，栈顶元素将是pImageArray的第一个元素
    void push_front(SImage **pImage, size_t count);
    //[功能函数]
    void releaseAll();
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
