#include "qextractwizardpagefile.h"
#include "ui_qextractwizardpagefile.h"

QExtractWizardPageFile::QExtractWizardPageFile(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::QExtractWizardPageFile)
{
    ui->setupUi(this);
}

QExtractWizardPageFile::~QExtractWizardPageFile()
{
    delete ui;
}
