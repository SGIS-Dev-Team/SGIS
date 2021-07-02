#ifndef SARCHIVEEXTRACTOR_H
#define SARCHIVEEXTRACTOR_H
#include <QStringList>
#include <QObject>
#include <archive.h>
#include <archive_entry.h>
#include <QFileInfo>
#include "sarchivemeta.h"
#include <QThread>

#define MAX_FILENAME_LEN 1024

class SArchiveExtractor;
class SArchiveProgress;

//档案进度信息结构
class SArchiveProgress
{
public:
    SArchiveExtractor* mpExtractor;
    struct archive* mpArchive;
    struct archive_entry* mpEntry;

    explicit SArchiveProgress() {}
    explicit SArchiveProgress(SArchiveExtractor* _pExtractor, archive* _pArchive, archive_entry* _pEntry):
        mpExtractor(_pExtractor), mpArchive(_pArchive), mpEntry(_pEntry) {}
    virtual ~SArchiveProgress() {}
};


//-----------------------------------
//              档案解压器
//          SArchiveExtractor
//  该类为多线程设计，提供解压档案和预览档案条目
//的功能。
//
//  槽函数doExtract和doReadMeta的第一个参数
//[pExtractorThread]用于标识执行解压的线程
//若传入的线程指针和当前线程指针不一致，则不做
//任何操作，这是为了一个信号连接到多个解压线程
//的槽函数时能够区分设计的。
//
//
//-----------------------------------

class SArchiveExtractor : public QObject
{
    Q_OBJECT

    /*-----构造函数与析构函数-----*/
public:
    explicit SArchiveExtractor() {}
    virtual ~SArchiveExtractor() {}

    /*-----信号-----*/
signals:
    //解压完成
    void extractComplete(QThread* pExtractorThread, const QString& strExtractPath, int flag);
    //预览完成
    void archiveMetaRead(QThread* pExtractorThread, const SArchiveMeta& metaList, int flag);
    //解压进度条更新
    void extractProgressUpdated(QThread* pExtractorThread, int progress);
    //预览进度条更新
    void readArchiveMetaProgressUpdated(QThread* pExtractThread, int progress);

    /*-----槽函数-----*/
public slots:
    //解压
    void doExtract(QThread* pExtractorThread, const QString& strArchivePath, const QString& strExtractDir);
    //预览
    void doReadMeta(QThread* pExtractorThread, const QString& strArchivePath);

    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
protected:
    //支持的档案文件后缀名
    static QStringList strListValidFileSuffix;

    /*-----成员函数-----*/
public:
    //[访问函数]

    //[修改函数]

    //检查是否为支持的档案类型
    static bool isValidArchive(const QFileInfo& fileinfo);
    //获取支持档案类型的文件过滤器列表(如"*.gz","*.zip"等)
    static QStringList validArchiveNameFilters();

    //[功能函数]
private:
    //在两个档案之间拷贝数据
    static int _copyData(struct archive* pArchiveRead, struct archive* pArchiveWrite);

    //发射解压进度更新信号
    void _updateExtractProgress(int progress) {emit extractProgressUpdated(QThread::currentThread(), progress);}
    //发射预览进度更新信号
    void _updateReadMetaProgress(int progress) {emit readArchiveMetaProgressUpdated(QThread::currentThread(), progress);}

    //解压进度条回调函数
    static void _extractProgressCallBackFunc(void* pIndicator);
    //预览进度条回调函数
    static void _readMetaProgressCallBackFunc(void* pIndicator);

    //计算解压进度
    static int _calcProgress(SArchiveProgress* pIndicator);
};

#endif // SARCHIVEEXTRACTOR_H
