#ifndef QEXTRACTWIZARDPAGETEMPDIR_H
#define QEXTRACTWIZARDPAGETEMPDIR_H

#include <QWizardPage>
#include <modules/global.h>
#include <QDir>

namespace Ui
{
class QExtractWizardPageTempDir;
}

class QExtractWizardPageTempDir : public QWizardPage
{
    Q_OBJECT
    /*-----构造函数与析构函数-----*/
public:
    explicit QExtractWizardPageTempDir(QWidget* parent = nullptr);
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
    //选择改变
    void onRadioButtonEmplaceToggled(bool isChecked);
    void onRadioButtonUseBelowToggled(bool isChecked);
    //“打开”按钮单击事件响应
    void onPushButtonOpenClicked();
    //文本框文本编辑事件响应
    void onTextEditTempDirTextChanged();

    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
protected:
    QDir mDefaultTempDir{};

    /*-----成员函数-----*/
public:
    //[访问函数]

    //[修改函数]

    //[功能函数]
private:
    //检查缓存目录是否合法
    bool _validateTempDir()const;

    void _initialize();
    void _initializeConnections();

    /*-----UI-----*/
private:
    Ui::QExtractWizardPageTempDir* ui;
};

#endif // QEXTRACTWIZARDPAGETEMPDIR_H
