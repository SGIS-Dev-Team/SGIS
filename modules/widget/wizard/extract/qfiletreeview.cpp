#include "qfiletreeview.h"
#include <QMouseEvent>

QFileTreeView::QFileTreeView(QWidget *parent):
    QTreeView(parent)
{
    _initialize();
}

QFileTreeView::~QFileTreeView()
{

}
void QFileTreeView::mouseReleaseEvent(QMouseEvent *event)
{
    QTreeView::mouseReleaseEvent(event);
    Q_ASSERT(model());
}

void QFileTreeView::mousePressEvent(QMouseEvent *event)
{
    QTreeView::mousePressEvent(event);
}

void QFileTreeView::_initialize()
{
    this->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

    _initializeConnections();
}

void QFileTreeView::_initializeConnections()
{

}
