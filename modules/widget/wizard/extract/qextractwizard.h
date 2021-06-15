#ifndef QEXTRACTWIZARD_H
#define QEXTRACTWIZARD_H

#include <QWizard>

namespace Ui {
class QExtractWizard;
}

class QExtractWizard : public QWizard
{
    Q_OBJECT

public:
    explicit QExtractWizard(QWidget *parent = nullptr);
    ~QExtractWizard();

private:
    Ui::QExtractWizard *ui;
};

#endif // QEXTRACTWIZARD_H
