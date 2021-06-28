#ifndef QFILELISTMODEL_H
#define QFILELISTMODEL_H

#include <QStringListModel>

class QFileListModel : public QStringListModel
{
    //Q_OBJECT

    /*-----构造函数与析构函数-----*/
public:
    explicit QFileListModel(QObject *parent = nullptr);
    virtual ~QFileListModel();

    /*-----虚函数重载-----*/
protected:

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

    /*-----UI-----*/
private:


    // QAbstractItemModel interface
public:
    virtual bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    virtual Qt::DropActions supportedDropActions() const override;
};

#endif // QFILELISTMODEL_H
