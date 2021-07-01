#include "qextractwizardpagefile.h"
#include "ui_qextractwizardpagefile.h"

QStringList QExtractWizardPageFile::strListValidFileSuffix = {"gz", "zip"};

QExtractWizardPageFile::QExtractWizardPageFile(QWidget* parent) :
    QWizardPage(parent),
    ui(new Ui::QExtractWizardPageFile)
{
    ui->setupUi(this);
}

QExtractWizardPageFile::~QExtractWizardPageFile()
{
    delete ui;
}

void QExtractWizardPageFile::initializePage()
{
    _initialize();
}

bool QExtractWizardPageFile::isComplete() const
{
    return false;
}

void QExtractWizardPageFile::onFileSysViewItemDblClicked(const QModelIndex& index)
{
    ui->mFileTreeView->clearSelection();
    //将双击的文件导入到文件列表
    QFileInfo&& fileinfo = mFileSysModel.fileInfo(index);
    if (!isValidArchive(fileinfo))
        return;
    addArchive(fileinfo);
}

void QExtractWizardPageFile::addArchive(const QFileInfo& fileinfo)
{
    addArchive(fileinfo.filePath());
}

void QExtractWizardPageFile::addArchive(const QString& path)
{
    addArchives(QStringList(path));
}

void QExtractWizardPageFile::addArchives(const QStringList& pathList)
{
    mStrFileList.append(pathList);
    setArchives(mStrFileList);
}

void QExtractWizardPageFile::setArchives(const QStringList& pathList)
{
    mStrFileList = pathList;

    QStringList nameList;
    nameList.reserve(mStrFileList.size());

    for (auto& path : pathList)
        nameList << QFileInfo(path).filePath();

    mFileListModel.setStringList(nameList);
}

void QExtractWizardPageFile::removeArchive(int idx)
{
    Q_ASSERT(idx < mStrFileList.size() && idx >= 0);
    mFileListModel.removeRow(idx);
    mStrFileList.erase(mStrFileList.begin() + idx);
}

void QExtractWizardPageFile::removeArchives(QList<int> indices)
{
    std::sort(indices.rbegin(), indices.rend());
    for (auto& idx : indices)
        removeArchive(idx);
}

void QExtractWizardPageFile::removeArchives(std::vector<int> indices)
{
    std::sort(indices.rbegin(), indices.rend());
    for (auto& idx : indices)
        removeArchive(idx);
}

bool QExtractWizardPageFile::isValidArchive(const QFileInfo& fileinfo)
{
    //检查是否为目录
    if (fileinfo.isDir())
        return false;
    //检查后缀名
    QString suffix = fileinfo.suffix();
    bool valid{false};
    foreach (const auto& validSuffix, strListValidFileSuffix)
        if (suffix.compare(validSuffix, Qt::CaseSensitivity::CaseInsensitive) == 0)
            valid = true;

    return valid;
}

QStringList QExtractWizardPageFile::validArchiveNameFilters()
{
    QStringList filterList;
    for (auto& suffix : strListValidFileSuffix)
        filterList.append("*." + suffix);

    return filterList;
}

void QExtractWizardPageFile::_initialize()
{
    //初始化文件系统模型
    mFileSysModel.setRootPath(QDir::currentPath());
    mFileSysModel.setNameFilters(validArchiveNameFilters());

    //初始化视图
    ui->mFileListView->setModel(&mFileListModel);
    ui->mFileTreeView->setModel(&mFileSysModel);

    //显示支持的文件类型
    ui->mLabelSupportedTypes->setText("(" + validArchiveNameFilters().join(", ") + ")");

    _initializeConnections();
}

void QExtractWizardPageFile::_initializeConnections()
{
    //文件系统视图双击
    connect(ui->mFileTreeView, &QTreeView::doubleClicked, this, &QExtractWizardPageFile::onFileSysViewItemDblClicked);
}






