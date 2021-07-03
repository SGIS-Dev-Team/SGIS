#include "qextractwizard.h"
#include "ui_qextractwizard.h"
#include "qextractwizardpagefile.h"

QExtractWizard::QExtractWizard(QWidget* parent) :
    QWizard(parent),
    ui(new Ui::QExtractWizard)
{
    ui->setupUi(this);
    _initialize();
}

QExtractWizard::~QExtractWizard()
{
    delete ui;
}

void QExtractWizard::_initialize()
{
    //创建页面
    mpPageIntro     = new QExtractWizardPageIntro();
    mpPageFile      = new QExtractWizardPageFile();
    mpPageTempDir   = new QExtractWizardPageTempDir();
    mpPageProgress  = new QExtractWizardPageProgress();
    mpPageProgress->setWizard(this);
    //添加页面
    addPage(mpPageIntro);
    addPage(mpPageFile);
    addPage(mpPageTempDir);
    addPage(mpPageProgress);
    //设置图像

}

void QExtractWizard::_initializeConnections()
{

}
