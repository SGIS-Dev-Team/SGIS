#include "qextractwizardpagefile.h"
#include "ui_qextractwizardpagefile.h"

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
    return mFileListModel.rowCount() != 0;
}

void QExtractWizardPageFile::onFileSysViewItemDblClicked(const QModelIndex& index)
{
    ui->mFileTreeView->clearSelection();
    //将双击的文件导入到文件列表
    QFileInfo&& fileinfo = mFileSysModel.fileInfo(index);
    if (!SArchiveExtractor::isValidArchive(fileinfo))
        return;

    mFileListModel.append(fileinfo.filePath());
}

void QExtractWizardPageFile::_initialize()
{
    //初始化文件系统模型
    mFileSysModel.setRootPath(QDir::currentPath());
    mFileSysModel.setNameFilters(SArchiveExtractor::validArchiveNameFilters());

    //初始化视图
    ui->mArchiveListView->setModel(&mFileListModel);
    ui->mFileTreeView->setModel(&mFileSysModel);

    //显示支持的文件类型
    ui->mLabelSupportedTypes->setText("(" + SArchiveExtractor::validArchiveNameFilters().join(", ") + ")");

    //注册域
    registerField("extractList", this, "extractList", SIGNAL(extractListChanged));

    _initializeConnections();
}

void QExtractWizardPageFile::_initializeConnections()
{
    //文件系统视图双击
    connect(ui->mFileTreeView, &QTreeView::doubleClicked, this, &QExtractWizardPageFile::onFileSysViewItemDblClicked);
    //内容改变，链接三个信号
    connect(ui->mArchiveListView, &QFileListView::contentChanged, this, &QExtractWizardPageFile::extractListChanged);
    connect(ui->mArchiveListView, &QFileListView::contentChanged, this, &QExtractWizardPageFile::completeChanged);
}






