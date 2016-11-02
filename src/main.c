#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "tui.h"
#include "files.h"

void on_item_selected(tab_window *win)
{
    char item[NAME_MAX];
    int n;

    strncpy(item, win->files[item_index(current_item(win->menu))]->name, NAME_MAX);
    if(strncmp(item, "/.", NAME_MAX) == 0) return;
    if(strncmp(item, "/..", NAME_MAX) == 0 && strncmp(win->path, "/", PATH_MAX) != 0)
    {
        memset(&win->path[strlen(win->path) - 1], '\0', 1);
        memset(&win->path[strrchr(win->path, '/') - win->path], '\0', 1); //don't ask about it...
    }
    else
    {
        if(S_ISDIR(win->files[item_index(current_item(win->menu))]->mode))
            strncat(win->path, item, NAME_MAX);
    }
    mvwhline(win->decoration, 1, 1, ' ', getmaxx(win->decoration) - 2);

    if(strlen(win->path) > (size_t)COLS / 2 - 5)
        mvwprintw(win->decoration, 1, 1, "...%s", win->path + strlen(win->path) - COLS / 2 + 5);
    else
        mvwprintw(win->decoration, 1, 1, "%s", win->path);

    if(!strcmp(win->path, ""))
        strncat(win->path, "/", 1);

    n = scan_dir(win->path, &win->files, dirsortbyname);
    if (n < 0)
    {
        mvwhline(win->decoration, 1, 1, ' ', getmaxx(win->decoration) - 2);
        mvwprintw(win->decoration, 1, 1, "%s", "Permission denied or not a directory!");
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
    tab_window *ltab;
    tab_window *rtab;
    int ch;
    void (*p)(tab_window *);
    tab_window *active_tab;
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
                active_tab = (tab_window *) panel_userptr(active_tab->panel);
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
                p = (void (*)(tab_window *))(uintptr_t)item_userptr(current_item(active_tab->menu));
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