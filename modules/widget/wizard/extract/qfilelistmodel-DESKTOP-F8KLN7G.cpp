#include "qfilelistmodel.h"
#include <QMimeData>

QFileListModel::QFileListModel(QObject *parent):
    QStringListModel(parent)
{

}

QFileListModel::~QFileListModel()
{

}

bool QFileListModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    //data->formats()
    return false;
}

bool QFileListModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    return false;
}

Qt::DropActions QFileListModel::supportedDropActions() const
{
    return Qt::DropAction::CopyAction;
}
