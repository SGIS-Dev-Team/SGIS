#ifndef SDECOMPRESSION_H
#define SDECOMPRESSION_H
#include <QString>
#include <archive.h>
#include <archive_entry.h>

#define MAX_FILENAME_LEN 1024


class Sdecompression
{
public:
    Sdecompression();

    void setPath(const char * path); //传入路径

    const char* getPath(); //获取解压路径

    void preview(const char * filename); //预览

private:
    QString unpack_input_path;

    static QString unpack_output_path; //类静态变量

    static int copy_data(struct archive* ar, struct archive* aw);

    static void extract(const char* filename, QString outDirName);

    static void die(char *fmt, ...);
};

#endif // UNPACK_H
