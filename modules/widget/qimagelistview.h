#ifndef QIMAGELISTVIEW_H
#define QIMAGELISTVIEW_H

#include <QListView>
#include <QMenu>
#include <QAction>

class QImageListView : public QListView
{
    Q_OBJECT
public:
    enum ImageListViewMenuAction
    {
        Add_Image,
        Add_Archive,
        Remove,
        Remove_All,
        Rebuild_Overviews,
        Show_Meta_Data,
        Open_In_Explorer
    };

    /*-----构造函数与析构函数-----*/
public:
    explicit QImageListView(QWidget* parent = nullptr);
    virtual ~QImageListView();

    /*-----虚函数重载-----*/
protected:
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;

    /*-----信号-----*/
signals:

    /*-----槽函数-----*/
private slots:

    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
protected:


    /*-----成员函数-----*/
public:
    //[访问函数]
    const QAction* getAction(ImageListViewMenuAction action);

    //[修改函数]

    //[功能函数]
public:

private:
    void _initialize();
    void _initializeConnections();

    /*-----UI-----*/
private:
    QMenu* mpMenu;
    QMenu* mpMenuAdd;
    QAction* mpActionAddImage;
    QAction* mpActionAddArchive;
    QAction* mpActionRemove;
    QAction* mpActionRebuildOverviews;
    QAction* mpActionShowMetaData;
    QAction* mpActionRemoveAll;
    QAction* mpActionOpenInExplorer;

};

#endif // QIMAGELISTVIEW_H
