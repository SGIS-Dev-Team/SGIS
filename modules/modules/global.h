#ifndef GLOBAL_H
#define GLOBAL_H

class SDocument;

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
//默认分片影像读取栈大小
#define DEFAULT_FRAG_TEMP_SIZE 20
#define DEFAULT_READ_IN_STACK_SIZE 20

//外接矩形逻辑像素
#define BOUND_RECT_PEN_WIDTH 2
#define BOUND_RECT_CORNER_RADIUS 5

//拖动触发的最小曼哈顿距离（像素）
#define DRAG_TRIGGERING_DISTANCE 3

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
