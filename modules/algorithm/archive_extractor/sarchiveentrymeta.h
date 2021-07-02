#ifndef SARCHIVEENTRYMETA_H
#define SARCHIVEENTRYMETA_H

#include <QString>

//档案条目元数据类
class SArchiveEntryMeta
{
    //成员变量
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

    //构造函数与析构函数
public:
    explicit SArchiveEntryMeta() {}
    explicit SArchiveEntryMeta(const QString& _entryPath, size_t _size, const QString& _lastModifiedTime):
        mStrEntryPath(_entryPath), muEntrySize(_size), mStrLastModifiedTime(_lastModifiedTime) {}
    virtual ~SArchiveEntryMeta() {}
};

#endif // SARCHIVEENTRYMETA_H
