#include "qextractwizardpageintro.h"
#include "ui_qextractwizardpageintro.h"

QExtractWizardPageIntro::QExtractWizardPageIntro(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::QExtractWizardPageIntro)
{
    ui->setupUi(this);
}

QExtractWizardPageIntro::~QExtractWizardPageIntro()
{
    delete ui;
}
