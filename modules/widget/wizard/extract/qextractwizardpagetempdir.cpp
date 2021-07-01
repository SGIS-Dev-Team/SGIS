#include "qextractwizardpagetempdir.h"
#include "ui_qextractwizardpagetempdir.h"
#include <QDir>
#include <QFileDialog>

QExtractWizardPageTempDir::QExtractWizardPageTempDir(QWidget* parent) :
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
    _initialize();
}

bool QExtractWizardPageTempDir::isComplete() const
{
    if (ui->mRadioButtonEmplace->isChecked())
        return true;
    if (ui->mRadioButtonUseBelow->isChecked())
        return _validateTempDir();

    return false;
}

void QExtractWizardPageTempDir::onRadioButtonEmplaceToggled(bool isChecked)
{
    emit completeChanged();
}

void QExtractWizardPageTempDir::onRadioButtonUseBelowToggled(bool isChecked)
{
    ui->mButtonOpen->setEnabled(isChecked);
    ui->mTextEditTempDir->setEnabled(isChecked);

    emit completeChanged();
}

void QExtractWizardPageTempDir::onPushButtonOpenClicked()
{
    //如果没有默认缓存目录则创建
    if (!mDefaultTempDir.exists())
        mDefaultTempDir.mkpath(mDefaultTempDir.path());

    QString selectedPath = QFileDialog::getExistingDirectory(this, tr("Select a temporary directory for extraction"), mDefaultTempDir.path());
    if (!selectedPath.isEmpty())
    {
        ui->mTextEditTempDir->setPlainText(selectedPath);
        emit completeChanged();
    }
}

void QExtractWizardPageTempDir::onTextEditTempDirTextChanged()
{
    QString strInfoText{};
    QString strStyleString{};

    if (!ui->mTextEditTempDir->toPlainText().isEmpty())
    {
        if (!_validateTempDir())
        {
            strInfoText = tr("Directory is invalid.");
            strStyleString = "color:rgb(255,0,0)";
        }
        else
        {
            strInfoText = "";
            strStyleString = "color:rgb(0,0,0)";
        }
    }
    else
    {
        strInfoText = tr("Please select or enter the directory path.");
        strStyleString = "color:rgb(0,0,0)";
    }

    ui->mLabelDirInfo->setText(strInfoText);
    ui->mLabelDirInfo->setStyleSheet(strStyleString);

    emit completeChanged();
}

bool QExtractWizardPageTempDir::_validateTempDir() const
{
    //检查当前文本框内的路径是否合法
    QString tmpPath = ui->mTextEditTempDir->toPlainText();

    QDir tempDir(tmpPath);

    if (tempDir.exists() && tempDir.path() != ".")
        return true;
    else
        return false;
}

void QExtractWizardPageTempDir::_initialize()
{
    //初始化路径文本框
    mDefaultTempDir.setPath(SGIS_DOCUMENT_FOLDER + DEFAULT_EXTRACT_TEMP_DIR);
    ui->mTextEditTempDir->setPlainText(mDefaultTempDir.path());
    onTextEditTempDirTextChanged();

    ui->mRadioButtonEmplace->setChecked(true);

    //注册域
    registerField("extractEmplace", ui->mRadioButtonEmplace);
    registerField("extractInSpecifiedDir", ui->mRadioButtonUseBelow);
    registerField("tempDir", ui->mTextEditTempDir);

    _initializeConnections();
}

void QExtractWizardPageTempDir::_initializeConnections()
{
    //链接按钮事件响应
    connect(ui->mRadioButtonEmplace, &QRadioButton::toggled, this, &QExtractWizardPageTempDir::onRadioButtonEmplaceToggled);
    connect(ui->mRadioButtonUseBelow, &QRadioButton::toggled, this, &QExtractWizardPageTempDir::onRadioButtonUseBelowToggled);
    connect(ui->mButtonOpen, &QPushButton::clicked, this, &QExtractWizardPageTempDir::onPushButtonOpenClicked);
    //链接文本框事件响应
    connect(ui->mTextEditTempDir, &QTextEdit::textChanged, this, &QExtractWizardPageTempDir::onTextEditTempDirTextChanged);
}






