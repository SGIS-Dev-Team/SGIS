#ifndef SARCHIVEEXTRACTOR_H
#define SARCHIVEEXTRACTOR_H
#include <QStringList>
#include <QObject>
#include <archive.h>
#include <archive_entry.h>

#define MAX_FILENAME_LEN 1024

//档案条目元数据
class SArchiveEntryMeta
{
private:
    QString mStrEntryPath;          //条目路径
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

class SArchiveExtractor : public QObject
{
    Q_OBJECT

    /*-----构造函数与析构函数-----*/
public:
    explicit SArchiveExtractor() {}
    virtual ~SArchiveExtractor() {}

public:
    bool unTar();
    QStringList preview(); //预览

private:
    QString getTempFilePath();

    /*-----信号-----*/
signals:
    void extractComplete(QString strExtractPath, int flag);
    void readArchiveMetaComplete(QList<SArchiveEntryMeta> metaList, int flag);

    /*-----槽函数-----*/
private slots:
    void doExtract(QString strArchivePath, QString strExtractPath);
    void doReadMeta(QString strArchivePath);

    /*-----属性-----*/
protected:

    /*-----成员变量-----*/
protected:

    /*-----成员函数-----*/
public:
    //[访问函数]

    //[修改函数]

    //[功能函数]

    /*-----UI-----*/
private:
};

#endif // SARCHIVEEXTRACTOR_H
