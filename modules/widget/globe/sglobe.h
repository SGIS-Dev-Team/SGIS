#ifndef SGLOBE_H
#define SGLOBE_H

#include <QMainWindow>

namespace Ui
{
class SGlobe;
}

class SGlobe : public QMainWindow
{
    Q_OBJECT

    /*-----构造函数与析构函数-----*/
public:
    explicit SGlobe(QWidget* parent = nullptr);
    virtual ~SGlobe();

    /*-----虚函数重载-----*/
public:
    virtual void closeEvent(QCloseEvent* event) override;

    /*-----信号-----*/
signals:
    void closed();
    void initComplete();

    /*-----槽函数-----*/
private slots:
    void onActionLineTriggered();

    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
protected:


    /*-----成员函数-----*/
public:
    //[访问函数]

    //[修改函数]

    //[功能函数]
    void _initialize();
    void _initializeConnections();

    /*-----UI-----*/
private:
    Ui::SGlobe* ui;
};

#endif // SGLOBE_H
