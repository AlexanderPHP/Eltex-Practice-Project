#ifndef TEXTEDITOR_FILES_H
#define TEXTEDITOR_FILES_H
#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
#endif

#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <linux/limits.h>
#include <stdlib.h>

#include "tui.h"

int dirsortbyname(const void *d1, const void *d2);
int scan_dir(const char *dirname, file_info ***namelist, int (*compar)(const void *, const void *));
void get_bytes_readable( char *readablesize, size_t bufsize, long filesize);
#endif //TEXTEDITOR_FILES_H
