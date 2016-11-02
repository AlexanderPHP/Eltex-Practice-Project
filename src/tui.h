#ifndef TUI
#define TUI
#include <panel.h>
#include <form.h>
#include <menu.h>
#include <linux/limits.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
struct _cursed_window
{
    WINDOW *decoration;
    WINDOW *overlay;
    PANEL *panel;
    MENU *menu;
    ITEM **items;
    char **str_items;
    struct file_info **files;
    int items_num;
    char path[PATH_MAX - 1];
};
typedef struct _cursed_window cursed;
struct file_info{
    char name[NAME_MAX + 1];
    off_t size;
    mode_t mode;
    struct timespec last_mod;
};
int tui_init(void);
cursed *tui_new_win(int sy, int sx, int h, int w, char *label, int bckg);
void tui_del_win(cursed *win);
void tui_win_label(WINDOW *win, char *string, int attrib);
void tui_make_menu (cursed *win, void (on_item_selected)(cursed *));
void tui_destroy_menu (cursed *win);
#endif