#ifndef SLAYERMANAGER_H
#define SLAYERMANAGER_H

#include <list>
#include <sobject.h>
#include <QStandardItemModel>
#include <QItemSelectionModel>

//------------------------------------------
//      SLayerManager类
//         图层管理器
//  该类含有一个由SObject对象指针构成的链表
//  注意：SObject对象在类外构造时，对象资源由该类接管，
//不要在类外释放其内存。
//
//
//
//------------------------------------------

typedef std::list<SObject*>::iterator list_iterator;
typedef std::list<SObject*> layer_list ;

class SLayerManager: public QObject
{
    Q_OBJECT
    /*-----构造函数与析构函数-----*/
public:
    explicit SLayerManager();

    /*-----信号-----*/
signals:
    void layersUpdated(SLayerManager* which);
    void selectStateChanged();

    /*-----槽函数-----*/
private slots:

    /*-----成员变量-----*/
private:

    //图层链表
    layer_list mLayerList;
    //选中的图层在链表中的迭代器组成的链表
    std::list<list_iterator> mSelectedLayerIterList;

    //图层数据模型
    QStandardItemModel mLayerModel;
    //图层选中模型
    QItemSelectionModel mSelectedLayerModel;

    /*-----成员函数-----*/

    //[访问与修改函数]
public:
    //添加图层在顶部
    void addLayer(SObject *obj);
    void addLayers(std::vector<SObject*> objVec);

    //更换图层
    void replaceLayer(size_t pos, SObject* newLayer);
    void replaceLayer(SObject* oldLayer, SObject* newLayer);
    void replaceLayer(std::list<SObject*>::iterator it, SObject* newLayer);

    //删除图层
    void removeLayer(SObject* obj);
    void removeLayer(size_t pos);
    void removeLayer(std::list<SObject*>::iterator it);

    //搜索图层:搜索方向自顶向下

    //获取图层链表
    const layer_list &getLayerList()const;
    //获取图层数据模型
    const QStandardItemModel *getLayerModel()const;
    //按位置（从下往上数）获取图层
    SObject &layerAt(size_t pos);

    //按对象类型获取图层

    //选择图层(函数会将鼠标位置的最顶层图层加入选择，若已加入则剔除选择)
    const SObject *clickSelect(const QPoint& pt);

    //清空选择
    void clearSelection();

    //[功能函数]
private:
    //使用位置索引链表迭代器：O(n),尽量单次使用
    list_iterator _iterAt(size_t pos);
    //通过迭代器反求位置
    size_t _posOf(list_iterator it);
    //计算链表中的位置对应的数据模型行数
    inline size_t _posOfModel(size_t pos_of_List);
    //使用SObject*初始化图层数据项
    QList<QStandardItem *> _createRowItem(SObject* obj);
};

size_t SLayerManager::_posOfModel(size_t pos_of_List)
{
    assert(pos_of_List < mLayerList.size());
    return mLayerList.size() - pos_of_List - 1;
}

#endif // SLAYERMANAGER_H
