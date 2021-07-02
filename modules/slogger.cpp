#include "slogger.h"
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QMessageBox>

SLogger* SLogger::global_logger = nullptr;

SLogger::SLogger()
{
    newLog();
}

SLogger::~SLogger()
{
    closeLog();
}

void SLogger::closeLog()
{
    if(!mLogFile.isOpen())
        return;
    //写入日志尾部
    mLogStream << WEBSITE
               << "# Log ended normally at " << QDateTime::currentDateTime().toString("yyyy_MM_dd-hh_mm_ss\n")
               << LOG_HEAD;
    //关闭文件
    mLogFile.close();
}

void SLogger::newLog()
{
    closeLog();
    //存放在我的文档sgis文件夹
    mStrLogDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/sgis";
    //检查sgif文件夹是否存在
    QDir dir;
    if(!dir.exists(mStrLogDir))
        dir.mkdir(mStrLogDir);
    //获取当前系统时间
    QString curDateTime = QDateTime::currentDateTime().toString("yyyy_MM_dd-hh_mm_ss");
    //打开并占用日志文件
    mLogFile.setFileName(mStrLogDir + LOG_NAME_PFX + curDateTime + ".txt");
    if(!mLogFile.open(QIODevice::WriteOnly | QIODevice::Text))
        QMessageBox::critical(nullptr,
                              tr("Fatal Error"),
                              tr((QString("S-GIS Logging System fails to create new log File in ") + mStrLogDir).toStdString().c_str()));
    mLogStream.setDevice(&mLogFile);
    //写入日志头
    mLogStream << LOG_HEAD
               << VER_STR
               << WEBSITE
               << "# Log began at " << curDateTime
               << "\n\n";
}

void SLogger::addEntry(const QString &id, LogType type, const QString& entry)
{
    if(!mLogFile.isOpen())
        return;
    //判断条目数是否超限
    if(muEntry >= LOG_MAX_ENTRY_COUNT)
        newLog();

    /*-----写入日志条目-----*/
	QString strLogLine;
	QTextStream textStream(&strLogLine);
	textStream.setFieldWidth(0);
    //输出时间信息和条目创建者
	textStream << QDateTime::currentDateTime().toString("yyyy_MM_dd-hh_mm_ss")
               << "\n[" << id << "]\t";
    //输出条目号码
	textStream.setFieldWidth((int)log10(LOG_MAX_ENTRY_COUNT) + 1);
	textStream.setFieldAlignment(QTextStream::FieldAlignment::AlignRight);
	textStream.setPadChar('0');
	textStream << muEntry++;
    //输出条目类型和主体内容
	textStream.setFieldWidth(0);
	textStream << '\n';
    switch (type)
    {
    case DebugInfo:
		textStream << "Debug_Info : ";
        break;
    case UserAction:
		textStream << "User_Action : ";
        break;
    case LocalError:
		textStream << "Local_Error : ";
        break;
    case RemoteError:
		textStream << "Remote_Error : ";
        break;
    case RunningStatus:
		textStream << "Running_Status : ";
        break;
    default:
        break;
    }
	textStream << entry ;
	mLogStream << strLogLine << "\n\n";
	//发送添加日志信号
	emit signalAddEntry(strLogLine);
}

void SLogger::addSeperator()
{
    if(!mLogFile.isOpen())
        return;
    mLogStream << "\n***************\n";
}


