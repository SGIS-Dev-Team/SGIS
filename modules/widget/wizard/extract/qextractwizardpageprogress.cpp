#include "qextractwizardpageprogress.h"
#include "ui_qextractwizardpageprogress.h"

QExtractWizardPageProgress::QExtractWizardPageProgress(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::QExtractWizardPageProgress)
{
    ui->setupUi(this);
}

QExtractWizardPageProgress::~QExtractWizardPageProgress()
{
    delete ui;
}

void QExtractWizardPageProgress::initializePage()
{

}

bool QExtractWizardPageProgress::isComplete() const
{
    return false;
}
