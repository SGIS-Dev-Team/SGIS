#include "qfilelistview.h"
#include <QAction>
#include <QMouseEvent>
#include <QMenu>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QMessageBox>
#include <QDesktopServices>
#include <QFileDialog>
#include "modules/algorithm/archive_extractor/sarchiveextractor.h"

QFileListView::QFileListView(QWidget* parent):
    QListView(parent)
{
    _initialize();
    this->setDragDropMode(QAbstractItemView::DragDropMode::DropOnly);
}

QFileListView::~QFileListView()
{

}

void QFileListView::mouseReleaseEvent(QMouseEvent* event)
{
    QListView::mouseReleaseEvent(event);
    Q_ASSERT(model());

    if (event->button() == Qt::RightButton && mbPopMenuEnabled)
    {
        //判断是否有选中对象
        mModelIndexOfPopMenu = this->indexAt(event->pos());
        bool isValidIdx = mModelIndexOfPopMenu.isValid();

        mpActionOpenInExplorer      ->setEnabled(isValidIdx);
        mpActionPreviewArchive      ->setEnabled(isValidIdx);
        mpActionRemove              ->setEnabled(isValidIdx);
        //调出菜单选项
        mpMenu->exec(event->globalPos());
    }
}

void QFileListView::mousePressEvent(QMouseEvent* event)
{
    QListView::mousePressEvent(event);
}

void QFileListView::dragEnterEvent(QDragEnterEvent* event)
{
    QListView::dragEnterEvent(event);
    //记录当前模型的行数
    if (model())
        mnModelRowCountWhenDragEntered = model()->rowCount();
}

void QFileListView::dropEvent(QDropEvent* event)
{
    QListView::dropEvent(event);
    //监控模型拖放结束是否改变
    if (model())
        if (mnModelRowCountWhenDragEntered != model()->rowCount())
            emit contentChanged();
    return;
}

void QFileListView::onActionOpenInExplorerTriggered()
{
    Q_ASSERT(model());
    onDoubleClicked(mModelIndexOfPopMenu);
    this->clearSelection();
}

void QFileListView::onActionPreviewArchiveTriggered()
{

}

void QFileListView::onActionRemoveTriggered()
{
    Q_ASSERT(model());
    QModelIndexList selectedList = this->selectedIndexes();
    for (auto iter = selectedList.rbegin(); iter != selectedList.rend(); ++iter)
        model()->removeRow((*iter).row());
    emit contentChanged();
}

void QFileListView::onActionAddArchiveTriggered()
{
    Q_ASSERT(model());
    QString strValidFilter = SArchiveExtractor::validArchiveNameFilters().join(" ");
    QStringList fileList = QFileDialog::getOpenFileNames(this, tr("Add Archives"), "", "archives (" + strValidFilter + ")");

    //添加到文件列表
    foreach (const auto& path, fileList)
    {
        model()->insertRow(model()->rowCount());
        model()->setData(model()->index(model()->rowCount() - 1, 0), QVariant(path));
    }

    if (fileList.size() > 0)
        emit contentChanged();
}

void QFileListView::onActionRemoveAllTriggered()
{
    Q_ASSERT(model());
    if (model()->rowCount() > 0)
    {
        model()->removeRows(0, model()->rowCount());
        emit contentChanged();
    }
}

void QFileListView::onDoubleClicked(const QModelIndex& index)
{
    Q_ASSERT(model());
    if (!index.isValid())
        return;
    //在资源管理器中打开目标文件
    const QString& imagePath = model()->data(index).toString();
    QDesktopServices::openUrl(QUrl(QFileInfo(imagePath).path()));
    //清空选择
    clearSelection();
}

const QAction* QFileListView::getAction(QFileListView::FileListViewMenuAction action)
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

void QFileListView::setPopMenuEnabled(bool enable)
{
    mbPopMenuEnabled = enable;
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

    mnModelRowCountWhenDragEntered = 0;
    mModelIndexOfPopMenu = QModelIndex();

    _initializeConnections();
}

void QFileListView::_initializeConnections()
{
    //菜单
    connect(this->mpActionOpenInExplorer, &QAction::triggered, this, &QFileListView::onActionOpenInExplorerTriggered);
    connect(this->mpActionAddArchive, &QAction::triggered, this, &QFileListView::onActionAddArchiveTriggered);
    connect(this->mpActionRemove, &QAction::triggered, this, &QFileListView::onActionRemoveTriggered);
    connect(this->mpActionRemoveAll, &QAction::triggered, this, &QFileListView::onActionRemoveAllTriggered);
    connect(this->mpActionPreviewArchive, &QAction::triggered, this, &QFileListView::onActionPreviewArchiveTriggered);

    //双击
    connect(this, &QFileListView::doubleClicked, this, &QFileListView::onDoubleClicked);
}








