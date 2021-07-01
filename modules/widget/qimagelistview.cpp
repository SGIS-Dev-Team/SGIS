#include "qimagelistview.h"
#include "QMouseEvent"

QImageListView::QImageListView(QWidget* parent): QListView(parent)
{
    _initialize();
}

QImageListView::~QImageListView()
{

}

void QImageListView::mouseReleaseEvent(QMouseEvent* event)
{
    QListView::mouseReleaseEvent(event);
    Q_ASSERT(model());

    if (event->button() == Qt::RightButton)
    {
        //判断是否有选中对象
        QModelIndex index = this->indexAt(event->pos());
        bool isValidIdx = index.isValid();
        mpActionOpenInExplorer      ->setEnabled(isValidIdx);
        mpActionRebuildOverviews    ->setEnabled(isValidIdx);
        mpActionShowMetaData        ->setEnabled(isValidIdx);
        mpActionRemove              ->setEnabled(isValidIdx);
        //调出菜单选项
        mpMenu->exec(event->globalPos());
    }
}

void QImageListView::mousePressEvent(QMouseEvent* event)
{
    if (event->modifiers() == Qt::NoModifier)
        if (event->button() == Qt::LeftButton)
            this->clearSelection();
    QListView::mousePressEvent(event);
}

const QAction* QImageListView::getAction(QImageListView::ImageListViewMenuAction action)
{
    switch (action)
    {
    case QImageListView::Add_Image:
        return mpActionAddImage;
        break;
    case QImageListView::Add_Archive:
        return mpActionAddArchive;
        break;
    case QImageListView::Remove:
        return mpActionRemove;
        break;
    case QImageListView::Remove_All:
        return mpActionRemoveAll;
        break;
    case QImageListView::Rebuild_Overviews:
        return mpActionRebuildOverviews;
        break;
    case QImageListView::Show_Meta_Data:
        return mpActionShowMetaData;
        break;
    case QImageListView::Open_In_Explorer:
        return mpActionOpenInExplorer;
        break;
    default:
        Q_ASSERT(0);
    }
}

void QImageListView::_initialize()
{
    //初始化右键菜单
    mpMenu                   = new QMenu(this);

    mpMenuAdd                = new QMenu(tr("Add..."),                  mpMenu);
    mpActionAddImage         = new QAction(tr("Add Image"),             mpMenuAdd);
    mpActionAddArchive       = new QAction(tr("Add Archive"),           mpMenuAdd);
    mpActionOpenInExplorer   = new QAction(tr("Open In Explorer..."),   mpMenu);
    mpActionRebuildOverviews = new QAction(tr("Rebuild Overviews"),     mpMenu);
    mpActionRemove           = new QAction(tr("Remove"),                mpMenu);
    mpActionRemoveAll        = new QAction(tr("Remove All"),            mpMenu);
    mpActionShowMetaData     = new QAction(tr("Show Meta Data..."),     mpMenu);

    mpMenu->addAction(mpActionOpenInExplorer);
    mpMenu->addAction(mpActionShowMetaData);
    mpMenu->addAction(mpActionRemove);
    mpMenu->addAction(mpActionRebuildOverviews);
    mpMenu->addSeparator();
    //子菜单
    mpMenuAdd->addAction(mpActionAddImage);
    mpMenuAdd->addAction(mpActionAddArchive);
    mpMenu->addMenu(mpMenuAdd);
    mpMenu->addAction(mpActionRemoveAll);
}

void QImageListView::_initializeConnections()
{
}
