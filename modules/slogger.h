#ifndef SLogger_H
#define SLogger_H

//---------------------------------
//         日志记录器
//          SLogger
//    用于记录应用程序运行日志与错误日志
//    单例全局对象
//---------------------------------
#include<QFile>
#include<QTextStream>
#include"modules/global.h"

class SLogger : public QObject
{
    Q_OBJECT

    //日志类型
public:
    enum LogType
    {
        DebugInfo,      //测试信息
        RunningStatus,  //运行状态
        UserAction,     //用户操作
        LocalError,     //本地错误
        RemoteError     //远程错误
    };
signals:
	void signalAddEntry(const QString& entry);
    //构造函数与析构函数
private:
    SLogger();
    virtual ~SLogger();

    /*成员变量*/
private:
    //日志文件路径
    QString mStrLogDir{};
    //日志文件
    QFile mLogFile{};
    //日志文件文本流
    QTextStream mLogStream{};
    //日志条目计数
    size_t muEntry = 0;

    static SLogger* global_logger;

    //[Me]
    QString Me = "SLogger";

    /*成员函数*/
public:

    //新建日志文件
    void newLog();
    //关闭日志记录
    void closeLog();
    //添加日志条目
    void addEntry(const QString& id, LogType type, const QString& entry);
    //添加分隔线
    void addSeperator();

    static SLogger* getLogger()
    {
        if(!global_logger)
            global_logger = new SLogger();
        return global_logger;
    }

};

template<typename T>
QString array2Str(T *pArray, size_t arraySize, char sep = '\n')
{
    QString str;
    QTextStream stream(&str);
    for(size_t i = 0; i < arraySize; ++i)
        stream << pArray[i] << sep;
    return str;
}

#endif // SLogger_H
