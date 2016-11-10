#ifndef TUI_H
#define TUI_H
#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
#endif

#include <panel.h>
#include <menu.h>
#include <form.h>
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

typedef struct _cursed_window
{
    WINDOW *decoration;
    WINDOW *overlay;
    PANEL *panel;
} cursed_window;

typedef struct _tab_window
{
    cursed_window *win;
    MENU *menu;
    ITEM **items;
    char **str_items;
    file_info **files;
    int items_num;
    char path[PATH_MAX - 1];
} tab_window;


void tui_init();
cursed_window *tui_new_win(int sy, int sx, int h, int w, int bckg);
void tui_del_win(tab_window *win);
void tui_make_menu (tab_window *tab, void (on_item_selected)(tab_window *, const char *));
void tui_destroy_menu (tab_window *tab);
#endif