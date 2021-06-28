#include "qextractwizardpagetempdir.h"
#include "ui_qextractwizardpagetempdir.h"

QExtractWizardPageTempDir::QExtractWizardPageTempDir(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::QExtractWizardPageTempDir)
{
    ui->setupUi(this);
}

QExtractWizardPageTempDir::~QExtractWizardPageTempDir()
{
    delete ui;
}

void QExtractWizardPageTempDir::initializePage()
{

}

bool QExtractWizardPageTempDir::isComplete() const
{
    return false;
}
