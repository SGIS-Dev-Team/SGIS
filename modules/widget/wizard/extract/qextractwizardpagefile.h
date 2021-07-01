#ifndef QEXTRACTWIZARDPAGEFILE_H
#define QEXTRACTWIZARDPAGEFILE_H

#include <QWizardPage>
#include <QFileSystemModel>
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
    explicit QExtractWizardPageFile(QWidget* parent = nullptr);
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
    //文件系统视图双击事件响应
    void onFileSysViewItemDblClicked(const QModelIndex& index);

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
private:
    static QStringList strListValidFileSuffix;

    /*-----成员函数-----*/
public:
    //[访问函数]

    //[修改函数]
    void addArchive(const QFileInfo& fileinfo);
    void addArchive(const QString& path);
    void addArchives(const QStringList& pathList);
    void setArchives(const QStringList& pathList);
    void removeArchive(int idx);
    void removeArchives(QList<int> indices);
    void removeArchives(std::vector<int> indices);

    //[功能函数]
    //检查是否为支持的档案类型
    static bool isValidArchive(const QFileInfo& fileinfo);
    //获取支持档案类型的文件过滤器列表(如"*.gz","*.zip"等)
    static QStringList validArchiveNameFilters();

private:
    void _initialize();
    void _initializeConnections();

    /*-----UI-----*/
private:
    Ui::QExtractWizardPageFile* ui;
};

#endif // QEXTRACTWIZARDPAGEFILE_H
