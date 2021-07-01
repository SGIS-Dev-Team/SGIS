#include "qfilelistmodel.h"
#include <QMimeData>
#include <QFileInfo>
#include <QUrl>
#include <QMessageBox>
#include "qextractwizardpagefile.h"

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
    //在url列表里面寻找tar.gz
    bool hasTarball{false};
    foreach (const auto& url, urlList)
    {
        QFileInfo fileInfo(url.toLocalFile());
        //判断格式是否受支持
        if (QExtractWizardPageFile::isValidArchive(fileInfo))
        {
            hasTarball = true;
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
    if (!hasTarball)
        QMessageBox::information(nullptr, tr("Unsupported Archive"), tr("Please drag tarball archive files inside."));

    return hasTarball;
}

Qt::DropActions QFileListModel::supportedDropActions() const
{
    return Qt::DropAction::CopyAction;
}
