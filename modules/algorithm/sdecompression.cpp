#include "sdecompression.h"

Sdecompression::Sdecompression()
{
    unpack_input_path = nullptr;
    unpack_output_path = nullptr;
}


void Sdecompression::setPath(const char * path)
{
    unpack_input_path = QString(path);


    extract(path, unpack_output_path);
}

const char* Sdecompression::getPath()
{
    const char* out = unpack_output_path.c_str();
    return out;
}

void Sdecompression::preview(const char * filename)
{
    char buff[8192];
    ssize_t len;
    int r;
    mode_t m;
    struct archive *ina;
    struct archive *outa;
    struct archive_entry *entry;

    /* Read an archive from stdin, with automatic format detection. */
    ina = archive_read_new();
    if (ina == NULL)
        die("Couldn't create archive reader.");
    if (archive_read_support_filter_all(ina) != ARCHIVE_OK)
        die("Couldn't enable decompression");
    if (archive_read_support_format_all(ina) != ARCHIVE_OK)
        die("Couldn't enable read formats");
    if (archive_read_open_fd(ina, 0, 10240) != ARCHIVE_OK)
        die("Couldn't open input archive");

    /* Write an uncompressed ustar archive to stdout. */
    outa = archive_write_new();
    if (outa == NULL)
        die("Couldn't create archive writer.");
    if (archive_write_set_compression_none(outa) != ARCHIVE_OK)
        die("Couldn't enable compression");
    if (archive_write_set_format_ustar(outa) != ARCHIVE_OK)
        die("Couldn't set output format");
    if (archive_write_open_fd(outa, 1) != ARCHIVE_OK)
        die("Couldn't open output archive");

    if ((r = archive_read_open_filename(ina, filename, 10240)))
        return (0);


    /* Examine each entry in the input archive. */
    while ((r = archive_read_next_header(ina, &entry)) == ARCHIVE_OK) {
        fprintf(stderr, "%s: ", archive_entry_pathname(entry));

        /* Skip anything that isn't a regular file. */
        if (!S_ISREG(archive_entry_mode(entry))) {
            fprintf(stderr, "skipped\n");
            continue;
        }

        /* Make everything owned by root/wheel. */
        archive_entry_set_uid(entry, 0);
        archive_entry_set_uname(entry, "root");
        archive_entry_set_gid(entry, 0);
        archive_entry_set_gname(entry, "wheel");

        /* Make everything permission 0744, strip SUID, etc. */
        m = archive_entry_mode(entry);
        archive_entry_set_mode(entry, (m & ~07777) | 0744);

        /* Copy input entries to output archive. */
        if (archive_write_header(outa, entry) != ARCHIVE_OK)
            die("Error writing output archive");
        if (archive_entry_size(entry) > 0) {
            len = archive_read_data(ina, buff, sizeof(buff));
            while (len > 0) {
                if (archive_write_data(outa, buff, len) != len)
                    die("Error writing output archive");
                len = archive_read_data(ina, buff, sizeof(buff));
            }
            if (len < 0)
                die("Error reading input archive");
        }
        fprintf(stderr, "copied\n");
    }
    if (r != ARCHIVE_EOF)
        die("Error reading archive");
    /* Close the archives.  */
    if (archive_read_free(ina) != ARCHIVE_OK)
        die("Error closing input archive");
    if (archive_write_free(outa) != ARCHIVE_OK)
        die("Error closing output archive");
    return (0);
}

int Sdecompression::copy_data(struct archive* ar, struct archive* aw)
{
    int r;
    const void* buff;
    size_t size;
    la_int64_t offset;

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
            return (ARCHIVE_OK);
        if (r < ARCHIVE_OK)
            return (r);
        r = archive_write_data_block(aw, buff, size, offset);
        if (r < ARCHIVE_OK) {
            fprintf(stderr, "%s\n", archive_error_string(aw));
            return (r);
        }
    }
}

void Sdecompression::extract(const char* filename, QString outDirName)
{
    struct archive* a;
    struct archive* ext;
    struct archive_entry* entry;
    int flags;
    int r;

    /* Select which attributes we want to restore. */
    flags = ARCHIVE_EXTRACT_TIME;
    flags |= ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;

    a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_compression_all(a);
    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);
    if ((r = archive_read_open_filename(a, filename, 10240)))
        exit(1);
    for (;;) {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
            break;

        //下面几行是我加的
        char outputFilePath[MAX_FILENAME_LEN] = "";
        const char* tmp = outDirName.c_str();
        strcpy_s(outputFilePath, MAX_FILENAME_LEN, tmp);
        strcat_s(outputFilePath, MAX_FILENAME_LEN, "/");
        strcat_s(outputFilePath, MAX_FILENAME_LEN, archive_entry_pathname(entry));

        archive_entry_set_pathname(entry, outputFilePath);

        unpack_output_path = outputFilePath;

        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(a));
        if (r < ARCHIVE_WARN)
            exit(1);
        r = archive_write_header(ext, entry);
        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(ext));
        else if (archive_entry_size(entry) > 0) {
            r = copy_data(a, ext);
            if (r < ARCHIVE_OK)
                fprintf(stderr, "%s\n", archive_error_string(ext));
            if (r < ARCHIVE_WARN)
                exit(1);
        }
        r = archive_write_finish_entry(ext);
        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(ext));
        if (r < ARCHIVE_WARN)
            exit(1);
    }
    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
    exit(0);
}


void Sdecompression::die(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    exit(1);
}
