#ifndef QEXTRACTWIZARDPAGEFILE_H
#define QEXTRACTWIZARDPAGEFILE_H

#include <QWizardPage>

namespace Ui {
class QExtractWizardPageFile;
}

class QExtractWizardPageFile : public QWizardPage
{
    Q_OBJECT

public:
    explicit QExtractWizardPageFile(QWidget *parent = nullptr);
    ~QExtractWizardPageFile();

private:
    Ui::QExtractWizardPageFile *ui;
};

#endif // QEXTRACTWIZARDPAGEFILE_H
