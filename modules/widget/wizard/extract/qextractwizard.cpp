#include "qextractwizard.h"
#include "ui_qextractwizard.h"

QExtractWizard::QExtractWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::QExtractWizard)
{
    ui->setupUi(this);
}

QExtractWizard::~QExtractWizard()
{
    delete ui;
}
