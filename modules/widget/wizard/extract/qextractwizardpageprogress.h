#ifndef QEXTRACTWIZARDPAGEPROGRESS_H
#define QEXTRACTWIZARDPAGEPROGRESS_H

#include <QWizardPage>

namespace Ui {
class QExtractWizardPageProgress;
}

class QExtractWizardPageProgress : public QWizardPage
{
    Q_OBJECT

public:
    explicit QExtractWizardPageProgress(QWidget *parent = nullptr);
    ~QExtractWizardPageProgress();

private:
    Ui::QExtractWizardPageProgress *ui;
};

#endif // QEXTRACTWIZARDPAGEPROGRESS_H
