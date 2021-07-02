#ifndef QEXTRACTWIZARDPAGEPROGRESS_H
#define QEXTRACTWIZARDPAGEPROGRESS_H

#include <QWizardPage>
#include "qfilelistmodel.h"
#include "qfilelistview.h"
#include "modules/algorithm/archive_extractor/sarchivestreammeta.h"
#include "modules/algorithm/archive_extractor/sarchiveextractor.h"

namespace Ui
{
class QExtractWizardPageProgress;
}

class QExtractWizardPageProgress : public QWizardPage
{
    Q_OBJECT

    /*-----构造函数与析构函数-----*/
public:
    explicit QExtractWizardPageProgress(QWidget* parent = nullptr);
    virtual ~QExtractWizardPageProgress();

    /*-----虚函数重载-----*/
protected:

public:
    virtual void initializePage() override;
    virtual bool isComplete() const override;

    /*-----信号-----*/
signals:
    void startExtract(QThread* pTargetThread, const QString& strArchivePath, const QString& strExtractDir);

    /*-----槽函数-----*/
private slots:
    void onExtractComplete(QThread* pExtractThread, const QString& strExtractDir);
    void onExtractProgressUpdated(QThread* pExtractThread, int progress);

    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
protected:
    //Wizard指针
    QWizard* pWizard{nullptr};

    //解压数据流
    std::vector<SArchiveStreamMeta> mStreamMetaVec{};
    //文件列表模型
    QFileListModel mArchiveListModel{};
    //解压线程
    QThread* mpExtractThreads{nullptr};
    //解压器
    SArchiveExtractor* mpExtractors{nullptr};
    //当前正在处理的数据流序号映射表
    size_t* mpExtractingStreamIdx{nullptr};
    //已经解压完成的数量
    size_t muExtractedCount{0};
    //解压线程数量
    size_t muExtractThreadCount{DEFAULT_EXTRACT_THREAD_COUNT};

    /*-----成员函数-----*/
public:
    //[访问函数]

    //[修改函数]
    void setWizard(QWizard* wizard) {pWizard = wizard;}

    //[功能函数]
private:
    void _initialize();
    void _initializeConnections();

    /*-----UI-----*/
private:
    Ui::QExtractWizardPageProgress* ui;
};

#endif // QEXTRACTWIZARDPAGEPROGRESS_H
