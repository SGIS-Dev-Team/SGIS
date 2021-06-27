#include "qextractwizardpagefile.h"
#include "ui_qextractwizardpagefile.h"

QExtractWizardPageFile::QExtractWizardPageFile(QWidget *parent) :
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

void QExtractWizardPageFile::onFileSysViewItemDblClicked(const QModelIndex & index)
{
    ui->mFileTreeView->clearSelection();
    //将双击的文件导入到文件列表
    QFileInfo &&fileinfo = mFileSysModel.fileInfo(index);
    if(!isValidArchive(fileinfo))
        return;
    addArchive(fileinfo);
}

void QExtractWizardPageFile::addArchive(const QFileInfo &fileinfo)
{
    addArchive(fileinfo.filePath());
}

void QExtractWizardPageFile::addArchive(const QString &path)
{
    addArchives(QStringList({path}));
}

void QExtractWizardPageFile::addArchives(const QStringList &pathList)
{
    mStrFileList.append(pathList);
    setArchives(mStrFileList);
}

void QExtractWizardPageFile::setArchives(const QStringList &pathList)
{
    mStrFileList = pathList;

    QStringList nameList;
    nameList.reserve(mStrFileList.size());

    for(auto &path : pathList)
        nameList << QFileInfo(path).fileName();

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
    for(auto &idx : indices)
        removeArchive(idx);
}

void QExtractWizardPageFile::removeArchives(std::vector<int> indices)
{
    std::sort(indices.rbegin(), indices.rend());
    for(auto &idx : indices)
        removeArchive(idx);
}

bool QExtractWizardPageFile::isValidArchive(const QFileInfo &fileinfo)
{
    //检查是否为目录
    if(fileinfo.isDir())
        return false;
    //检查后缀名
    QString suffix = fileinfo.suffix();
    bool valid{false};
    for(const auto &validSuffix : mStrListValidFileSuffix)
        if(suffix == validSuffix)
            valid = true;

    return valid;
}

void QExtractWizardPageFile::_initialize()
{
    //初始化视图
    mFileSysModel.setRootPath(QDir::currentPath());
    ui->mFileListView->setModel(&mFileListModel);
    ui->mFileTreeView->setModel(&mFileSysModel);

    _initializeConnections();
}

void QExtractWizardPageFile::_initializeConnections()
{
    connect(ui->mFileTreeView, &QFileTreeView::doubleClicked, this, &QExtractWizardPageFile::onFileSysViewItemDblClicked);
}
