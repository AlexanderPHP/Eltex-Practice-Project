#include "event.h"

void event_handler(tab_window *win, const char* event)
{
    if(!strcmp(event, "select"))
        select_item(win);
    else if(!strcmp(event, "remove"))
        remove_item(win);
    else if(!strcmp(event, "rename"))
        rename_item(win);
}

void select_item(tab_window *tab)
{
    char item[NAME_MAX];
    int n;

    strncpy(item, tab->files[item_index(current_item(tab->menu))]->name, NAME_MAX);
    if(!strncmp(item, "/.", NAME_MAX))
        return;
    if(!strncmp(item, "/..", NAME_MAX))
        memset(&tab->path[strrchr(tab->path, '/') - tab->path], '\0', 1);
    else if(S_ISDIR(tab->files[item_index(current_item(tab->menu))]->mode))
        strncat(tab->path, item, NAME_MAX);

    if(!strcmp(tab->path, ""))
        strncat(tab->path, "/", 1);

    n = scan_dir(tab->path, &tab->files, dirsortbyname);
    mvwhline(tab->win->decoration, 1, 1, ' ', getmaxx(tab->win->decoration) - 2);
    if (n >= 0)
    {
        if(strlen(tab->path) > (size_t)COLS / 2 - 5)
            mvwprintw(tab->win->decoration, 1, 1, "...%s", tab->path + strlen(tab->path) - COLS / 2 + 5);
        else
            mvwprintw(tab->win->decoration, 1, 1, "%s", tab->path);
        tui_destroy_menu(tab);
        tab->items_num = n;
        tui_make_menu(tab, event_handler);
    }
    else
    {
        mvwprintw(tab->win->decoration, 1, 1, "Permission denied!");
        memset(&tab->path[strrchr(tab->path, '/') - tab->path + 1], '\0', 1);
    }
}

void remove_item(tab_window *win)
{
    return;
}

void rename_item(tab_window *win)
{
    return;
}