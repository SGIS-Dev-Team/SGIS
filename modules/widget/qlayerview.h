#ifndef QLAYERVIEW_H
#define QLAYERVIEW_H

//------------------------------
//      QLayerView
//      图层视图类
//      继承自QTreeView
//-----------------------------

#include <modules/global.h>
#include <QTreeView>
#include <modules/doc/sdocument.h>
#include <QStandardItemModel>

class QLayerView : public QTreeView
{
    Q_OBJECT
    /*-----构造函数与析构函数-----*/

public:
    explicit QLayerView(QWidget* parent = nullptr);
    explicit QLayerView(QWidget* parent, std::shared_ptr<SDocument> pCurrentDocument);
    virtual ~QLayerView();

    /*-----虚函数重载-----*/
public:
    virtual void mouseReleaseEvent(QMouseEvent* event)override;

    /*-----信号-----*/
signals:
    void importRaster();
    void importShapeFile();
    void fitViewToArea(const QRectF& rect);

    /*-----槽函数-----*/
private slots:
    //图层管理器选择改变
    void onLayerManagerSelectChanged();
    //菜单选项事件响应
    void onActionAddRasterTriggered();
    void onActionAddShapeFileTriggered();
    void onActionCopyTriggered();
    void onActionCutTriggered();
    void onActionPasteTriggered();
    void onActionSwitchVisibleTriggered();
    void onActionRemoveTriggered();
    void onActionScaleToFitTriggered();
    void onActionAttributeTriggered();


    virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)override;

    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
private:
    //当前文档
    std::shared_ptr<SDocument> mpCurDoc;
    //鼠标右键所在的图层
    QModelIndex mContextIndex{};
    //剪贴板
    QModelIndexList mLayerClipList{};
    //剪贴板状态
    enum LayerClipStatus
    {
        None,
        Copied,
        Cut,
    } mLayerClipStatus{None};

    /*-----成员函数-----*/
public:
    //[访问函数]
    std::shared_ptr<SDocument> getDocument();

    //[修改函数]
    void setDocument(std::shared_ptr<SDocument> pCurrentDocument);

    //[功能函数]
private:
    //使用当前文档更新图层数据模型
    void _updateLayerModel();

    /*
     * 为选中的栅格设置波段,which代表RGB中的一个
     * which = 0    设置Red
     * which = 1    设置Green
     * which = 2    设置Blue
     */
    void _setBand(int which, int band);

    void _initialize();
    void _initializeContextMenu();
    void _initializeConnections();
    void _connectDocument();
    void _disconnectDocument();

    /*------UI------*/
private:
    QMenu*      mpContextMenu{nullptr};

    QMenu*      mpSubMenuAdd{nullptr};
    QAction*    mpActionAddRaster{nullptr};
    QAction*    mpActionAddShapeFile{nullptr};

    QAction*    mpActionCopy{nullptr};
    QAction*    mpActionCut{nullptr};
    QAction*    mpActionPaste{nullptr};

    QAction*    mpActionSwitchVisible{nullptr};
    QAction*    mpActionRemove{nullptr};

    QAction*    mpActionScaleToFit{nullptr};

    QMenu*      mpSubMenuSetRedBand  {nullptr};
    QMenu*      mpSubMenuSetGreenBand{nullptr};
    QMenu*      mpSubMenuSetBlueBand {nullptr};

    QAction*    mpActionAttribute{nullptr};
};

#endif // QLAYERVIEW_H
