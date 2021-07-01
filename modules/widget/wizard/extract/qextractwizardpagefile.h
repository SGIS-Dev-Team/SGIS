#ifndef QEXTRACTWIZARDPAGEFILE_H
#define QEXTRACTWIZARDPAGEFILE_H

#include <QWizardPage>
#include <QFileSystemModel>
#include "qfilelistmodel.h"
#include "modules/algorithm/sarchiveextractor.h"

namespace Ui
{
class QExtractWizardPageFile;
}

class QExtractWizardPageFile : public QWizardPage
{
    Q_OBJECT
    /*-----构造函数与析构函数-----*/
public:
    explicit QExtractWizardPageFile(QWidget* parent = nullptr);
    virtual ~QExtractWizardPageFile();

    /*-----虚函数重载-----*/
protected:

public:
    virtual void initializePage() override;
    virtual bool isComplete() const override;

    /*-----信号-----*/
signals:
    //文件列表改变
    void extractListChanged();

    /*-----槽函数-----*/
private slots:
    //文件系统视图双击事件响应
    void onFileSysViewItemDblClicked(const QModelIndex& index);

    /*-----属性-----*/
protected:
    Q_PROPERTY(QStringList extractList READ extractList NOTIFY extractListChanged);

    /*-----成员变量-----*/
protected:
    //文件系统模型
    QFileSystemModel mFileSysModel;
    //文件列表模型
    QFileListModel mFileListModel;

private:

    /*-----成员函数-----*/
public:
    //[访问函数]
    QStringList extractList()const {return mFileListModel.stringList();}

    //[功能函数]


private:
    void _initialize();
    void _initializeConnections();

    /*-----UI-----*/
private:
    Ui::QExtractWizardPageFile* ui;
};

#endif // QEXTRACTWIZARDPAGEFILE_H
