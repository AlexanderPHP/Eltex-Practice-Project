#include "global.h"

int dirsortbyname(const void *d1, const void *d2)
{
    const struct file_info *a = *(struct file_info **)d1;
    const struct file_info *b = *(struct file_info **)d2;
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

int scan_dir(const char *dirname, struct file_info ***namelist, int (*compar)(const void *, const void *))
{
    DIR *dirp;
    if ((dirp = opendir(dirname)) == NULL)
        return -1;
    int result = 0;
    int vsize = 0;
    char filepath[PATH_MAX];
    char dir[NAME_MAX+1];
    strncpy(filepath, dirname, PATH_MAX);
    struct file_info **names = NULL;
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
            struct file_info **newv = (struct file_info **)realloc(names, vsize * sizeof(struct file_info *));
            if(newv == NULL)
                goto fail;
            names = newv;
        }
        struct file_info *p = (struct file_info *)malloc(sizeof(struct file_info));
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
        qsort(names, (size_t)result, sizeof(struct file_info *), compar);
    *namelist = names;
    return result;
    fail:
    while(result > 0)
        free(names[--result]);
    free(names);
    closedir(dirp);
    return -1;
}

void on_item_selected(cursed *win)
{
    char item[NAME_MAX];
    int n;

    strncpy(item, win->files[item_index(current_item(win->menu))]->name, NAME_MAX);
    if(strncmp(item, "/.", NAME_MAX) == 0) return;
    if(strncmp(item, "/..", NAME_MAX) == 0 && strncmp(win->path, "/", PATH_MAX) != 0)
    {
        memset(&win->path[strlen(win->path) - 1], '\0', 1);
        memset(&win->path[strrchr(win->path, '/') - win->path + 1], '\0', 1); //don't ask about it...
    }
    else
    {
        strncat(win->path, item, NAME_MAX);
        if(!S_ISDIR(win->files[item_index(current_item(win->menu))]->mode))
            strncat(win->path, "/", 1);
    }
    mvwhline(win->decoration, 1, 1, ' ', getmaxx(win->decoration) - 2);

    if(strlen(win->path) > (size_t)COLS / 2 - 5)
        mvwprintw(win->decoration, 1, 1, "...%s", win->path + strlen(win->path) - COLS / 2 + 5);
    else
        mvwprintw(win->decoration, 1, 1, "%s", win->path);
    n = scan_dir(win->path, &win->files, dirsortbyname);

    if (n < 0)
    {
        mvwhline(win->decoration, 1, 1, ' ', getmaxx(win->decoration) - 2);
//        mvwprintw(win->decoration, 1, 1, "%s", "Permission denied or not a directory!");
        mvwprintw(win->decoration, 1, 1, "%s", win->path);
        memset(&win->path[strlen(win->path) - 1], '\0', 1);
        memset(&win->path[strrchr(win->path, '/') - win->path + 1], '\0', 1); //don't ask about it...
    }
    else
    {
        tui_destroy_menu(win);
        win->items_num = n;
        tui_make_menu(win, on_item_selected);
    }
}

int main(void)
{
    if ( tui_init( ))
    {
        puts("error");
        return 1;
    }

    int ch;
    void (*p)(cursed *);
    cursed *active_tab;
    ltab = tui_new_win(0, 0, LINES, COLS / 2 , " ", 2);
    rtab = tui_new_win(0, COLS / 2, LINES, COLS / 2, " ", 2);

    mvwprintw(ltab->decoration, 1,1, "/");
    mvwprintw(rtab->decoration, 1,1, "/");

    set_panel_userptr(ltab->panel, rtab);
    set_panel_userptr(rtab->panel, ltab);

    top_panel(ltab->panel);

    active_tab = ltab;

    strncpy(ltab->path, "/home/alexander/.cache", PATH_MAX - 1);
    strncpy(rtab->path, "/", PATH_MAX - 1);
    ltab->items_num = scan_dir(ltab->path, &ltab->files, dirsortbyname);
    rtab->items_num = scan_dir(rtab->path, &rtab->files, dirsortbyname);

    tui_make_menu(ltab, on_item_selected);
    tui_make_menu(rtab, on_item_selected);

    touchwin(panel_window(active_tab->panel));
    update_panels();
    doupdate();

    while ((ch = getch()) != KEY_F(12))
    {
        switch (ch)
        {
            case '\t':
                active_tab = (cursed *) panel_userptr(active_tab->panel);
                top_panel(active_tab->panel);
                break;
            case KEY_DOWN:
                menu_driver(active_tab->menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(active_tab->menu, REQ_UP_ITEM);
                break;
            case KEY_NPAGE:
                menu_driver(active_tab->menu, REQ_SCR_DPAGE);
                break;
            case KEY_PPAGE:
                menu_driver(active_tab->menu, REQ_SCR_UPAGE);
                break;
            case KEY_HOME:
                menu_driver(active_tab->menu, REQ_FIRST_ITEM);
                break;
            case KEY_END:
                menu_driver(active_tab->menu, REQ_LAST_ITEM);
                break;

            case 13: // enter
                p = (void (*)(cursed *))(uintptr_t)item_userptr(current_item(active_tab->menu));
                p(active_tab);
                break;
            default:break;
        }
        touchwin(panel_window(active_tab->panel));
        update_panels();
        doupdate();
    }
    for(int i = 0; i < ltab->items_num ; ++i)
    {
        free(ltab->files[i]);
    }
    free(ltab->files);
    for(int i = 0; i < rtab->items_num; ++i)
    {
        free(rtab->files[i]);
    }
    free(rtab->files);
    tui_destroy_menu(rtab);
    tui_destroy_menu(ltab);
    tui_del_win(ltab);
    tui_del_win(rtab);
    endwin();
}