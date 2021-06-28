#ifndef QFILETREEVIEW_H
#define QFILETREEVIEW_H

#include <QTreeView>

class QFileTreeView : public QTreeView
{
    Q_OBJECT

    /*-----构造函数与析构函数-----*/
public:
    explicit QFileTreeView(QWidget *parent = nullptr);
    virtual ~QFileTreeView();

    /*-----虚函数重载-----*/
protected:
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

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
private:
    void _initialize();
    void _initializeConnections();

    /*-----UI-----*/
private:
};

#endif // QFILETREEVIEW_H
