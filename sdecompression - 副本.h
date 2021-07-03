#ifndef SDECOMPRESSION_H
#define SDECOMPRESSION_H
#include <QStringList>
#include <archive.h>
#include <archive_entry.h>

#define MAX_FILENAME_LEN 1024


class Sdecompression
{
public:
    Sdecompression(QString strZipFilePath, QString strUnZipFileFolder = QString());
public:
	bool unTar();
    QStringList preview(); //预览

private:
	QString unTarFile(QString strFilePath);
	bool unZlibFiles(QString strZlibFilePath, QString strOutFolder);

	QString getTempFilePath();
private:
	QString m_strZipFilePath;
	QString m_strUnZipFileFolder;
};

#endif // UNPACK_H
