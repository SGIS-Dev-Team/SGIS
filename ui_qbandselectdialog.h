/********************************************************************************
** Form generated from reading UI file 'qbandselectdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QBANDSELECTDIALOG_H
#define UI_QBANDSELECTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_QBandSelectDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *mLabelPreviewImage;
    QFrame *mFrame;
    QVBoxLayout *verticalLayout;
    QLabel *mLabelStatus;
    QFrame *frame;
    QFormLayout *formLayout;
    QLabel *mLabelRed;
    QLabel *mLabelGreen;
    QComboBox *mComboGreen;
    QLabel *mLabelBlue;
    QComboBox *mComboBlue;
    QComboBox *mComboRed;
    QLabel *mLabelInfo;
    QProgressBar *mProgressBarBuildOverview;
    QDialogButtonBox *mButtonBox;

    void setupUi(QDialog *QBandSelectDialog)
    {
        if (QBandSelectDialog->objectName().isEmpty())
            QBandSelectDialog->setObjectName(QString::fromUtf8("QBandSelectDialog"));
        QBandSelectDialog->resize(791, 597);
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        QBandSelectDialog->setFont(font);
        horizontalLayout = new QHBoxLayout(QBandSelectDialog);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        mLabelPreviewImage = new QLabel(QBandSelectDialog);
        mLabelPreviewImage->setObjectName(QString::fromUtf8("mLabelPreviewImage"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mLabelPreviewImage->sizePolicy().hasHeightForWidth());
        mLabelPreviewImage->setSizePolicy(sizePolicy);
        mLabelPreviewImage->setMinimumSize(QSize(400, 400));
        mLabelPreviewImage->setScaledContents(true);

        horizontalLayout->addWidget(mLabelPreviewImage);

        mFrame = new QFrame(QBandSelectDialog);
        mFrame->setObjectName(QString::fromUtf8("mFrame"));
        mFrame->setFrameShape(QFrame::StyledPanel);
        mFrame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(mFrame);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        mLabelStatus = new QLabel(mFrame);
        mLabelStatus->setObjectName(QString::fromUtf8("mLabelStatus"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(mLabelStatus->sizePolicy().hasHeightForWidth());
        mLabelStatus->setSizePolicy(sizePolicy1);
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(11);
        font1.setBold(true);
        font1.setWeight(75);
        mLabelStatus->setFont(font1);
        mLabelStatus->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(mLabelStatus);

        frame = new QFrame(mFrame);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        formLayout = new QFormLayout(frame);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        mLabelRed = new QLabel(frame);
        mLabelRed->setObjectName(QString::fromUtf8("mLabelRed"));
        mLabelRed->setFont(font);

        formLayout->setWidget(2, QFormLayout::LabelRole, mLabelRed);

        mLabelGreen = new QLabel(frame);
        mLabelGreen->setObjectName(QString::fromUtf8("mLabelGreen"));
        mLabelGreen->setFont(font);

        formLayout->setWidget(3, QFormLayout::LabelRole, mLabelGreen);

        mComboGreen = new QComboBox(frame);
        mComboGreen->setObjectName(QString::fromUtf8("mComboGreen"));
        mComboGreen->setFont(font);

        formLayout->setWidget(3, QFormLayout::FieldRole, mComboGreen);

        mLabelBlue = new QLabel(frame);
        mLabelBlue->setObjectName(QString::fromUtf8("mLabelBlue"));
        mLabelBlue->setFont(font);

        formLayout->setWidget(4, QFormLayout::LabelRole, mLabelBlue);

        mComboBlue = new QComboBox(frame);
        mComboBlue->setObjectName(QString::fromUtf8("mComboBlue"));
        mComboBlue->setFont(font);

        formLayout->setWidget(4, QFormLayout::FieldRole, mComboBlue);

        mComboRed = new QComboBox(frame);
        mComboRed->setObjectName(QString::fromUtf8("mComboRed"));
        mComboRed->setFont(font);

        formLayout->setWidget(2, QFormLayout::FieldRole, mComboRed);

        mLabelInfo = new QLabel(frame);
        mLabelInfo->setObjectName(QString::fromUtf8("mLabelInfo"));
        mLabelInfo->setLayoutDirection(Qt::LeftToRight);
        mLabelInfo->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(0, QFormLayout::FieldRole, mLabelInfo);


        verticalLayout->addWidget(frame);

        mProgressBarBuildOverview = new QProgressBar(mFrame);
        mProgressBarBuildOverview->setObjectName(QString::fromUtf8("mProgressBarBuildOverview"));
        mProgressBarBuildOverview->setValue(0);

        verticalLayout->addWidget(mProgressBarBuildOverview);

        mButtonBox = new QDialogButtonBox(mFrame);
        mButtonBox->setObjectName(QString::fromUtf8("mButtonBox"));
        mButtonBox->setOrientation(Qt::Horizontal);
        mButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(mButtonBox);


        horizontalLayout->addWidget(mFrame);


        retranslateUi(QBandSelectDialog);
        QObject::connect(mButtonBox, SIGNAL(accepted()), QBandSelectDialog, SLOT(accept()));
        QObject::connect(mButtonBox, SIGNAL(rejected()), QBandSelectDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(QBandSelectDialog);
    } // setupUi

    void retranslateUi(QDialog *QBandSelectDialog)
    {
        QBandSelectDialog->setWindowTitle(QCoreApplication::translate("QBandSelectDialog", "Preview", nullptr));
        mLabelPreviewImage->setText(QString());
        mLabelStatus->setText(QCoreApplication::translate("QBandSelectDialog", "Choose three bands to preview", nullptr));
        mLabelRed->setText(QCoreApplication::translate("QBandSelectDialog", "R: ", nullptr));
        mLabelGreen->setText(QCoreApplication::translate("QBandSelectDialog", "G: ", nullptr));
        mLabelBlue->setText(QCoreApplication::translate("QBandSelectDialog", "B: ", nullptr));
        mLabelInfo->setText(QCoreApplication::translate("QBandSelectDialog", "Processing", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QBandSelectDialog: public Ui_QBandSelectDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QBANDSELECTDIALOG_H
