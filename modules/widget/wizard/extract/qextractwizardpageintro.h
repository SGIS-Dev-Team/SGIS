#ifndef QEXTRACTWIZARDPAGEINTRO_H
#define QEXTRACTWIZARDPAGEINTRO_H

#include <QWizardPage>

namespace Ui {
class QExtractWizardPageIntro;
}

class QExtractWizardPageIntro : public QWizardPage
{
    Q_OBJECT

public:
    explicit QExtractWizardPageIntro(QWidget *parent = nullptr);
    ~QExtractWizardPageIntro();

private:
    Ui::QExtractWizardPageIntro *ui;
};

#endif // QEXTRACTWIZARDPAGEINTRO_H
