#ifndef SLAYERMANAGER_H
#define SLAYERMANAGER_H

#include <list>
#include <modules/paint/sobject.h>
#include <QStandardItemModel>
#include <QItemSelectionModel>

//------------------------------------------
//      SLayerManager类
//         图层管理器
//  该类含有一个由SObject对象指针构成的链表
//  注意：SObject对象在类外构造时，对象资源由该类接管，
//不要在类外释放其内存。
//
//  该类使用逻辑坐标系
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
    virtual ~SLayerManager();
    /*-----信号-----*/
signals:
    void layersUpdated(SLayerManager* which);
    void selectStateChanged();

    /*-----槽函数-----*/
public slots:
    void onLayerViewClicked(const QModelIndex& index);
private slots:

    /*-----成员变量-----*/
private:

    //图层链表
    layer_list mLayerList;
    //选中的图层在链表中的迭代器组成的链表：[待定]可能vector更加适合
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
    void replaceLayer(list_iterator it, SObject* newLayer);

    //删除图层
    void removeLayer(SObject* obj);
    void removeLayer(size_t pos);
    void removeLayer(list_iterator it);

    //搜索图层:搜索方向自顶向下

    //获取图层链表
    const layer_list &getLayerList()const;
    const std::list<list_iterator> &getSelectedLayerIterList()const;
    //获取图层数据模型
    const QStandardItemModel *getLayerModel()const;
    //获取鼠标位置所在图层,若参数2为True,只获取被选中的图层，若参数3为True
    SObject *getTopLayerOn(const QPointF& pt, bool onlySelected = false, bool inBoundRect = false)const;
    //按位置（从下往上数）获取图层
    SObject *layerAt(size_t pos);

    //链表长度
    inline size_t layerListSize();
    inline size_t selectedListSize();

    //按对象类型获取图层

    //选择图层(函数会将鼠标位置的最顶层图层加入选择，若已加入则剔除选择)
    void clickSelect(const QPointF& pt, bool doMultiSelect = false);

    //清空选择
    void clearSelection();

    //选中图层上移
    /*  移动逻辑
     *   选中的所有图层将被移至连续，并置于原来选中
     *图层中最高层的上一层
     */
    void bringForward();

    //选中图层下移
    /*  移动逻辑
     *   选中的所有图层将被移至连续，并置于原来选中
     *图层中最低层的下一层
     */
    void sendBackward();

    //选中图层置顶
    /*  移动逻辑
     *   选中的所有图层将被移至连续，并置于所有图层
     *的最顶层
     */
    void bringToFront();

    //选中图层置底
    /*  移动逻辑
     *   选中的所有图层将被移至连续，并置于所有图层
     *的最底层
     */
    void sendToBack();


    //[功能函数]
private:
    //使用位置索引链表迭代器：O(n),尽量单次使用
    list_iterator _iterAt(size_t pos);
    //通过迭代器反求位置
    size_t _posOf(list_iterator it);
    //链表中的索引与模型中的行数互换
    inline size_t _posSwitch(size_t pos);
    //使用SObject*初始化图层数据项
    QList<QStandardItem *> _createRowItem(SObject* obj);
    //按图层顺序排序选择链表
    void _sortSelectList();
    //图层顺序调整辅助函数
    void _reOrderLayerList(list_iterator (*getInsertPos)(SLayerManager*));

};

size_t SLayerManager::layerListSize()
{
    return mLayerList.size();
}

size_t SLayerManager::selectedListSize()
{
    return mSelectedLayerIterList.size();
}

size_t SLayerManager::_posSwitch(size_t pos)
{
    Q_ASSERT(pos < mLayerList.size());
    return mLayerList.size() - pos - 1;
}

#endif // SLAYERMANAGER_H
