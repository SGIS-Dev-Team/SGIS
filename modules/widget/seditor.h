#ifndef SEDITOR_H
#define SEDITOR_H

#include <QMainWindow>
#include <modules/global.h>
#include "modules/widget/qcanvasarea.h"
#include"modules/widget/qcanvas.h"
#include<QLabel>
#include <modules/widget/qdataimportwizard.h>

namespace Ui
{
class SEditor;
}

class SEditor : public QMainWindow
{
    Q_OBJECT
    /*-----构造函数与析构函数-----*/
public:
    explicit SEditor(QWidget* parent = nullptr);
    virtual ~SEditor();

    /*-----信号-----*/
signals:
    void closed();

    /*-----槽函数-----*/
private slots:
    //[菜单选项响应]
    void onActionZoominTriggered();
    void onActionZoomoutTriggered();
    //画布前后移动
    void onActionBringForwardTriggered();
    void onActionSendBackwardTriggered();
    void onActionBringtoFrontTriggered();
    void onActionSendtoBackTriggered();
    //绘图对象移动
    void onActionAlignLeftTriggered();
    void onActionAlignMiddleTriggered();
    void onActionAlignRightTriggered();
    void onActionAlignTopTriggered();
    void onActionAlignCenterTriggered();
    void onActionAlignBottomTriggered();
    //多个绘图对象等间距操作
    void onActionDistributeHorizentallyTriggered();
    void onActionDistributeVerticallyTriggered();
    //[测试]
    void onActionCreateRectTriggered();
    void onActionLoadImageTriggered();
    void onActionLoadFragmentsTriggered();
    void onActionLoadHugeImageTriggered();
    //[数据导入对话框时间响应]
    void onImportingData();
    void onActionReportLeaksTriggered();
    //[切换绘图区响应]
    void onTabSwitched();
    //[画布事件响应]
    //鼠标移动
    void onCanvasMouseMoved(QPointF Log_pos);
    //缩放
    void onCanvasScaled(double value);
    //[图层事件响应]
    void onLayersUpdated(SLayerManager* which);

    /*-----虚函数重载-----*/
public:
    void closeEvent(QCloseEvent* event)override;

    /*-----成员变量-----*/
private:
    //[Me]
    QString Me = QString("SEditor");
    //[当前绘图区]
    QCanvasArea* mpCurCanvasArea{};
    //[当前文档]
    std::shared_ptr<SDocument> mpCurDoc{};
    //[已打开并加载的绘图区]
    std::vector<QCanvasArea*> mpCanvasAreaVec{};
    //[已打开并加载的文档]
    std::vector<std::shared_ptr<SDocument>> mpDocVec{};

    /*-----成员函数-----*/
public:
    //初始化函数
    void initialize();
    //初始化链接
    void initializeConnections();
    //创建新的绘图区
    void createWorkspace(const QSize& CanvasSize = DEFAULT_CANVAS_SIZE);

    /*-----UI与控件-----*/
private:
    Ui::SEditor* ui;
    //[状态栏控件]
    QLabel* mpStatLblCursorPos;
    QLabel* mpStatLblCanvasScale;
    //[数据导入向导]
    QDataImportWizard* mpImportDialog;
};

#endif // SEditor_H
