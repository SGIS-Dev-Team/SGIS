#ifndef QEXTRACTWIZARDPAGETEMPDIR_H
#define QEXTRACTWIZARDPAGETEMPDIR_H

#include <QWizardPage>

namespace Ui {
class QExtractWizardPageTempDir;
}

class QExtractWizardPageTempDir : public QWizardPage
{
    Q_OBJECT

public:
    explicit QExtractWizardPageTempDir(QWidget *parent = nullptr);
    ~QExtractWizardPageTempDir();

private:
    Ui::QExtractWizardPageTempDir *ui;
};

#endif // QEXTRACTWIZARDPAGETEMPDIR_H
