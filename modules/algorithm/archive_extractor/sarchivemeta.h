#ifndef SARCHIVEMETA_H
#define SARCHIVEMETA_H

#include "sarchiveentrymeta.h"
#include <vector>

//档案元数据类
class SArchiveMeta
{
    //成员变量
private:
    std::vector<SArchiveEntryMeta> mEntryMetaVec{};
    size_t muArchiveSize;

    //访问函数
public:
    const std::vector<SArchiveEntryMeta>& entries()const {return mEntryMetaVec;}
    std::vector<SArchiveEntryMeta>& entries() {return mEntryMetaVec;}

    size_t archiveSize()const {return muArchiveSize;}

    //修改函数
public:
    void setArchiveSize(size_t size) {muArchiveSize = size;}

    //构造函数与析构函数
public:
    explicit SArchiveMeta() {}
    explicit SArchiveMeta(const std::vector<SArchiveEntryMeta>& entryMetaVec, size_t archiveSize):
        mEntryMetaVec(entryMetaVec), muArchiveSize(archiveSize) {}
    virtual ~SArchiveMeta() {}

};

#endif // SARCHIVEMETA_H
