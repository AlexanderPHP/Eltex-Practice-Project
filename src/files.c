#include "files.h"
int dirsortbyname(const void *d1, const void *d2)
{
    const file_info *a = *(file_info **)d1;
    const file_info *b = *(file_info **)d2;
    if(S_ISDIR(a->mode))
    {
        if(S_ISDIR(b->mode))
            return strcmp(a->name, b->name);
        else
            return -1;
    }
    else if(S_ISDIR(b->mode))
        return 1;
    else
        return strcmp(a->name, b->name);
}

int scan_dir(const char *dirname, file_info ***namelist, int (*compar)(const void *, const void *))
{
    DIR *dirp;
    if ((dirp = opendir(dirname)) == NULL)
        return -1;
    int result = 0;
    int vsize = 0;
    char filepath[PATH_MAX];
    char dir[NAME_MAX+1];
    strncpy(filepath, dirname, PATH_MAX);
    file_info **names = NULL;
    struct dirent *d;
    struct stat stb;
    while((d = readdir(dirp)) != NULL)
    {
        if(!strcmp(d->d_name, ".")) continue;
        if(result == vsize)
        {
            if(vsize == 0)
                vsize = 10;
            else
                vsize *= 2;
            file_info **newv = realloc(names, vsize * sizeof(file_info *));
            if(newv == NULL)
                goto fail;
            names = newv;
        }
        file_info *p = malloc(sizeof(file_info));
        if(p == NULL)
            goto fail;
        strncat(filepath, "/", 1);
        strncat(filepath, d->d_name, PATH_MAX);
        if(lstat(filepath, &stb) < 0)
            goto fail;
        if(S_ISDIR(stb.st_mode))
            memcpy(p->name, filepath + strlen(filepath) - strlen(d->d_name) - 1, NAME_MAX + 1);
        else
            memcpy(p->name, d->d_name, NAME_MAX + 1);
        p->size = stb.st_size;
        p->mode = stb.st_mode;
        p->last_mod = stb.st_mtim;
        names[result++] = p;
        memset(&filepath[strlen(dirname)], '\0', 1);
        memset(&dir, '\0', 1);
    }
    closedir(dirp);
    if(result && compar != NULL)
        qsort(names, (size_t)result, sizeof(file_info *), compar);
    *namelist = names;
    return result;
    fail:
    while(result > 0)
        free(names[--result]);
    free(names);
    closedir(dirp);
    return -1;
}

void get_bytes_readable( char *readablesize, size_t bufsize, long filesize)
{
    const char *sizes[] = { "B", "K", "M", "G" };
    double len = filesize;
    int order = 0;
    while (len >= 1000 && ++order < 4)
    {
        len = len / 1000;
    }
    snprintf(readablesize, bufsize, "%.5g%s", len, sizes[order]); //segfault if fsize greater then 1TB
}
