#ifndef SARCHIVEEXTRACTOR_H
#define SARCHIVEEXTRACTOR_H
#include <QStringList>
#include <QObject>
#include <archive.h>
#include <archive_entry.h>
#include <QFileInfo>

#define MAX_FILENAME_LEN 1024

//档案条目元数据
class SArchiveEntryMeta
{
private:
    QString mStrEntryPath;          //条目路径（档案内部路径）
    size_t  muEntrySize;            //解压大小
    QString mStrLastModifiedTime;   //修改时间

public:
    //访问函数
    const QString& entryPath()const {return mStrEntryPath;}
    const QString& lastModifiedTime()const {return mStrLastModifiedTime;}
    size_t size()const {return muEntrySize;}
    //修改函数
    void setEntryPath(const QString& path) {mStrEntryPath = path;}
    void setLastModifiedTime(const QString& time) {mStrLastModifiedTime = time;}
    void setUnpackSize(size_t sz) {muEntrySize = sz;}

public:
    explicit SArchiveEntryMeta(const QString& _entryPath, size_t _size, const QString& _lastModifiedTime):
        mStrEntryPath(_entryPath), muEntrySize(_size), mStrLastModifiedTime(_lastModifiedTime) {}
    virtual ~SArchiveEntryMeta() {}
};


//档案解压器
class SArchiveExtractor : public QObject
{
    Q_OBJECT

    /*-----构造函数与析构函数-----*/
public:
    explicit SArchiveExtractor() {}
    virtual ~SArchiveExtractor() {}

    /*-----信号-----*/
signals:
    void extractComplete(QString strExtractPath, int flag);
    void readArchiveMetaComplete(QList<SArchiveEntryMeta> metaList, int flag);

    /*-----槽函数-----*/
private slots:
    void doExtract(QString strArchivePath, QString strExtractDir);
    void doReadMeta(QString strArchivePath);

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
    //在两个档案之间拷贝数据
    static int copyData(struct archive* pArchiveRead, struct archive* pArchiveWrite);

};

#endif // SARCHIVEEXTRACTOR_H
