#ifndef QFILELISTVIEW_H
#define QFILELISTVIEW_H

#include <QListView>

class QFileListView : public QListView
{
    Q_OBJECT
public:
    enum FileListViewMenuAction
    {
        Open_In_Explorer,
        Preview_Archive,
        Remove,
        Add_Archive,
        Remove_All,
    };

    /*-----构造函数与析构函数-----*/
public:
    explicit QFileListView(QWidget* parent = nullptr);
    virtual ~QFileListView();

    /*-----虚函数重载-----*/
protected:
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;

    /*-----信号-----*/
signals:

    /*-----槽函数-----*/
private slots:
    //文件列表视图右键菜单响应
    void onActionOpenInExplorerTriggered();
    void onActionPreviewArchiveTriggered();
    void onActionRemoveTriggered();
    void onActionAddArchiveTriggered();
    void onActionRemoveAllTriggered();

    //文件列表视图双击事件响应
    void onDoubleClicked(const QModelIndex& index);
    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
protected:


    /*-----成员函数-----*/
public:
    //[访问函数]
    const QAction* getAction(FileListViewMenuAction action);

    //[修改函数]

    //[功能函数]
public:

private:
    void _initialize();
    void _initializeConnections();

    /*-----UI-----*/
private:
    QMenu* mpMenu;
    QAction* mpActionOpenInExplorer;
    QAction* mpActionPreviewArchive;
    QAction* mpActionRemove;
    QAction* mpActionAddArchive;
    QAction* mpActionRemoveAll;

};

#endif // QFILELISTVIEW_H
