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
//  存储在链表中的图层顺序为        底层到顶层   链表在前的结点为低层
//  显示在图层视图中的图层顺序为     顶层到底层  行索引值靠前的图层为高层
//
//  操作函数pos参数均为链表中的位置
//------------------------------------------

typedef std::list<SObject*>::iterator list_iterator;
typedef std::list<SObject*>::const_iterator list_const_iterator ;
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
    //这个信号指导画布重绘
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

    //信号抑制器
    bool mbUpdateSignal{true};

    /*-----成员函数-----*/

    //[访问与修改函数]
public:
    //添加图层在顶部
    void addLayer(SObject* obj);
    void addLayers(const std::vector<SObject*>& objVec);

    //插入图层
    void insertLayer(SObject* obj, int pos);
    void insertLayer(SObject* obj, list_iterator it);
    void insertLayers(const std::vector<SObject*>& objVec, int pos);
    void insertLayers(const std::vector<SObject*>& objVec, list_iterator it);

    //更换图层
    void replaceLayer(int pos, SObject* newLayer);
    void replaceLayer(SObject* oldLayer, SObject* newLayer);
    void replaceLayer(list_iterator it, SObject* newLayer);

    //删除图层
    void removeSelectedLayers();
    void removeLayer(SObject* obj);
    void removeLayer(int pos);
    void removeLayer(list_iterator it);
    void removeLayers(std::vector<SObject*> objVec);
    void removeLayers(std::vector<int> posVec);
    void removeLayers(std::vector<list_iterator> iterVec);

    //移动图层src处的图层将被插入到dst的图层之前
    //dstObj == nullptr时将srcObj移动到末位
    void moveLayer(SObject* srcObj, SObject* dstObj);
    void moveLayer(int srcPos, int dstPos);
    void moveLayer(const list_iterator& srcIter, const list_iterator& dstIter);
    void moveLayers(const std::vector<SObject*>& srcObjVec, SObject* dstObj);
    void moveLayers(const std::vector<int>& srcPosVec, int dstPos);
    void moveLayers(const std::vector<list_iterator>& srcIterVec, const list_iterator& dstIter);

    //下标运算符
    const SObject* operator[](int pos)const;
    SObject* operator[](int pos);

    //搜索图层:搜索方向自顶向下

    //获取图层链表
    const layer_list& getLayerList()const;
    const std::list<list_iterator>& getSelectedLayerIterList()const;

    //获取图层选择链表对应的模型行号
    std::vector<int> getSelectedRowIndices()const;

    //获取图层数据模型
    const QStandardItemModel* getLayerModel()const {return &mLayerModel;}
    QStandardItemModel* getLayerModel() {return &mLayerModel;}

    //获取鼠标位置所在图层,若参数2为True,只获取被选中的图层，若参数3为True，则判断是鼠标点是否在包围矩形内
    SObject* getTopLayerOn(const QPointF& pt, bool onlySelected = false, bool inBoundRect = false)const;

    //按位置（从下往上数）获取图层
    SObject* layerAt(size_t pos);

    //链表长度
    inline size_t layerListSize();
    inline size_t selectedListSize();

    //按对象类型获取图层

    //选择图层：在画布上选择(函数会将鼠标位置的最顶层图层加入选择，若已加入则剔除选择)
    void clickSelect(const QPointF& pt, bool doMultiSelect = false);

    //选择图层：用于在图层管理器上选择，这里使用的index是图层模型的行序号
    void select(const std::vector<int>& indexVec);
    void select(int index);

    //清空选择
    void clearSelection();

    /* 选中图层上移
     *   选中的所有图层将被移至连续，并置于原来选中图层中最高层的上一层
     */
    void bringForward();

    /* 选中图层下移
     *   选中的所有图层将被移至连续，并置于原来选中图层中最低层的下一层
     */
    void sendBackward();

    /* 选中图层置顶
     *   选中的所有图层将被移至连续，并置于所有图层的最顶层
     */
    void bringToFront();

    /* 选中图层置底
     *   选中的所有图层将被移至连续，并置于所有图层的最底层
     */
    void sendToBack();

    //设置选中图层的可见性
    void setVisible(bool visible);

    //获取选中图层的总包围矩形，画布逻辑坐标
    QRectF getBoundingRectOfSelected()const;


    //[功能函数]
private:
    //使用位置索引链表迭代器：O(n),尽量单次使用
    list_iterator _iterAt(int pos);
    list_const_iterator _iterAt(int pos)const;

    //通过迭代器反求位置
    int _posOf(list_iterator it)const;
    //链表中的索引与模型中的行数互换
    inline int _posSwitch(int pos)const;
    //使用SObject*初始化图层数据项
    QList<QStandardItem*> _createRowItem(SObject* obj);
    //按图层顺序排序选择链表
    void _sortSelectList();
    //图层顺序调整辅助函数
    void _reOrderLayerList(list_iterator(*getInsertPos)(SLayerManager*));

    //转换函数
    std::vector<list_iterator> _objVecToIterVec(const std::vector<SObject*>& objVec);
    std::vector<list_iterator> _posVecToIterVec(const std::vector<int>& posVec);

    //用于调整信号发射
    void _beginModifyLayers() {mbUpdateSignal = false;}
    void _endModifyLayers() {mbUpdateSignal = true; emit layersUpdated(this); emit selectStateChanged();}
};

size_t SLayerManager::layerListSize()
{
    return mLayerList.size();
}

size_t SLayerManager::selectedListSize()
{
    return mSelectedLayerIterList.size();
}

int SLayerManager::_posSwitch(int pos)const
{
    Q_ASSERT(pos < mLayerList.size() && pos >= 0);
    return mLayerList.size() - 1 - pos;
}

#endif // SLAYERMANAGER_H
