#include "qimagelistview.h"
#include "QMouseEvent"

QImageListView::QImageListView(QWidget *parent): QListView(parent)
{
    _initialize();
}

QImageListView::~QImageListView()
{

}

void QImageListView::mouseReleaseEvent(QMouseEvent *event)
{
    QListView::mouseReleaseEvent(event);

    if(event->button() == Qt::RightButton)
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

void QImageListView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        this->clearSelection();
    QListView::mousePressEvent(event);
}

const QAction *QImageListView::getAction(QImageListView::ImageListViewMenuAction action)
{
    switch (action)
    {
    case QImageListView::Add_Image:
        return mpActionAddImage.get();
        break;
    case QImageListView::Add_Tarball:
        return mpActionAddImage.get();
        break;
    case QImageListView::Remove:
        return mpActionRemove.get();
        break;
    case QImageListView::Remove_All:
        return mpActionRemoveAll.get();
        break;
    case QImageListView::Rebuild_Overviews:
        return mpActionRebuildOverviews.get();
        break;
    case QImageListView::Show_Meta_Data:
        return mpActionShowMetaData.get();
        break;
    case QImageListView::Open_In_Explorer:
        return mpActionOpenInExplorer.get();
        break;
    default:
        Q_ASSERT(0);
    }
}

void QImageListView::_initialize()
{
    //初始化右键菜单
    mpMenu                   .reset(new QMenu());

    mpMenuAdd                .reset(new QMenu(tr("Add..."),                  mpMenu.get()));
    mpActionAddImage         .reset(new QAction(tr("Add Image"),             mpMenuAdd.get()));
    mpActionAddTarball       .reset(new QAction(tr("Add Tarball"),           mpMenuAdd.get()));
    mpActionOpenInExplorer   .reset(new QAction(tr("Open In Explorer..."),   mpMenu.get()));
    mpActionRebuildOverviews .reset(new QAction(tr("Rebuild Overviews"),     mpMenu.get()));
    mpActionRemove           .reset(new QAction(tr("Remove"),                mpMenu.get()));
    mpActionRemoveAll        .reset(new QAction(tr("Remove All"),            mpMenu.get()));
    mpActionShowMetaData     .reset(new QAction(tr("Show Meta Data..."),     mpMenu.get()));

    mpMenu->addAction(mpActionOpenInExplorer     .get());
    mpMenu->addAction(mpActionShowMetaData       .get());
    mpMenu->addAction(mpActionRemove             .get());
    mpMenu->addAction(mpActionRebuildOverviews   .get());
    mpMenu->addSeparator();
    //子菜单
    mpMenuAdd->addAction(mpActionAddImage        .get());
    mpMenuAdd->addAction(mpActionAddTarball      .get());
    mpMenu->addMenu(mpMenuAdd                    .get());
    mpMenu->addAction(mpActionRemoveAll          .get());
}

void QImageListView::_initializeConnections()
{
}
