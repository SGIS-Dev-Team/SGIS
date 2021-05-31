#ifndef GLOBAL_H
#define GLOBAL_H

//Visual Leak Detector内存泄露探查器
#pragma comment(lib,"vld.lib")
#ifndef QT_NO_DEBUG
#include <vld.h>
#endif
//仅在Debug模式下使用，调用VLDReportLeak()在应用程序输出中查看内存泄露报告

#include"slogger.h"
#include <QStandardPaths>
#include <QDebug>
#include <cassert>
//配置文件开头
#define CFG_HEAD "# S-GIS Application Configuration File\n"
#define LOG_HEAD "# S-GIS Application Log File\n"
//版本号
#define VER 0.01
#define VER_STR "# version: " + QString::number(VER, 'g', 2) + "\n"
//网站
#define WEBSITE "# [WebSite]\n"
//托盘菜单配置文件名
#define TRAY_MENU_CFG_NAME "/tmenu.cfg"
//日志文件名前缀
#define LOG_NAME_PFX "/sgis_log_"
//单个文件最大日志条目数
#define LOG_MAX_ENTRY_COUNT 1000
//默认画布大小
#define DEFAULT_CANVAS_SIZE QSize(10*15200,10*10200)
//图层预览图标大小
#define LAYER_ICON_SIZE QSize(32,32)
//debug流辅助宏函数
#define SSTR(expr)#expr
#define SDBG(expr)SSTR(expr)<<expr<<"\n"
#define S2DBG(expr1,expr2)SDBG(expr1)<<SDBG(expr2)
#define S3DBG(expr1,expr2,expr3)S2DBG(expr1,expr2)<<SDBG(expr3)
#define S4DBG(expr1,expr2,expr3,expr4)S3DBG(expr1,expr2,expr3)<<SDBG(expr4)
#define S5DBG(expr1,expr2,expr3,expr4,expr5)S4DBG(expr1,expr2,expr3,expr4)<<SDBG(expr5)
#define S6DBG(expr1,expr2,expr3,expr4,expr5,expr6)S5DBG(expr1,expr2,expr3,expr4,expr5)<<SDBG(expr6)
//debug计时器
#include <ctime>
#ifndef QT_NO_DEBUG
#define CLOCK_START(N)     std::clock_t start ## N = clock();
#define CLOCK_STOP(N)      qDebug() << "Clock "<<N<<" Elapsed Time at "<<__FILE__<<"["<<__LINE__<<"] :" << std::clock() - start ## N <<" ms";
#else
#define CLOCK_START(N)
#define CLOCK_STOP(N)
#endif

//默认分片影像读取栈大小
#define DEFAULT_FRAG_TEMP_SIZE 20
#define DEFAULT_READ_IN_STACK_SIZE 100

//外接矩形逻辑像素
#define BOUND_RECT_PEN_WIDTH 2
#define BOUND_RECT_CORNER_RADIUS 5

//拖动触发的最小曼哈顿距离（像素）
#define DRAG_TRIGGERING_DISTANCE 3

//旋转图标矩形
#define ROTATE_ICON_RADIUS 12
#define ROTATE_ICON_RECT QRect(-ROTATE_ICON_RADIUS, -ROTATE_ICON_RADIUS, ROTATE_ICON_RADIUS * 2, ROTATE_ICON_RADIUS * 2)
#define ROTATE_ICON_HOVER_DISTANCE 27
#define ROTATE_ICON_CENTER_Y -(ROTATE_ICON_HOVER_DISTANCE + ROTATE_ICON_RADIUS)         //27+24/2   中心是相对于包围矩形第一个中点的

//圆周率，懂得都懂
#define PI 3.141592653589793

//缩略图大小（顶层金字塔限制大小）
#define DEFAULT_TOP_PYRAMID_SIZE 1600
//默认逻辑分片尺寸(长和宽)
#define DEFAULT_LOGICAL_FRAGMENT_SIZE 800

//文档位置
#define SGIS_DOCUMENT_FOLDER QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/sgis"

//金字塔缓存文件夹
#define PYRAMID_FOLDER_NAME "/pyramid"

//绘图对象类名
namespace sgis
{
enum PaintObject
{
    ObjectBase,
    ShapeBase,
    TextBase,
    BrushBase,
    ImageBase,
    FragImageBase

};
}
#endif // GLOBAL_H
