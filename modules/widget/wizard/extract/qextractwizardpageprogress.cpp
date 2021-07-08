#include "qextractwizardpageprogress.h"
#include "ui_qextractwizardpageprogress.h"
#include <QThread>
#include <QFileInfo>

QExtractWizardPageProgress::QExtractWizardPageProgress(QWidget* parent) :
    QWizardPage(parent),
    ui(new Ui::QExtractWizardPageProgress)
{
    ui->setupUi(this);
}

QExtractWizardPageProgress::~QExtractWizardPageProgress()
{
    for (size_t i = 0; i < muExtractThreadCount; ++i)
    {
        mpExtractThreads[i].quit();
        mpExtractThreads[i].wait();
    }

    if (mpExtractThreads)delete[] mpExtractThreads;
    mpExtractThreads = nullptr;

    if (mpExtractors)delete[]mpExtractors;
    mpExtractors = nullptr;

    if (mpExtractingStreamIdx)delete[]mpExtractingStreamIdx;
    mpExtractingStreamIdx = nullptr;

    delete ui;
}

void QExtractWizardPageProgress::initializePage()
{
    _initialize();
}

bool QExtractWizardPageProgress::isComplete() const
{
    return muExtractedCount == mStreamMetaVec.size();
}

void QExtractWizardPageProgress::onExtractComplete(QThread* pExtractThread, const QString& strExtractDir)
{
    ++muExtractedCount;

    //TODO:这个设计是复杂冗余的，之后改掉
    //遍历线程表，确定是哪个线程完成了任务
    size_t threadIdx{0};
    while (pExtractThread != mpExtractThreads + threadIdx) ++threadIdx;

    //寻找未开始解压的档案数据流
    for (size_t i = 0; i < mStreamMetaVec.size(); ++i)
    {
        auto& streamMeta = mStreamMetaVec[i];

        if (streamMeta.extractProgress() == -1)
        {
            streamMeta.setExtractProgress(0);
            mpExtractingStreamIdx[threadIdx] = i;
            emit startExtract(pExtractThread, streamMeta.archivePath(), streamMeta.extractDir());
        }
    }

    ui->mLabelExtractStatus->setText(
        tr("Status: ") +
        QString::number(muExtractedCount) +
        "/" +
        QString::number(mStreamMetaVec.size()) +
        tr(" Extracted"));

    emit completeChanged();
}

void QExtractWizardPageProgress::onExtractProgressUpdated(QThread* pExtractThread, int progress)
{
    //遍历线程表，确定是哪个线程完成了任务
    size_t threadIdx{0};
    while (pExtractThread != mpExtractThreads + threadIdx) ++threadIdx;

    //更新对应列表项的文本
    mArchiveListModel.setData(
        mArchiveListModel.index(threadIdx),
        mStreamMetaVec[threadIdx].archivePath() + "  " + QString::number(progress) + "%");
}

void QExtractWizardPageProgress::_initialize()
{
    //将返回停用
    if (pWizard)
    {
        QAbstractButton* pBackButton = pWizard->button(QWizard::BackButton);
        if (pBackButton)
            pBackButton->setEnabled(false);
    }

    //获取文件选择页面的域
    QStringList strListArchivePaths = field("extractList")              .toStringList();

    //获取缓存文件夹页面的域
    bool extractEmplace             = field("extractEmplace")           .toBool();
    bool extractInSpecifiedDir      = field("extractInSpecifiedDir")    .toBool();
    QString tempDir                 = field("tempDir")                  .toString();

    //链接模型与视图
    mArchiveListModel.setStringList(strListArchivePaths);
    ui->mArchiveListView->setModel(&mArchiveListModel);
    //停用视图右键菜单（偷懒）
    ui->mArchiveListView->setPopMenuEnabled(false);

    //构建解压数据流
    mStreamMetaVec.reserve(strListArchivePaths.size());
    foreach (const auto& archivePath, strListArchivePaths)
    {
        QString extractDir = extractEmplace ? QFileInfo(archivePath).path() : tempDir;
        mStreamMetaVec.emplace_back(SArchiveMeta(), archivePath, extractDir, -1);
    }

    //状态标签
    ui->mLabelExtractStatus->setText(
        tr("Status: ") +
        QString::number(0) +
        "/" +
        QString::number(mStreamMetaVec.size()) +
        tr(" Extracted"));


    //初始化解压线程
    mpExtractThreads = new QThread[muExtractThreadCount];
    mpExtractors = new SArchiveExtractor[muExtractThreadCount];
    mpExtractingStreamIdx = new size_t[muExtractThreadCount];

    for (size_t i = 0; i < muExtractThreadCount; ++i)
    {
        mpExtractors[i].moveToThread(mpExtractThreads + i);
        connect(this, &QExtractWizardPageProgress::startExtract, mpExtractors + i, &SArchiveExtractor::doExtract);
        connect(mpExtractors + i, &SArchiveExtractor::extractComplete, this, &QExtractWizardPageProgress::onExtractComplete);
        connect(mpExtractors + i, &SArchiveExtractor::extractProgressUpdated, this, &QExtractWizardPageProgress::onExtractProgressUpdated);
        mpExtractThreads[i].start();
    }

    //启动解压线程
    for (size_t i = 0; i < muExtractThreadCount && i < mStreamMetaVec.size(); ++i)
    {
        mStreamMetaVec[i].setExtractProgress(0);
        emit startExtract(mpExtractThreads + i, mStreamMetaVec[i].archivePath(), mStreamMetaVec[i].extractDir());
    }

    _initializeConnections();
}

void QExtractWizardPageProgress::_initializeConnections()
{

}









