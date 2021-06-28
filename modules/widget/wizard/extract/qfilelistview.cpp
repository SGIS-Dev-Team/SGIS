#include "qfilelistview.h"
#include <QAction>
#include <QMouseEvent>
#include <QMenu>

QFileListView::QFileListView(QWidget *parent):
    QListView(parent)
{
    _initialize();
}

QFileListView::~QFileListView()
{

}

void QFileListView::mouseReleaseEvent(QMouseEvent *event)
{
    QListView::mouseReleaseEvent(event);
    Q_ASSERT(model());

    if(event->button() == Qt::RightButton)
    {
        //判断是否有选中对象
        QModelIndex index = this->indexAt(event->pos());
        bool isValidIdx = index.isValid();
        mpActionOpenInExplorer      ->setEnabled(isValidIdx);
        mpActionPreviewArchive      ->setEnabled(isValidIdx);
        mpActionRemove              ->setEnabled(isValidIdx);
        //调出菜单选项
        mpMenu->exec(event->globalPos());
    }
}

void QFileListView::mousePressEvent(QMouseEvent *event)
{
    QListView::mousePressEvent(event);
}

const QAction *QFileListView::getAction(QFileListView::FileListViewMenuAction action)
{
    switch (action)
    {
    case QFileListView::Add_Archive:
        return mpActionAddArchive;
        break;
    case QFileListView::Preview_Archive:
        return mpActionPreviewArchive;
        break;
    case QFileListView::Remove:
        return mpActionRemove;
        break;
    case QFileListView::Remove_All:
        return mpActionRemoveAll;
        break;
    case QFileListView::Open_In_Explorer:
        return mpActionOpenInExplorer;
        break;
    default:
        Q_ASSERT(0);
    }
}

void QFileListView::_initialize()
{
    //初始化右键菜单
    mpMenu                   = new QMenu(this);

    mpActionAddArchive       = new QAction(tr("Add Archive"),             mpMenu);
    mpActionOpenInExplorer   = new QAction(tr("Open In Explorer..."),   mpMenu);
    mpActionRemove           = new QAction(tr("Remove"),                mpMenu);
    mpActionRemoveAll        = new QAction(tr("Remove All"),            mpMenu);
    mpActionPreviewArchive   = new QAction(tr("Preview Archive"),       mpMenu);

    mpMenu->addAction(mpActionOpenInExplorer);
    mpMenu->addAction(mpActionPreviewArchive);
    mpMenu->addAction(mpActionRemove);
    mpMenu->addSeparator();
    mpMenu->addAction(mpActionAddArchive);
    mpMenu->addAction(mpActionRemoveAll);
    //设置选择模式
    this->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

    _initializeConnections();
}

void QFileListView::_initializeConnections()
{
}
