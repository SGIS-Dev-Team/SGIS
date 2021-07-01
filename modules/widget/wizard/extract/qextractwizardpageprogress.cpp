#include "qextractwizardpageprogress.h"
#include "ui_qextractwizardpageprogress.h"
#include <QThread>

QExtractWizardPageProgress::QExtractWizardPageProgress(QWidget* parent) :
    QWizardPage(parent),
    ui(new Ui::QExtractWizardPageProgress)
{
    ui->setupUi(this);
}

QExtractWizardPageProgress::~QExtractWizardPageProgress()
{
    delete ui;
}

void QExtractWizardPageProgress::initializePage()
{
    _initialize();
}

bool QExtractWizardPageProgress::isComplete() const
{
    return false;
}

void QExtractWizardPageProgress::_initialize()
{
    //获取文件选择页面的域
    QStringList strListArchivePaths = field("extractList")              .toStringList();

    //获取缓存文件夹页面的域
    bool extractEmplace             = field("extractEmplace")           .toBool();
    bool extractInSpecifiedDir      = field("extractInSpecifiedDir")    .toBool();
    QString tempDir                 = field("tempDir")                  .toString();

    //链接模型与视图
    mArchiveListModel.setStringList(strListArchivePaths);
    ui->mArchiveListView->setModel(&mArchiveListModel);

    //初始化解压线程
    mpExtractThreads = new QThread[muExtractThreadCount];
    for (size_t i = 0; i < strListArchivePaths.size(); ++i)
    {

    }

    _initializeConnections();
}

void QExtractWizardPageProgress::_initializeConnections()
{

}
