#ifndef GLOBAL_H
#define GLOBAL_H
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
#define LOG_NAME_PFX "/sgif_log_"
//单个文件最大日志条目数
#define LOG_MAX_ENTRY_COUNT 1000
//默认画布大小
#define DEFAULT_CANVAS_SIZE QSize(10000,10000)
//绘图对象类名
namespace sgif
{
enum PaintObject
{
    ObjectBase,
    ShapeBase,
    TextBase,
    BrushBase,
    ImageBase
};
}
#endif // GLOBAL_H
