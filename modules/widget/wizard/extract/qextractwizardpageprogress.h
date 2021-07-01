#ifndef QEXTRACTWIZARDPAGEPROGRESS_H
#define QEXTRACTWIZARDPAGEPROGRESS_H

#include <QWizardPage>
#include "qfilelistmodel.h"
#include "qfilelistview.h"

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

    /*-----槽函数-----*/
private slots:

    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
protected:
    //文件列表模型
    QFileListModel mArchiveListModel;
    //解压线程
    QThread* mpExtractThreads{nullptr};
    //解压线程数量
    size_t muExtractThreadCount{DEFAULT_EXTRACT_THREAD_COUNT};

    /*-----成员函数-----*/
public:
    //[访问函数]

    //[修改函数]

    //[功能函数]
private:
    void _initialize();
    void _initializeConnections();

    /*-----UI-----*/
private:
    Ui::QExtractWizardPageProgress* ui;
};

#endif // QEXTRACTWIZARDPAGEPROGRESS_H
