#ifndef QEXTRACTWIZARDPAGEINTRO_H
#define QEXTRACTWIZARDPAGEINTRO_H

#include <QWizardPage>

namespace Ui
{
class QExtractWizardPageIntro;
}

class QExtractWizardPageIntro : public QWizardPage
{
    Q_OBJECT
    /*-----构造函数与析构函数-----*/
public:
    explicit QExtractWizardPageIntro(QWidget *parent = nullptr);
    virtual ~QExtractWizardPageIntro();

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

    /*-----UI-----*/
private:
    Ui::QExtractWizardPageIntro *ui;
};

#endif // QEXTRACTWIZARDPAGEINTRO_H
