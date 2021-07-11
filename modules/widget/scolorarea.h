#ifndef SCOLORAREA_H
#define SCOLORAREA_H

#include <QPushButton>
#include <QColor>
//继承PushButton，用来显示颜色
class SColorArea : public QPushButton
{
    Q_OBJECT
public:
    explicit SColorArea(QColor color, QWidget *parent = nullptr);
public:
    QColor getColor();
private:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
private:
    bool mpEnter;
    QColor mpColor; //控件颜色
};

#endif // SCOLORAREA_H
