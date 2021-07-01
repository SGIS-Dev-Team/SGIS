#include "sarchiveextractor.h"
#include <archive.h>
#include <archive_entry.h>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QDateTime>

bool SArchiveExtractor::unTar()
{
    auto strZibFile = unTarFile(m_strZipFilePath);
    if (!QFile::exists(strZibFile))
    {
        return false;
    }
    auto bRetValue = unZlibFiles(strZibFile, m_strUnZipFileFolder);
    QFile::remove(strZibFile);
    return bRetValue;
}

QStringList SArchiveExtractor::preview()
{

}

QString SArchiveExtractor::unTarFile(QString strTarFilePath)
{
    struct archive* a = archive_read_new();
    archive_read_support_compression_all(a);
    archive_read_support_format_raw(a);
    auto filepath = strTarFilePath.toUtf8();
    auto r = archive_read_open_filename(a, filepath.data(), 16384);
    auto strTempFile = getTempFilePath();
    if (r != ARCHIVE_OK)
    {
        archive_read_free(a);
        return strTempFile;
    }
    for (;;)
    {
        struct archive_entry* entry = nullptr;
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
            break;
        if (r != ARCHIVE_OK)
        {
            archive_read_free(a);
            return strTempFile;
        }
        auto szFileItemName = archive_entry_pathname_utf8(entry);
        auto m = archive_entry_mode(entry);
        QFile file(strTempFile);
        file.open(QIODevice::WriteOnly | QIODevice::Truncate);
        for (;;)
        {
            char buff[1024] = { 0 };
            auto size = archive_read_data(a, buff, 1023);
            if (size < 0)
            {
                /* ERROR */
            }
            if (size == 0)
                break;
            file.write(buff, size);
        }
        file.close();
    }
    archive_read_free(a);

    return strTempFile;
}

bool SArchiveExtractor::unZlibFiles(QString strZlibFilePath, QString strOutFolder)
{
    struct archive* a = archive_read_new();
    archive_read_support_compression_all(a);
    //archive_read_support_format_raw(a);
    archive_read_support_format_all(a);
    auto filepath = strZlibFilePath.toUtf8();
    auto r = archive_read_open_filename(a, filepath.data(), 16384);

    if (r != ARCHIVE_OK)
    {
        archive_read_free(a);
        return false;
    }
    for (;;)
    {
        struct archive_entry* entry = nullptr;
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
            break;
        if (r != ARCHIVE_OK)
        {
            archive_read_free(a);
            return false;
        }
        auto szFileItemName = archive_entry_pathname_utf8(entry);
        auto strFileItemName = QString::fromUtf8(szFileItemName);
        auto strItemFilePath = strOutFolder + "/" + strFileItemName;
        auto m = archive_entry_mode(entry);
        if (strFileItemName.endsWith('/'))
        {
            QDir().mkpath(strItemFilePath);
            continue;
        }
        QFile file(strItemFilePath);
        file.open(QIODevice::WriteOnly | QIODevice::Truncate);
        for (;;)
        {
            char buff[1024] = { 0 };
            auto size = archive_read_data(a, buff, 1023);
            if (size < 0)
            {
                /* ERROR */
            }
            if (size == 0)
                break;
            file.write(buff, size);
        }
        file.close();
    }
    archive_read_free(a);

    return true;
}

QString SArchiveExtractor::getTempFilePath()
{
    auto strTempFolder = QDir::tempPath();
    for (auto i = 0; ; i++)
    {
        auto strFilePath = strTempFolder + QString("/x%0.tar").arg(i);
        if (!QFile::exists(strFilePath))
        {
            return strFilePath;
        }
    }
    return QString();
}

void SArchiveExtractor::doExtract(QString strArchivePath, QString strExtractPath)
{

}

void SArchiveExtractor::doReadMeta(QString strArchivePath)
{
    //描述数据列表
    QList<SArchiveEntryMeta> metaList;

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
        emit readArchiveMetaComplete({}, flag);
        return;
    }

    //循环读取每个条目
    while (archive_read_next_header(pArchive, &pEntry) == ARCHIVE_OK)
    {
        QString strEntryPath = archive_entry_pathname(pEntry);
        size_t entrySize = archive_entry_size(pEntry);
        QDateTime modifTime(QDateTime::fromTime_t(archive_entry_mtime(pEntry)));
        QString strModifTime = modifTime.toString("yyyy/MM/dd hh:mm::ss");
        //构造元数据类
        metaList << SArchiveEntryMeta(strEntryPath, entrySize, strModifTime);
        //跳过读取条目数据
        archive_read_data_skip(pArchive);
    }

    //结束读取档案
    flag = archive_read_free(pArchive);

    emit readArchiveMetaComplete(metaList, flag);
}







