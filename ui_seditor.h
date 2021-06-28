/********************************************************************************
** Form generated from reading UI file 'seditor.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEDITOR_H
#define UI_SEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "qlayerview.h"

QT_BEGIN_NAMESPACE

class Ui_SEditor
{
public:
    QAction *mActionNewTemplate;
    QAction *mActionOpenTemplate;
    QAction *mActionCloseTemplate;
    QAction *mActionSave;
    QAction *mActionSaveAs;
    QAction *mActionExport;
    QAction *mActionRelease;
    QAction *mActionExitEditor;
    QAction *mActionUndo;
    QAction *mActionRedo;
    QAction *mActionCopy;
    QAction *mActionCut;
    QAction *mActionDelete;
    QAction *mActionSelect;
    QAction *mActionMutiSelect;
    QAction *mActionRotate90Clockwise;
    QAction *mActionRotate90Anticlockwise;
    QAction *mActionRotate;
    QAction *mActionFlipHorizentally;
    QAction *mActionFlipVertically;
    QAction *mActionScale;
    QAction *mActionTranslate;
    QAction *mActionSelectTool;
    QAction *mActionCombineTool;
    QAction *mActionBrush;
    QAction *mActionEraser;
    QAction *mActionPen;
    QAction *mActionText;
    QAction *mActionAnotator;
    QAction *mActionBringForward;
    QAction *mActionSendBackward;
    QAction *mActionBringtoFront;
    QAction *mActionSendtoBack;
    QAction *mActionAlignLeft;
    QAction *mActionAlignMiddle;
    QAction *mActionAlignRight;
    QAction *mActionAlignTop;
    QAction *mActionAlignCenter;
    QAction *mActionAlignBottom;
    QAction *mActionShowLayer;
    QAction *mActionHideLayer;
    QAction *mActionDistributeHorizentally;
    QAction *mActionDistributeVertically;
    QAction *mActionShowAll;
    QAction *mActionHideAll;
    QAction *mActionRename;
    QAction *mActionChangeColor;
    QAction *mActionExportLayer;
    QAction *mActionTool_Bar;
    QAction *mActionHistory_Panel;
    QAction *mActionPallete;
    QAction *mActionStyle_Panel;
    QAction *mActionLayer_Panel;
    QAction *mActionGrid;
    QAction *mActionReferenceLine;
    QAction *mActionScaleplate;
    QAction *mActionDefault;
    QAction *mActionShowAllPanels;
    QAction *mActionShake;
    QAction *mActionMotion;
    QAction *mActionSpin;
    QAction *actionPreferences;
    QAction *mActionTutorials;
    QAction *mActionForum;
    QAction *mActionWebsite;
    QAction *mActionPreferences;
    QAction *mActionZoomin;
    QAction *mActionZoomout;
    QAction *mActionImport;
    QAction *mActionCreateRect;
    QAction *mActionLoadImage;
    QAction *mActionLoadFragments;
    QAction *mActionLoadHugeImage;
    QAction *mActionReportLeaks;
    QWidget *mCentralWidget;
    QHBoxLayout *horizontalLayout;
    QTabWidget *mTabWidget;
    QMenuBar *mMenuBar;
    QMenu *mMenuFile;
    QMenu *mMenuEdit;
    QMenu *menuView;
    QMenu *menuLayer;
    QMenu *mMenuHelp;
    QMenu *menuTools;
    QMenu *menuShape;
    QStatusBar *mStatusBar;
    QToolBar *mToolBar;
    QDockWidget *mDockStyle;
    QWidget *mDockStyleWidget;
    QDockWidget *mDockPallete;
    QWidget *mDockPalleteWidget;
    QDockWidget *mDockHistory;
    QWidget *mDockHistoryWdiget;
    QDockWidget *mDockLayer;
    QWidget *mDockLayerWidget;
    QHBoxLayout *horizontalLayout_2;
    QLayerView *mLayerView;

    void setupUi(QMainWindow *SEditor)
    {
        if (SEditor->objectName().isEmpty())
            SEditor->setObjectName(QString::fromUtf8("SEditor"));
        SEditor->resize(1100, 650);
        mActionNewTemplate = new QAction(SEditor);
        mActionNewTemplate->setObjectName(QString::fromUtf8("mActionNewTemplate"));
        mActionOpenTemplate = new QAction(SEditor);
        mActionOpenTemplate->setObjectName(QString::fromUtf8("mActionOpenTemplate"));
        mActionCloseTemplate = new QAction(SEditor);
        mActionCloseTemplate->setObjectName(QString::fromUtf8("mActionCloseTemplate"));
        mActionSave = new QAction(SEditor);
        mActionSave->setObjectName(QString::fromUtf8("mActionSave"));
        mActionSaveAs = new QAction(SEditor);
        mActionSaveAs->setObjectName(QString::fromUtf8("mActionSaveAs"));
        mActionExport = new QAction(SEditor);
        mActionExport->setObjectName(QString::fromUtf8("mActionExport"));
        mActionRelease = new QAction(SEditor);
        mActionRelease->setObjectName(QString::fromUtf8("mActionRelease"));
        mActionExitEditor = new QAction(SEditor);
        mActionExitEditor->setObjectName(QString::fromUtf8("mActionExitEditor"));
        mActionUndo = new QAction(SEditor);
        mActionUndo->setObjectName(QString::fromUtf8("mActionUndo"));
        mActionRedo = new QAction(SEditor);
        mActionRedo->setObjectName(QString::fromUtf8("mActionRedo"));
        mActionCopy = new QAction(SEditor);
        mActionCopy->setObjectName(QString::fromUtf8("mActionCopy"));
        mActionCut = new QAction(SEditor);
        mActionCut->setObjectName(QString::fromUtf8("mActionCut"));
        mActionDelete = new QAction(SEditor);
        mActionDelete->setObjectName(QString::fromUtf8("mActionDelete"));
        mActionSelect = new QAction(SEditor);
        mActionSelect->setObjectName(QString::fromUtf8("mActionSelect"));
        mActionMutiSelect = new QAction(SEditor);
        mActionMutiSelect->setObjectName(QString::fromUtf8("mActionMutiSelect"));
        mActionRotate90Clockwise = new QAction(SEditor);
        mActionRotate90Clockwise->setObjectName(QString::fromUtf8("mActionRotate90Clockwise"));
        mActionRotate90Anticlockwise = new QAction(SEditor);
        mActionRotate90Anticlockwise->setObjectName(QString::fromUtf8("mActionRotate90Anticlockwise"));
        mActionRotate = new QAction(SEditor);
        mActionRotate->setObjectName(QString::fromUtf8("mActionRotate"));
        mActionFlipHorizentally = new QAction(SEditor);
        mActionFlipHorizentally->setObjectName(QString::fromUtf8("mActionFlipHorizentally"));
        mActionFlipVertically = new QAction(SEditor);
        mActionFlipVertically->setObjectName(QString::fromUtf8("mActionFlipVertically"));
        mActionScale = new QAction(SEditor);
        mActionScale->setObjectName(QString::fromUtf8("mActionScale"));
        mActionTranslate = new QAction(SEditor);
        mActionTranslate->setObjectName(QString::fromUtf8("mActionTranslate"));
        mActionSelectTool = new QAction(SEditor);
        mActionSelectTool->setObjectName(QString::fromUtf8("mActionSelectTool"));
        mActionCombineTool = new QAction(SEditor);
        mActionCombineTool->setObjectName(QString::fromUtf8("mActionCombineTool"));
        mActionBrush = new QAction(SEditor);
        mActionBrush->setObjectName(QString::fromUtf8("mActionBrush"));
        mActionEraser = new QAction(SEditor);
        mActionEraser->setObjectName(QString::fromUtf8("mActionEraser"));
        mActionPen = new QAction(SEditor);
        mActionPen->setObjectName(QString::fromUtf8("mActionPen"));
        mActionText = new QAction(SEditor);
        mActionText->setObjectName(QString::fromUtf8("mActionText"));
        mActionAnotator = new QAction(SEditor);
        mActionAnotator->setObjectName(QString::fromUtf8("mActionAnotator"));
        mActionBringForward = new QAction(SEditor);
        mActionBringForward->setObjectName(QString::fromUtf8("mActionBringForward"));
        mActionSendBackward = new QAction(SEditor);
        mActionSendBackward->setObjectName(QString::fromUtf8("mActionSendBackward"));
        mActionBringtoFront = new QAction(SEditor);
        mActionBringtoFront->setObjectName(QString::fromUtf8("mActionBringtoFront"));
        mActionSendtoBack = new QAction(SEditor);
        mActionSendtoBack->setObjectName(QString::fromUtf8("mActionSendtoBack"));
        mActionAlignLeft = new QAction(SEditor);
        mActionAlignLeft->setObjectName(QString::fromUtf8("mActionAlignLeft"));
        mActionAlignMiddle = new QAction(SEditor);
        mActionAlignMiddle->setObjectName(QString::fromUtf8("mActionAlignMiddle"));
        mActionAlignRight = new QAction(SEditor);
        mActionAlignRight->setObjectName(QString::fromUtf8("mActionAlignRight"));
        mActionAlignTop = new QAction(SEditor);
        mActionAlignTop->setObjectName(QString::fromUtf8("mActionAlignTop"));
        mActionAlignCenter = new QAction(SEditor);
        mActionAlignCenter->setObjectName(QString::fromUtf8("mActionAlignCenter"));
        mActionAlignBottom = new QAction(SEditor);
        mActionAlignBottom->setObjectName(QString::fromUtf8("mActionAlignBottom"));
        mActionShowLayer = new QAction(SEditor);
        mActionShowLayer->setObjectName(QString::fromUtf8("mActionShowLayer"));
        mActionHideLayer = new QAction(SEditor);
        mActionHideLayer->setObjectName(QString::fromUtf8("mActionHideLayer"));
        mActionDistributeHorizentally = new QAction(SEditor);
        mActionDistributeHorizentally->setObjectName(QString::fromUtf8("mActionDistributeHorizentally"));
        mActionDistributeVertically = new QAction(SEditor);
        mActionDistributeVertically->setObjectName(QString::fromUtf8("mActionDistributeVertically"));
        mActionShowAll = new QAction(SEditor);
        mActionShowAll->setObjectName(QString::fromUtf8("mActionShowAll"));
        mActionHideAll = new QAction(SEditor);
        mActionHideAll->setObjectName(QString::fromUtf8("mActionHideAll"));
        mActionRename = new QAction(SEditor);
        mActionRename->setObjectName(QString::fromUtf8("mActionRename"));
        mActionChangeColor = new QAction(SEditor);
        mActionChangeColor->setObjectName(QString::fromUtf8("mActionChangeColor"));
        mActionExportLayer = new QAction(SEditor);
        mActionExportLayer->setObjectName(QString::fromUtf8("mActionExportLayer"));
        mActionTool_Bar = new QAction(SEditor);
        mActionTool_Bar->setObjectName(QString::fromUtf8("mActionTool_Bar"));
        mActionHistory_Panel = new QAction(SEditor);
        mActionHistory_Panel->setObjectName(QString::fromUtf8("mActionHistory_Panel"));
        mActionPallete = new QAction(SEditor);
        mActionPallete->setObjectName(QString::fromUtf8("mActionPallete"));
        mActionStyle_Panel = new QAction(SEditor);
        mActionStyle_Panel->setObjectName(QString::fromUtf8("mActionStyle_Panel"));
        mActionLayer_Panel = new QAction(SEditor);
        mActionLayer_Panel->setObjectName(QString::fromUtf8("mActionLayer_Panel"));
        mActionGrid = new QAction(SEditor);
        mActionGrid->setObjectName(QString::fromUtf8("mActionGrid"));
        mActionReferenceLine = new QAction(SEditor);
        mActionReferenceLine->setObjectName(QString::fromUtf8("mActionReferenceLine"));
        mActionScaleplate = new QAction(SEditor);
        mActionScaleplate->setObjectName(QString::fromUtf8("mActionScaleplate"));
        mActionDefault = new QAction(SEditor);
        mActionDefault->setObjectName(QString::fromUtf8("mActionDefault"));
        mActionShowAllPanels = new QAction(SEditor);
        mActionShowAllPanels->setObjectName(QString::fromUtf8("mActionShowAllPanels"));
        mActionShake = new QAction(SEditor);
        mActionShake->setObjectName(QString::fromUtf8("mActionShake"));
        mActionMotion = new QAction(SEditor);
        mActionMotion->setObjectName(QString::fromUtf8("mActionMotion"));
        mActionSpin = new QAction(SEditor);
        mActionSpin->setObjectName(QString::fromUtf8("mActionSpin"));
        actionPreferences = new QAction(SEditor);
        actionPreferences->setObjectName(QString::fromUtf8("actionPreferences"));
        mActionTutorials = new QAction(SEditor);
        mActionTutorials->setObjectName(QString::fromUtf8("mActionTutorials"));
        mActionForum = new QAction(SEditor);
        mActionForum->setObjectName(QString::fromUtf8("mActionForum"));
        mActionWebsite = new QAction(SEditor);
        mActionWebsite->setObjectName(QString::fromUtf8("mActionWebsite"));
        mActionPreferences = new QAction(SEditor);
        mActionPreferences->setObjectName(QString::fromUtf8("mActionPreferences"));
        mActionZoomin = new QAction(SEditor);
        mActionZoomin->setObjectName(QString::fromUtf8("mActionZoomin"));
        mActionZoomout = new QAction(SEditor);
        mActionZoomout->setObjectName(QString::fromUtf8("mActionZoomout"));
        mActionImport = new QAction(SEditor);
        mActionImport->setObjectName(QString::fromUtf8("mActionImport"));
        mActionCreateRect = new QAction(SEditor);
        mActionCreateRect->setObjectName(QString::fromUtf8("mActionCreateRect"));
        mActionLoadImage = new QAction(SEditor);
        mActionLoadImage->setObjectName(QString::fromUtf8("mActionLoadImage"));
        mActionLoadFragments = new QAction(SEditor);
        mActionLoadFragments->setObjectName(QString::fromUtf8("mActionLoadFragments"));
        mActionLoadHugeImage = new QAction(SEditor);
        mActionLoadHugeImage->setObjectName(QString::fromUtf8("mActionLoadHugeImage"));
        mActionReportLeaks = new QAction(SEditor);
        mActionReportLeaks->setObjectName(QString::fromUtf8("mActionReportLeaks"));
        mCentralWidget = new QWidget(SEditor);
        mCentralWidget->setObjectName(QString::fromUtf8("mCentralWidget"));
        horizontalLayout = new QHBoxLayout(mCentralWidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        mTabWidget = new QTabWidget(mCentralWidget);
        mTabWidget->setObjectName(QString::fromUtf8("mTabWidget"));

        horizontalLayout->addWidget(mTabWidget);

        SEditor->setCentralWidget(mCentralWidget);
        mMenuBar = new QMenuBar(SEditor);
        mMenuBar->setObjectName(QString::fromUtf8("mMenuBar"));
        mMenuBar->setGeometry(QRect(0, 0, 1100, 26));
        mMenuFile = new QMenu(mMenuBar);
        mMenuFile->setObjectName(QString::fromUtf8("mMenuFile"));
        mMenuEdit = new QMenu(mMenuBar);
        mMenuEdit->setObjectName(QString::fromUtf8("mMenuEdit"));
        menuView = new QMenu(mMenuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuLayer = new QMenu(mMenuBar);
        menuLayer->setObjectName(QString::fromUtf8("menuLayer"));
        mMenuHelp = new QMenu(mMenuBar);
        mMenuHelp->setObjectName(QString::fromUtf8("mMenuHelp"));
        menuTools = new QMenu(mMenuBar);
        menuTools->setObjectName(QString::fromUtf8("menuTools"));
        menuShape = new QMenu(menuTools);
        menuShape->setObjectName(QString::fromUtf8("menuShape"));
        SEditor->setMenuBar(mMenuBar);
        mStatusBar = new QStatusBar(SEditor);
        mStatusBar->setObjectName(QString::fromUtf8("mStatusBar"));
        SEditor->setStatusBar(mStatusBar);
        mToolBar = new QToolBar(SEditor);
        mToolBar->setObjectName(QString::fromUtf8("mToolBar"));
        SEditor->addToolBar(Qt::LeftToolBarArea, mToolBar);
        mDockStyle = new QDockWidget(SEditor);
        mDockStyle->setObjectName(QString::fromUtf8("mDockStyle"));
        mDockStyleWidget = new QWidget();
        mDockStyleWidget->setObjectName(QString::fromUtf8("mDockStyleWidget"));
        mDockStyle->setWidget(mDockStyleWidget);
        SEditor->addDockWidget(Qt::RightDockWidgetArea, mDockStyle);
        mDockPallete = new QDockWidget(SEditor);
        mDockPallete->setObjectName(QString::fromUtf8("mDockPallete"));
        mDockPalleteWidget = new QWidget();
        mDockPalleteWidget->setObjectName(QString::fromUtf8("mDockPalleteWidget"));
        mDockPallete->setWidget(mDockPalleteWidget);
        SEditor->addDockWidget(Qt::RightDockWidgetArea, mDockPallete);
        mDockHistory = new QDockWidget(SEditor);
        mDockHistory->setObjectName(QString::fromUtf8("mDockHistory"));
        mDockHistoryWdiget = new QWidget();
        mDockHistoryWdiget->setObjectName(QString::fromUtf8("mDockHistoryWdiget"));
        mDockHistory->setWidget(mDockHistoryWdiget);
        SEditor->addDockWidget(Qt::RightDockWidgetArea, mDockHistory);
        mDockLayer = new QDockWidget(SEditor);
        mDockLayer->setObjectName(QString::fromUtf8("mDockLayer"));
        mDockLayerWidget = new QWidget();
        mDockLayerWidget->setObjectName(QString::fromUtf8("mDockLayerWidget"));
        horizontalLayout_2 = new QHBoxLayout(mDockLayerWidget);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(1, 1, 1, 1);
        mLayerView = new QLayerView(mDockLayerWidget);
        mLayerView->setObjectName(QString::fromUtf8("mLayerView"));

        horizontalLayout_2->addWidget(mLayerView);

        mDockLayer->setWidget(mDockLayerWidget);
        SEditor->addDockWidget(Qt::RightDockWidgetArea, mDockLayer);

        mMenuBar->addAction(mMenuFile->menuAction());
        mMenuBar->addAction(mMenuEdit->menuAction());
        mMenuBar->addAction(menuTools->menuAction());
        mMenuBar->addAction(menuLayer->menuAction());
        mMenuBar->addAction(menuView->menuAction());
        mMenuBar->addAction(mMenuHelp->menuAction());
        mMenuFile->addAction(mActionNewTemplate);
        mMenuFile->addAction(mActionOpenTemplate);
        mMenuFile->addSeparator();
        mMenuFile->addAction(mActionSave);
        mMenuFile->addAction(mActionSaveAs);
        mMenuFile->addAction(mActionImport);
        mMenuFile->addAction(mActionExport);
        mMenuFile->addSeparator();
        mMenuFile->addAction(mActionCloseTemplate);
        mMenuFile->addSeparator();
        mMenuFile->addAction(mActionRelease);
        mMenuFile->addSeparator();
        mMenuFile->addAction(mActionExitEditor);
        mMenuEdit->addAction(mActionUndo);
        mMenuEdit->addAction(mActionRedo);
        mMenuEdit->addSeparator();
        mMenuEdit->addAction(mActionCopy);
        mMenuEdit->addAction(mActionCut);
        mMenuEdit->addAction(mActionDelete);
        mMenuEdit->addSeparator();
        mMenuEdit->addAction(mActionSelect);
        mMenuEdit->addAction(mActionMutiSelect);
        mMenuEdit->addSeparator();
        mMenuEdit->addAction(mActionRotate90Clockwise);
        mMenuEdit->addAction(mActionRotate90Anticlockwise);
        mMenuEdit->addAction(mActionRotate);
        mMenuEdit->addSeparator();
        mMenuEdit->addAction(mActionFlipHorizentally);
        mMenuEdit->addAction(mActionFlipVertically);
        mMenuEdit->addSeparator();
        mMenuEdit->addAction(mActionScale);
        mMenuEdit->addSeparator();
        mMenuEdit->addAction(mActionTranslate);
        menuView->addAction(mActionTool_Bar);
        menuView->addAction(mActionStyle_Panel);
        menuView->addAction(mActionLayer_Panel);
        menuView->addAction(mActionPallete);
        menuView->addAction(mActionHistory_Panel);
        menuView->addSeparator();
        menuView->addAction(mActionGrid);
        menuView->addAction(mActionReferenceLine);
        menuView->addSeparator();
        menuView->addAction(mActionDefault);
        menuView->addAction(mActionShowAllPanels);
        menuView->addSeparator();
        menuView->addAction(mActionZoomin);
        menuView->addAction(mActionZoomout);
        menuView->addSeparator();
        menuLayer->addAction(mActionBringForward);
        menuLayer->addAction(mActionSendBackward);
        menuLayer->addSeparator();
        menuLayer->addAction(mActionBringtoFront);
        menuLayer->addAction(mActionSendtoBack);
        menuLayer->addSeparator();
        menuLayer->addAction(mActionAlignLeft);
        menuLayer->addAction(mActionAlignMiddle);
        menuLayer->addAction(mActionAlignRight);
        menuLayer->addSeparator();
        menuLayer->addAction(mActionAlignTop);
        menuLayer->addAction(mActionAlignCenter);
        menuLayer->addAction(mActionAlignBottom);
        menuLayer->addSeparator();
        menuLayer->addAction(mActionDistributeHorizentally);
        menuLayer->addAction(mActionDistributeVertically);
        menuLayer->addSeparator();
        menuLayer->addAction(mActionShowLayer);
        menuLayer->addAction(mActionHideLayer);
        menuLayer->addSeparator();
        menuLayer->addAction(mActionShowAll);
        menuLayer->addAction(mActionHideAll);
        menuLayer->addSeparator();
        menuLayer->addAction(mActionRename);
        menuLayer->addAction(mActionChangeColor);
        menuLayer->addSeparator();
        menuLayer->addAction(mActionExportLayer);
        mMenuHelp->addAction(mActionTutorials);
        mMenuHelp->addAction(mActionForum);
        mMenuHelp->addAction(mActionWebsite);
        menuTools->addAction(mActionSelectTool);
        menuTools->addAction(mActionCombineTool);
        menuTools->addSeparator();
        menuTools->addAction(mActionBrush);
        menuTools->addAction(mActionEraser);
        menuTools->addAction(mActionPen);
        menuTools->addAction(menuShape->menuAction());
        menuTools->addAction(mActionText);
        menuTools->addSeparator();
        menuTools->addAction(mActionAnotator);
        menuTools->addSeparator();
        menuTools->addAction(mActionPreferences);
        menuTools->addAction(mActionReportLeaks);
        menuShape->addAction(mActionCreateRect);
        menuShape->addAction(mActionLoadImage);
        menuShape->addAction(mActionLoadFragments);
        menuShape->addAction(mActionLoadHugeImage);

        retranslateUi(SEditor);

        mTabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(SEditor);
    } // setupUi

    void retranslateUi(QMainWindow *SEditor)
    {
        SEditor->setWindowTitle(QCoreApplication::translate("SEditor", "S-GIS Editor", nullptr));
        mActionNewTemplate->setText(QCoreApplication::translate("SEditor", "New Workspace...", nullptr));
        mActionOpenTemplate->setText(QCoreApplication::translate("SEditor", "Open Workspace...", nullptr));
        mActionCloseTemplate->setText(QCoreApplication::translate("SEditor", "Close Template", nullptr));
        mActionSave->setText(QCoreApplication::translate("SEditor", "Save", nullptr));
        mActionSaveAs->setText(QCoreApplication::translate("SEditor", "Save as...", nullptr));
        mActionExport->setText(QCoreApplication::translate("SEditor", "Export", nullptr));
        mActionRelease->setText(QCoreApplication::translate("SEditor", "Release", nullptr));
        mActionExitEditor->setText(QCoreApplication::translate("SEditor", "Exit Editor", nullptr));
        mActionUndo->setText(QCoreApplication::translate("SEditor", "Undo", nullptr));
        mActionRedo->setText(QCoreApplication::translate("SEditor", "Redo", nullptr));
        mActionCopy->setText(QCoreApplication::translate("SEditor", "Copy", nullptr));
        mActionCut->setText(QCoreApplication::translate("SEditor", "Cut", nullptr));
        mActionDelete->setText(QCoreApplication::translate("SEditor", "Delete", nullptr));
        mActionSelect->setText(QCoreApplication::translate("SEditor", "Select", nullptr));
        mActionMutiSelect->setText(QCoreApplication::translate("SEditor", "Muti-Select", nullptr));
        mActionRotate90Clockwise->setText(QCoreApplication::translate("SEditor", "Rotate 90\302\260 Clockwise", nullptr));
        mActionRotate90Anticlockwise->setText(QCoreApplication::translate("SEditor", "Rotate 90\302\260 Counter-clockwise", nullptr));
        mActionRotate->setText(QCoreApplication::translate("SEditor", "Rotate...", nullptr));
        mActionFlipHorizentally->setText(QCoreApplication::translate("SEditor", "Flip Horizentally", nullptr));
        mActionFlipVertically->setText(QCoreApplication::translate("SEditor", "Flip Vertically", nullptr));
        mActionScale->setText(QCoreApplication::translate("SEditor", "Scale...", nullptr));
        mActionTranslate->setText(QCoreApplication::translate("SEditor", "Translate...", nullptr));
        mActionSelectTool->setText(QCoreApplication::translate("SEditor", "Select Tool", nullptr));
        mActionCombineTool->setText(QCoreApplication::translate("SEditor", "Combine Tool", nullptr));
        mActionBrush->setText(QCoreApplication::translate("SEditor", "Brush", nullptr));
        mActionEraser->setText(QCoreApplication::translate("SEditor", "Eraser", nullptr));
        mActionPen->setText(QCoreApplication::translate("SEditor", "Pen", nullptr));
        mActionText->setText(QCoreApplication::translate("SEditor", "Text", nullptr));
        mActionAnotator->setText(QCoreApplication::translate("SEditor", "Anotator", nullptr));
        mActionBringForward->setText(QCoreApplication::translate("SEditor", "Bring Forward", nullptr));
        mActionSendBackward->setText(QCoreApplication::translate("SEditor", "Send Backward", nullptr));
        mActionBringtoFront->setText(QCoreApplication::translate("SEditor", "Bring to Front", nullptr));
        mActionSendtoBack->setText(QCoreApplication::translate("SEditor", "Send to Back", nullptr));
        mActionAlignLeft->setText(QCoreApplication::translate("SEditor", "Align Left", nullptr));
        mActionAlignMiddle->setText(QCoreApplication::translate("SEditor", "Align Middle", nullptr));
        mActionAlignRight->setText(QCoreApplication::translate("SEditor", "Align Right", nullptr));
        mActionAlignTop->setText(QCoreApplication::translate("SEditor", "Align Top", nullptr));
        mActionAlignCenter->setText(QCoreApplication::translate("SEditor", "Align Center", nullptr));
        mActionAlignBottom->setText(QCoreApplication::translate("SEditor", "Align Bottom", nullptr));
        mActionShowLayer->setText(QCoreApplication::translate("SEditor", "Show Layer", nullptr));
        mActionHideLayer->setText(QCoreApplication::translate("SEditor", "Hide Layer", nullptr));
        mActionDistributeHorizentally->setText(QCoreApplication::translate("SEditor", "Distribute Horizentally", nullptr));
        mActionDistributeVertically->setText(QCoreApplication::translate("SEditor", "Distribute Vertically", nullptr));
        mActionShowAll->setText(QCoreApplication::translate("SEditor", "Show All", nullptr));
        mActionHideAll->setText(QCoreApplication::translate("SEditor", "Hide All", nullptr));
        mActionRename->setText(QCoreApplication::translate("SEditor", "Rename Layer...", nullptr));
        mActionChangeColor->setText(QCoreApplication::translate("SEditor", "Change Color...", nullptr));
        mActionExportLayer->setText(QCoreApplication::translate("SEditor", "Export Layer...", nullptr));
        mActionTool_Bar->setText(QCoreApplication::translate("SEditor", "Tool Bar", nullptr));
        mActionHistory_Panel->setText(QCoreApplication::translate("SEditor", "History Panel", nullptr));
        mActionPallete->setText(QCoreApplication::translate("SEditor", "Pallete", nullptr));
        mActionStyle_Panel->setText(QCoreApplication::translate("SEditor", "Style Panel", nullptr));
        mActionLayer_Panel->setText(QCoreApplication::translate("SEditor", "Layer Panel", nullptr));
        mActionGrid->setText(QCoreApplication::translate("SEditor", "Grid", nullptr));
        mActionReferenceLine->setText(QCoreApplication::translate("SEditor", "Reference Line", nullptr));
        mActionScaleplate->setText(QCoreApplication::translate("SEditor", "Scaleplate", nullptr));
        mActionDefault->setText(QCoreApplication::translate("SEditor", "Default View", nullptr));
        mActionShowAllPanels->setText(QCoreApplication::translate("SEditor", "Show All Panels", nullptr));
        mActionShake->setText(QCoreApplication::translate("SEditor", "Shake", nullptr));
        mActionMotion->setText(QCoreApplication::translate("SEditor", "Motion", nullptr));
        mActionSpin->setText(QCoreApplication::translate("SEditor", "Spin", nullptr));
        actionPreferences->setText(QCoreApplication::translate("SEditor", "Preferences...", nullptr));
        mActionTutorials->setText(QCoreApplication::translate("SEditor", "Tutorials", nullptr));
        mActionForum->setText(QCoreApplication::translate("SEditor", "Forum", nullptr));
        mActionWebsite->setText(QCoreApplication::translate("SEditor", "Website", nullptr));
        mActionPreferences->setText(QCoreApplication::translate("SEditor", "Preferences...", nullptr));
        mActionZoomin->setText(QCoreApplication::translate("SEditor", "Zoom in", nullptr));
#if QT_CONFIG(shortcut)
        mActionZoomin->setShortcut(QCoreApplication::translate("SEditor", "Ctrl+=", nullptr));
#endif // QT_CONFIG(shortcut)
        mActionZoomout->setText(QCoreApplication::translate("SEditor", "Zoom out", nullptr));
#if QT_CONFIG(shortcut)
        mActionZoomout->setShortcut(QCoreApplication::translate("SEditor", "Ctrl+-", nullptr));
#endif // QT_CONFIG(shortcut)
        mActionImport->setText(QCoreApplication::translate("SEditor", "Import...", nullptr));
        mActionCreateRect->setText(QCoreApplication::translate("SEditor", "[Test] Create Rect", nullptr));
        mActionLoadImage->setText(QCoreApplication::translate("SEditor", "[Test] Load Image", nullptr));
        mActionLoadFragments->setText(QCoreApplication::translate("SEditor", "[Test] Load Fragments", nullptr));
        mActionLoadHugeImage->setText(QCoreApplication::translate("SEditor", "[Test] Load Huge Image", nullptr));
        mActionReportLeaks->setText(QCoreApplication::translate("SEditor", "[Test] Print Memory Leak Report", nullptr));
        mMenuFile->setTitle(QCoreApplication::translate("SEditor", "File", nullptr));
        mMenuEdit->setTitle(QCoreApplication::translate("SEditor", "Edit", nullptr));
        menuView->setTitle(QCoreApplication::translate("SEditor", "View", nullptr));
        menuLayer->setTitle(QCoreApplication::translate("SEditor", "Layer", nullptr));
        mMenuHelp->setTitle(QCoreApplication::translate("SEditor", "Help", nullptr));
        menuTools->setTitle(QCoreApplication::translate("SEditor", "Tool", nullptr));
        menuShape->setTitle(QCoreApplication::translate("SEditor", "Shape", nullptr));
        mToolBar->setWindowTitle(QString());
        mDockStyle->setWindowTitle(QCoreApplication::translate("SEditor", "Style", nullptr));
        mDockPallete->setWindowTitle(QCoreApplication::translate("SEditor", "Pallete", nullptr));
        mDockHistory->setWindowTitle(QCoreApplication::translate("SEditor", "History", nullptr));
        mDockLayer->setWindowTitle(QCoreApplication::translate("SEditor", "Layer", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SEditor: public Ui_SEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEDITOR_H
