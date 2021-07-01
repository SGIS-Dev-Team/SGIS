#ifndef QFILELISTMODEL_H
#define QFILELISTMODEL_H

#include <QStringListModel>
#include <modules/global.h>

class QFileListModel : public QStringListModel
{
    Q_OBJECT
public:

    /*-----构造函数与析构函数-----*/
public:
    explicit QFileListModel(QObject *parent = nullptr);
    virtual ~QFileListModel(){}

    /*-----虚函数重载-----*/
protected:
    virtual bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    virtual Qt::DropActions supportedDropActions() const override;

    /*-----信号-----*/
signals:

    /*-----槽函数-----*/
private slots:

    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
protected:


    /*-----成员函数-----*/
public:
    //[访问函数]

    //[修改函数]

    //[功能函数]
};

#endif // QFILELISTMODEL_H
