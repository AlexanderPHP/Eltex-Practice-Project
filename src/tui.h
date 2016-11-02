#ifndef TUI_H
#define TUI_H

#include <panel.h>
#include <menu.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

typedef struct _file_info{
    char name[NAME_MAX + 1];
    off_t size;
    mode_t mode;
    struct timespec last_mod;
} file_info;

typedef struct _tab_window
{
    WINDOW *decoration;
    WINDOW *overlay;
    PANEL *panel;
    MENU *menu;
    ITEM **items;
    char **str_items;
    file_info **files;
    int items_num;
    char path[PATH_MAX - 1];
} tab_window;


int tui_init(void);
tab_window *tui_new_win(int sy, int sx, int h, int w, char *label, int bckg);
void tui_del_win(tab_window *win);
void tui_win_label(WINDOW *win, char *string, int attrib);
void tui_make_menu (tab_window *win, void (on_item_selected)(tab_window *));
void tui_destroy_menu (tab_window *win);
#endif