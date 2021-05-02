#ifndef SLAYERMANAGER_H
#define SLAYERMANAGER_H

#include <list>
#include <sobject.h>

//------------------------------------------
//      SLayerManager类
//         图层管理器
//  该类含有一个由SObject对象指针构成的链表
//  注意：SObject对象在类外构造时，对象资源由该类接管，
//不要在类外释放其内存也不要再访问该对象。
//
//
//
//------------------------------------------

class SLayerManager
{

    /*-----构造函数与析构函数-----*/
public:
    explicit SLayerManager();

    /*-----成员变量-----*/
private:
    //图层链表
    std::list<SObject*> mLayerList;

    //选中的图层在链表中的迭代器组成的链表
    std::list<std::list<SObject*>::iterator> mSelectedLayerIterList;

    /*-----成员函数-----*/

    //[访问与修改函数]
public:
    void addLayer(SObject *obj);
    void addLayers(std::vector<SObject*> objVec);

    //获取图层链表
    std::list<SObject *> &getLayerList();
    //按位置（从下往上数）获取图层
    SObject &layerAt(size_t pos)const;

    //按对象类型获取图层

    //选择图层(函数会将鼠标位置的最顶层图层加入选择，若已加入则剔除选择)
    const SObject *clickSelect(const QPoint& pt);

    //清空选择
    void clearSelection();

    //[功能函数]
private:


};

#endif // SLAYERMANAGER_H
