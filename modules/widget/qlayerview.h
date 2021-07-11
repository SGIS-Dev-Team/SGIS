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
public:
    explicit QLayerView(QWidget* parent = nullptr, std::shared_ptr<SDocument> pCurrentDocument = nullptr);
    ~QLayerView();

    /*-----虚函数重载-----*/
public:
    virtual void mouseReleaseEvent(QMouseEvent* event)override;

    /*-----信号-----*/
signals:

    /*-----槽函数-----*/
private slots:
    void onLayerManagerSelectChanged();

    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
private:
    std::shared_ptr<SDocument> mpCurDoc;


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

    void _initialize();
    void _initializeContextMenu();
    void _initializeConnections();

    /*------UI------*/
private:
    QMenu*      mpContextMenu{nullptr};

    QMenu*      mpSubMenuAdd{nullptr};
    QAction*    mpActionAddRaster{nullptr};
    QAction*    mpActionAddShapeFile{nullptr};
    QAction*    mpActionAddArchive{nullptr};

    QAction*    mpActionCopy{nullptr};
    QAction*    mpActionCut{nullptr};
    QAction*    mpActionPaste{nullptr};

    QAction*    mpActionSwitchVisible{nullptr};
    QAction*    mpActionRemove{nullptr};

    QAction*    mpActionScaleToFit{nullptr};
    QAction*    mpActionSetBands{nullptr};
    QAction*    mpActionAttribute{nullptr};
};

#endif // QLAYERVIEW_H
