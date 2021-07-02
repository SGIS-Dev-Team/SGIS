#include "sarchiveextractor.h"
#include <archive.h>
#include <archive_entry.h>
#include <QStringList>
#include <QDateTime>
#include <QDir>

QStringList SArchiveExtractor::strListValidFileSuffix = {"gz", "zip"};

void SArchiveExtractor::doExtract(QThread* pExtractorThread, const QString& strArchivePath, const QString& strExtractDir)
{
    if (pExtractorThread != QThread::currentThread())
        return;

    //初始化档案指针
    struct archive* pArchiveRead = archive_read_new();
    struct archive* pArchiveWrite = archive_write_disk_new();
    int flag{};

    //支持所有格式
    archive_read_support_compression_all(pArchiveRead);
    archive_read_support_format_all(pArchiveRead);

    //路径字符串转换
    QByteArray strArchivePath_byte = strArchivePath.toUtf8();
    QString strExtractPath = (strExtractDir + "/" + QFileInfo(strArchivePath).fileName()).toUtf8();
    QDir().mkpath(strExtractPath);

    //打开文件
    int block_size = 10240;
    flag = archive_read_open_filename(pArchiveRead, strArchivePath_byte.constData(), block_size);
    if (flag != ARCHIVE_OK)
    {
        archive_read_free(pArchiveRead);
        archive_write_free(pArchiveWrite);
        emit extractComplete(pExtractorThread, {}, flag);
        return;
    }

    //设置进度回调函数
    SArchiveProgress progressIndicator(this, pArchiveRead, nullptr);
    archive_read_extract_set_progress_callback(pArchiveRead, SArchiveExtractor::_extractProgressCallBackFunc, (void*)&progressIndicator);

    //循环读取每个条目
    for (;;)
    {
        //读取下一个条目
        struct archive_entry* pEntry = nullptr;
        flag = archive_read_next_header(pArchiveRead, &pEntry);

        //到达文件尾或出现错误跳出循环
        if (flag == ARCHIVE_EOF || flag != ARCHIVE_OK)
            break;

        //更改条目路径
        QString entryPath = archive_entry_pathname(pEntry);
        QString extractedEntryPath = strExtractPath + "/" + entryPath;
        QByteArray extractedEntryPath_byte = extractedEntryPath.toUtf8();
        archive_entry_set_pathname(pEntry, extractedEntryPath_byte.constData());

        progressIndicator.mpEntry = pEntry;

        //写入条目数据标头
        flag = archive_write_header(pArchiveWrite, pEntry);
        if (flag != ARCHIVE_OK)
            break;

        //拷贝条目数据
        _copyData(pArchiveRead, pArchiveWrite);

        //结束条目
        flag = archive_write_finish_entry(pArchiveWrite);
    }

    archive_read_close(pArchiveRead);
    archive_read_free(pArchiveRead);

    archive_write_close(pArchiveWrite);
    archive_write_free(pArchiveWrite);

    emit extractComplete(pExtractorThread, strExtractDir, flag);
}

void SArchiveExtractor::doReadMeta(QThread* pExtractorThread, const QString& strArchivePath)
{
    if (pExtractorThread != QThread::currentThread())
        return;

    //描述数据列表
    std::vector<SArchiveEntryMeta> metaList;

    //档案对象
    struct archive* pArchive;
    //档案条目对象
    struct archive_entry* pEntry;
    //状态标识
    int flag;

    //构造档案对象
    pArchive = archive_read_new();
    archive_read_support_filter_all(pArchive);
    archive_read_support_format_all(pArchive);

    QByteArray strArchivePath_byte = strArchivePath.toUtf8();

    //区块尺寸
    int block_size = 10240;
    //打开文件
    flag = archive_read_open_filename(pArchive, strArchivePath_byte.data(), block_size);

    if (flag != ARCHIVE_OK)
    {
        archive_read_free(pArchive);
        emit archiveMetaRead(pExtractorThread, SArchiveMeta(), flag);
        return;
    }

    //循环读取每个条目
    while ((flag = archive_read_next_header(pArchive, &pEntry)) == ARCHIVE_OK)
    {
        QString strEntryPath = archive_entry_pathname(pEntry);
        size_t entrySize = archive_entry_size(pEntry);
        QDateTime modifTime(QDateTime::fromTime_t(archive_entry_mtime(pEntry)));
        QString strModifTime = modifTime.toString("yyyy/MM/dd hh:mm::ss");
        //构造元数据类
        metaList.emplace_back(strEntryPath, entrySize, strModifTime);
        //跳过读取条目数据
        archive_read_data_skip(pArchive);
    }

    //结束读取档案
    archive_read_close(pArchive);
    archive_read_free(pArchive);

    emit archiveMetaRead(pExtractorThread,
                         SArchiveMeta(metaList, QFileInfo(strArchivePath).size()),
                         flag);
}

bool SArchiveExtractor::isValidArchive(const QFileInfo& fileinfo)
{
    //检查是否为目录
    if (fileinfo.isDir())
        return false;
    //检查后缀名
    QString suffix = fileinfo.suffix();
    bool valid{false};
    foreach (const auto& validSuffix, strListValidFileSuffix)
        if (suffix.compare(validSuffix, Qt::CaseSensitivity::CaseInsensitive) == 0)
            valid = true;

    return valid;
}

QStringList SArchiveExtractor::validArchiveNameFilters()
{
    QStringList filterList;
    for (auto& suffix : strListValidFileSuffix)
        filterList.append("*." + suffix);

    return filterList;
}

int SArchiveExtractor::_copyData(struct archive* pArchiveRead, struct archive* pArchiveWrite)
{
    int flag;
    const void* dataBuffer;
    size_t dataSize;
    int64_t offset;

    for (;;)
    {
        flag = archive_read_data_block(pArchiveRead, &dataBuffer, &dataSize, &offset);

        if (flag == ARCHIVE_EOF)
            return (ARCHIVE_OK);
        if (flag != ARCHIVE_OK)
            return flag;

        flag = archive_write_data_block(pArchiveWrite, dataBuffer, dataSize, offset);

        if (flag != ARCHIVE_OK)
            return flag;
    }
}

int SArchiveExtractor::_calcProgress(SArchiveProgress* pProgressIndicator)
{
    struct archive* pArchive = pProgressIndicator->mpArchive;

    uint64_t comp, uncomp;
    int progress{0};

    if (pArchive != nullptr)
    {
        comp = archive_filter_bytes(pArchive, -1);
        uncomp = archive_filter_bytes(pArchive, 0);
        if (comp > uncomp)
            progress = 0;
        else
            progress = (int)((uncomp - comp) * 100 / uncomp);
    }

    return progress;
}

void SArchiveExtractor::_extractProgressCallBackFunc(void* pIndicator)
{
    SArchiveProgress* pProgressIndicator = static_cast<SArchiveProgress*>(pIndicator);
    pProgressIndicator->mpExtractor->_updateExtractProgress(_calcProgress(pProgressIndicator));
}

void SArchiveExtractor::_readMetaProgressCallBackFunc(void* pIndicator)
{
    SArchiveProgress* pProgressIndicator = static_cast<SArchiveProgress*>(pIndicator);
    pProgressIndicator->mpExtractor->_updateReadMetaProgress(_calcProgress(pProgressIndicator));
}







