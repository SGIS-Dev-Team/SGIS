﻿#ifndef QBANDSELECTDIALOG_H
#define QBANDSELECTDIALOG_H

#include "modules/global.h"
#include <QDialog>
#include <modules/paint/simage.h>
#include <QThread>

namespace Ui
{
class QBandSelectDialog;
}

class QBandSelectDialog : public QDialog
{
    Q_OBJECT
    typedef unsigned char UCHAR;

    /*-----构造函数与析构函数-----*/
public:
    explicit QBandSelectDialog(QWidget *parent = nullptr);
    explicit QBandSelectDialog(const QString &imagePath, QWidget *parent = nullptr);
    ~QBandSelectDialog();

    /*-----虚函数重载-----*/
public:

    /*-----信号-----*/
signals:
    void startBuildingThread(QString imgPath);

    /*-----槽函数-----*/
private slots:
    //金字塔、分片处理完毕
    void onOverviewBuilt(QString strFragPath);
    //进度条
    void onProgressUpdated(int progress, QString info);
    //组合框选项变化
    void onComboBoxIndexChanged(int idx);
    //按下确定按钮
    void onButtonOkClicked();
    //按下取消按钮
    void onButtonCancelClicked();


    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
protected:
    //原始图像路径
    QString mStrOriImgPath{};
    //分片图像的路径
    QString mStrFragPath{};

    //选中的波段:波段序号从1开始
    int mnRedBandIdx{0};
    int mnGreenBandIdx{0};
    int mnBlueBandIdx{0};

    //顶层图像预览数据
    SImage mImage{nullptr};
    int mnBands{0};

    //是否根据选择框更新预览图像
    bool mbUpdatePreviewImg{false};

    //构建器线程
    QThread mSlicerThread{};

    /*-----成员函数-----*/
public:
    //[访问函数]
    QString getFragPath() const;

    int getRedBandIdx() const;
    int getGreenBandIdx() const;
    int getBlueBandIdx() const;

    QString getOriginalImagePath() const;

    //获取RGB对应被选中的波段（波段从1开始）
    void getBandIdices(int* pRGBIdx)const;

    //获取均衡化函数
    void getHistEqFunc(std::shared_ptr<void> pEqFunc[]);

    //[修改函数]
    void setOriginalImagePath(const QString &strOriImgPath);
    //[功能函数]


private:
    void _initialize();
    void _initializeConnections();
    void _releaseAll();

private:
    Ui::QBandSelectDialog *ui;
};

#endif // QBANDSELECTDIALOG_H
