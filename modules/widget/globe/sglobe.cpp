#include "sglobe.h"
#include "ui_sglobe.h"
#include "qdialog.h"
#include "qinputdialog.h"

SGlobe::SGlobe(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::SGlobe)
{
    ui->setupUi(this);
    _initialize();
}

SGlobe::~SGlobe()
{
    delete ui;
}

void SGlobe::closeEvent(QCloseEvent* event)
{
    emit closed();
}

void SGlobe::onActionLineTriggered()
{
    QString strInput = QInputDialog::getText(this, "Test Line", "Input Coordinate xxx,xxx,xxx,xxx,xxx,xxx");
    QStringList coorsList = strInput.split(',');
    std::vector<double> coorsVec;
    foreach (auto& coorString, coorsList)
        coorsVec.emplace_back(coorString.toDouble());
#ifndef DISABLE_GLOBE
    ui->mGlobeView->drawLine(coorsVec);
#endif
}

void SGlobe::_initialize()
{
    _initializeConnections();
    emit initComplete();
}

void SGlobe::_initializeConnections()
{
    connect(ui->mActionLine, &QAction::triggered, this, &SGlobe::onActionLineTriggered);
}
