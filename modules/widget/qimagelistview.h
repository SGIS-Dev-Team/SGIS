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
        Add_Tarball,
        Remove,
        Remove_All,
        Rebuild_Overviews,
        Show_Meta_Data,
        Open_In_Explorer

    };

    /*-----构造函数与析构函数-----*/
public:
    explicit QImageListView(QWidget *parent = nullptr);
    ~QImageListView();

    /*-----虚函数重载-----*/
protected:
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

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
    const QAction *getAction(ImageListViewMenuAction action);

    //[修改函数]

    //[功能函数]
public:

private:
    void _initialize();
    void _initializeConnections();

    /*-----UI-----*/
private:
    std::unique_ptr<QMenu> mpMenu;
    std::unique_ptr<QMenu> mpMenuAdd;
    std::unique_ptr<QAction> mpActionAddImage;
    std::unique_ptr<QAction> mpActionAddTarball;
    std::unique_ptr<QAction> mpActionRemove;
    std::unique_ptr<QAction> mpActionRebuildOverviews;
    std::unique_ptr<QAction> mpActionShowMetaData;
    std::unique_ptr<QAction> mpActionRemoveAll;
    std::unique_ptr<QAction> mpActionOpenInExplorer;

};

#endif // QIMAGELISTVIEW_H
