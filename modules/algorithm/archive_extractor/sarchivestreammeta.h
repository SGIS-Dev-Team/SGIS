#ifndef SARCHIVESTREAMMETA_H
#define SARCHIVESTREAMMETA_H

#include "sarchivemeta.h"

//档案数据流描述类
class SArchiveStreamMeta
{
    //成员变量
private:
    //档案元数据
    SArchiveMeta mArchiveMeta{};
    //档案路径
    QString mArchivePath{};
    //档案解压路径
    QString mExtractDir{};
    //解压进度:-1为未开始解压，0~100为正在解压范围
    int mExtractProgress{-1};

    //访问函数
public:
    const SArchiveMeta& archiveMeta()const {return mArchiveMeta;}
    const QString& archivePath()const {return mArchivePath;}
    const QString& extractDir()const {return mExtractDir;}
    int extractProgress()const {return mExtractProgress;}
    bool isExtracted()const {return mExtractProgress == 100;}

    //修改函数
public:
    void setArchiveMeta(const SArchiveMeta& meta) {mArchiveMeta = meta;}
    void setArchivePath(const QString& path) {mArchivePath = path;}
    void setExtractDir(const QString& dir) {mExtractDir = dir;}
    void setExtractProgress(int progress) {mExtractProgress = progress;}

    //构造与析构函数
public:
    explicit SArchiveStreamMeta() {}
    explicit SArchiveStreamMeta(
        const SArchiveMeta& archiveMeta,
        const QString& archivePath,
        const QString& extractDir,
        int extractProgress):
        mArchiveMeta(archiveMeta),
        mArchivePath(archivePath),
        mExtractDir(extractDir),
        mExtractProgress(extractProgress)
    {}
    virtual ~SArchiveStreamMeta() {}
};

#endif // SARCHIVESTREAMMETA_H
