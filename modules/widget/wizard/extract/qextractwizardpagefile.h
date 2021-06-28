#ifndef QEXTRACTWIZARDPAGEFILE_H
#define QEXTRACTWIZARDPAGEFILE_H

#include <QWizardPage>
#include <QFileSystemModel>
#include <QStringListModel>
#include "qfilelistmodel.h"

namespace Ui
{
class QExtractWizardPageFile;
}

class QExtractWizardPageFile : public QWizardPage
{
    Q_OBJECT
    /*-----构造函数与析构函数-----*/
public:
    explicit QExtractWizardPageFile(QWidget *parent = nullptr);
    virtual ~QExtractWizardPageFile();

    /*-----虚函数重载-----*/
protected:

public:
    virtual void initializePage() override;
    virtual bool isComplete() const override;
    /*-----信号-----*/
signals:

    /*-----槽函数-----*/
private slots:
    void onFileSysViewItemDblClicked(const QModelIndex & index);

    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
protected:
    //文件系统模型
    QFileSystemModel mFileSysModel;
    //文件列表模型
    QFileListModel mFileListModel;
    //文件列表
    QStringList mStrFileList;
    //支持的档案后缀
    QStringList mStrListValidFileSuffix{"zip", "gz"};

    /*-----成员函数-----*/
public:
    //[访问函数]

    //[修改函数]
    void addArchive(const QFileInfo &fileinfo);
    void addArchive(const QString &path);
    void addArchives(const QStringList &pathList);
    void setArchives(const QStringList &pathList);
    void removeArchive(int idx);
    void removeArchives(QList<int> indices);
    void removeArchives(std::vector<int> indices);

    //[功能函数]
    bool isValidArchive(const QFileInfo &fileinfo);

private:
    void _initialize();
    void _initializeConnections();

    /*-----UI-----*/
private:
    Ui::QExtractWizardPageFile *ui;
};

#endif // QEXTRACTWIZARDPAGEFILE_H
