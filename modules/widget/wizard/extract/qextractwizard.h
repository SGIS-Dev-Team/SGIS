#ifndef QEXTRACTWIZARD_H
#define QEXTRACTWIZARD_H

#include <QWizard>
#include <modules/widget/wizard/extract/qextractwizardpagefile.h>
#include <modules/widget/wizard/extract/qextractwizardpageintro.h>
#include <modules/widget/wizard/extract/qextractwizardpageprogress.h>
#include <modules/widget/wizard/extract/qextractwizardpagetempdir.h>

namespace Ui
{
class QExtractWizard;
}

class QExtractWizard : public QWizard
{
    Q_OBJECT
public:
    enum ExtractWizardPages
    {
        IntroPage,
        FilePage,
        TempDirPage,
        ProgressPage,
    };

    /*-----构造函数与析构函数-----*/
public:
    explicit QExtractWizard(QWidget* parent = nullptr);
    virtual ~QExtractWizard();

    /*-----虚函数重载-----*/
protected:

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

private:
    void _initialize();
    void _initializeConnections();

    /*-----UI-----*/
private:
    Ui::QExtractWizard* ui;
    QExtractWizardPageIntro*    mpPageIntro;
    QExtractWizardPageFile*     mpPageFile;
    QExtractWizardPageTempDir*  mpPageTempDir;
    QExtractWizardPageProgress* mpPageProgress;

};

#endif // QEXTRACTWIZARD_H
