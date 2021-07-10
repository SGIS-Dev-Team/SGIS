#include "scolorarea.h"
#include <QPainter>
SColorArea::SColorArea(QColor color, QWidget *parent)
    : QPushButton(parent)
    , mpColor(color)
    , mpEnter(false)
{

}

QColor SColorArea::getColor()
{
    return mpColor;
}
//鼠标进入标记
void SColorArea::enterEvent(QEvent *event)
{
    mpEnter = true;
}
//鼠标退出标记
void SColorArea::leaveEvent(QEvent *event)
{
    mpEnter = false;
}

void SColorArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //对控件填充颜色
    auto drawRect = rect();
    //如果鼠标未进入控件范围，就把正常范围缩小2
    //鼠标进入控件范围就按正常填充，制造出选择的动态效果
    if (!mpEnter)
    {
        drawRect.adjust(2, 2, -2, -2);
    }
    painter.fillRect(drawRect, mpColor);
}
