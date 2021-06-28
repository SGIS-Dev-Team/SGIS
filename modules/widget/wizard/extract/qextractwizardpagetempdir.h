#ifndef QEXTRACTWIZARDPAGETEMPDIR_H
#define QEXTRACTWIZARDPAGETEMPDIR_H

#include <QWizardPage>

namespace Ui
{
class QExtractWizardPageTempDir;
}

class QExtractWizardPageTempDir : public QWizardPage
{
    Q_OBJECT
    /*-----构造函数与析构函数-----*/
public:
    explicit QExtractWizardPageTempDir(QWidget *parent = nullptr);
    virtual ~QExtractWizardPageTempDir();

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

    /*-----成员函数-----*/
public:
    //[访问函数]

    //[修改函数]

    //[功能函数]

    /*-----UI-----*/
private:
    Ui::QExtractWizardPageTempDir *ui;
};

#endif // QEXTRACTWIZARDPAGETEMPDIR_H
