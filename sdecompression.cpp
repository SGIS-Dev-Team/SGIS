#include "sdecompression.h"
#include <archive.h>
#include <archive_entry.h>
#include <QStringList>
#include <QFile>
#include <QDir>
Sdecompression::Sdecompression(QString strZipFilePath, QString strUnZipFileFolder)
	: m_strZipFilePath(strZipFilePath)
	, m_strUnZipFileFolder(strUnZipFileFolder)
{
	if (!m_strUnZipFileFolder.isEmpty())
	{
		QDir(m_strUnZipFileFolder).mkpath(m_strUnZipFileFolder);
	}
}

bool Sdecompression::unTar()
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

QStringList Sdecompression::preview()
{
	QStringList lstFileNames;
	struct archive *a;
	struct archive_entry *entry;
	int r;

	a = archive_read_new();
	archive_read_support_filter_all(a);
	archive_read_support_format_all(a);

	auto filePath = m_strZipFilePath.toUtf8();
	r = archive_read_open_filename(a, filePath.data(), 10240); // Note 1
	if (r != ARCHIVE_OK)
	{
		return QStringList();
	}
	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		auto szFileItem = archive_entry_pathname(entry);
		lstFileNames.append(QString::fromUtf8(szFileItem));
		archive_read_data_skip(a);  // Note 2
	}
	r = archive_read_free(a);  // Note 3
	if (r != ARCHIVE_OK)
	{
		return QStringList();
	}
	return lstFileNames;
}

QString Sdecompression::unTarFile(QString strTarFilePath)
{

	struct archive *a = archive_read_new();
	archive_read_support_compression_all(a);
	archive_read_support_format_raw(a);
	auto filepath = strTarFilePath.toUtf8();
	auto r = archive_read_open_filename(a, filepath.data(), 16384);
	auto strTempFile = getTempFilePath();
	if (r != ARCHIVE_OK) {
		archive_read_free(a);
		return strTempFile;
	}
	for (;;) {
		struct archive_entry *entry = nullptr;
		r = archive_read_next_header(a, &entry);
		if (r == ARCHIVE_EOF)
			break;
		if (r != ARCHIVE_OK) {
			archive_read_free(a);
			return strTempFile;
		}
		auto szFileItemName = archive_entry_pathname_utf8(entry);
		auto m = archive_entry_mode(entry);
		QFile file(strTempFile);
		file.open(QIODevice::WriteOnly | QIODevice::Truncate);
		for (;;) {
			char buff[1024] = { 0 };
			auto size = archive_read_data(a, buff, 1023);
			if (size < 0) {
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

bool Sdecompression::unZlibFiles(QString strZlibFilePath, QString strOutFolder)
{
	struct archive *a = archive_read_new();
	archive_read_support_compression_all(a);
	//archive_read_support_format_raw(a);
	archive_read_support_format_all(a);
	auto filepath = strZlibFilePath.toUtf8();
	auto r = archive_read_open_filename(a, filepath.data(), 16384);

	if (r != ARCHIVE_OK) {
		archive_read_free(a);
		return false;
	}
	for (;;) {
		struct archive_entry *entry = nullptr;
		r = archive_read_next_header(a, &entry);
		if (r == ARCHIVE_EOF)
			break;
		if (r != ARCHIVE_OK) {
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
		for (;;) {
			char buff[1024] = { 0 };
			auto size = archive_read_data(a, buff, 1023);
			if (size < 0) {
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

QString Sdecompression::getTempFilePath()
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
