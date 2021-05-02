#ifndef SLAYERMANAGER_H
#define SLAYERMANAGER_H

#include <QLinkedList>
#include <sobject.h>
class SLayerManager
{

    /*-----构造函数与析构函数-----*/
public:
    explicit SLayerManager();

    /*-----成员变量-----*/
private:
    //图层链表
    QLinkedList<SObject*> mLayerList;

    //选中的图层在链表中的迭代器组成的链表
    QLinkedList<QLinkedList<SObject*>::iterator> mSelectedLayerIterList;

    /*-----成员函数-----*/

    //[访问与修改函数]
public:
    void addLayer(const SObject &obj);
    void addLayers(QVector<SObject> objVec);

    //获取图层链表
    const QLinkedList<SObject>& getLayerList()const;
    //按位置（从下往上数）获取图层
    SObject* layerAt(size_t pos)const;

    //按对象类型获取图层

    //选择图层(函数会将鼠标位置的图层加入选择)
    const SObject* clickSelect(const QPoint& pos);

    //

    //清空选择
    void clearSelection();


    //[功能函数]
private:


};

#endif // SLAYERMANAGER_H
