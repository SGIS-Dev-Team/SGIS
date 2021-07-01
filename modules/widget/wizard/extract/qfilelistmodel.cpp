#include "qfilelistmodel.h"
#include <QMimeData>
#include <QFileInfo>
#include <QUrl>
#include <QMessageBox>
#include "modules/algorithm/sarchiveextractor.h"

QFileListModel::QFileListModel(QObject* parent) : QStringListModel(parent)
{
}

bool QFileListModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const
{
    if (data->hasFormat("text/uri-list"))
        return true;
    return false;
}

bool QFileListModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    QList<QUrl> urlList = data->urls();
    //在url列表里面寻找压缩档案
    bool hasArchive{false};
    foreach (const auto& url, urlList)
    {
        QFileInfo fileInfo(url.toLocalFile());
        //判断格式是否受支持
        if (SArchiveExtractor::isValidArchive(fileInfo))
        {
            hasArchive = true;
            //判断是否已存在
            bool exist = this->stringList().contains(fileInfo.filePath());
            //添加到模型中
            if (exist == false)
            {
                if (row == -1) row = this->rowCount();
                this->insertRow(row);
                this->setData(this->index(row), fileInfo.filePath());
            }
        }
    }
    if (!hasArchive)
        QMessageBox::information(nullptr, tr("Unsupported Archive"), tr("Please drag tarball archive files inside."));

    return hasArchive;
}

Qt::DropActions QFileListModel::supportedDropActions() const
{
    return Qt::DropAction::CopyAction;
}

void QFileListModel::append(const QString& string)
{
    int newRow = this->rowCount();
    this->insertRow(newRow);
    this->setData(this->index(newRow), string);
}
