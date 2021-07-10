#ifndef SEDITOR_H
#define SEDITOR_H

#include <QMainWindow>
#include <modules/global.h>
#include "modules/widget/qcanvasarea.h"
#include"modules/widget/qcanvas.h"
#include<QLabel>
#include <modules/widget/qdataimportwizard.h>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QListWidget>
#include <QGridLayout>
#include "modules/global.h"
#include "vld.h"

namespace Ui
{
class SEditor;
}

class ImageInfoWidget;
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
    void initComplete();

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
    //导入栅格数据
    void onActionImportRasterTriggered();
    //[栅格数据导入对话框事件响应]
    void onImportingRaster();
    //导出内存泄露报告
    void onActionReportLeaksTriggered();
    //[矢量数据导入事件响应]
    void onActionImportShapeFileTriggered();

    //[切换绘图区响应]
    void onTabSwitched();
    //[画布事件响应]
    //鼠标移动
    void onCanvasMouseMoved(QPointF Log_pos);
    //缩放
    void onCanvasScaled(double value);
    //[图层事件响应]
    void onLayersUpdated(SLayerManager* which);
    //输出日志信息
    void onOutput(const QString& entry);

    //颜色选中槽函数
    void onSelectColor();

    /*-----虚函数重载-----*/
private:
    void closeEvent(QCloseEvent* event)override;
    virtual bool eventFilter(QObject *watched, QEvent *event);
    /*-----成员变量-----*/
private:
    //
    void initDockPalleteWidget();
    void adjustPallete(QSize size);

    //更新tiff图层信息
    void updateImageInfo(SLayerManager* which);
    void fillByImageFilePath(QString strFilePath);
    void fillImageFileInfo(GDALDataset* pGDALDataset);
    void fillImageLayerInfo(GDALDataset* pGDALDataset);
    void fillImageProjectionInfo(GDALDataset* pGDALDataset);
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
private:
    //初始化函数
    void _initialize();
    //初始化链接
    void _initializeConnections();
    void _connectDataImportWizard();

    //创建新的绘图区
    void createWorkspace(const QSize& CanvasSize = DEFAULT_CANVAS_SIZE);

    /*-----UI与控件-----*/
private:
    Ui::SEditor* ui;
    //[状态栏控件]
    QLabel* mpStatLblCursorPos;
    QLabel* mpStatLblCanvasScale;
    QLabel* mpStatLblGSD;      //先用GSD代替比例尺
    QLabel* mpStatLblProjCS;
    //[数据导入向导]
    QDataImportWizard* mpImportDialog{nullptr};
    //
    QGridLayout* mpPaletteLayout;
    std::vector<QWidget*> mpPaletteButtons{};
};

#endif // SEditor_H
