#ifndef TUI
#define TUI
#include <panel.h>
#include <form.h>
#include <menu.h>
#include <linux/limits.h>
#include <dirent.h>
struct _cursed_window
{
    WINDOW *background;
    WINDOW *decoration;
    WINDOW *overlay;
    PANEL *panel;
    MENU *menu;
    ITEM **items;
    struct dirent **files;
    int items_num;
    char path[PATH_MAX - 1];
};
typedef struct _cursed_window cursed;

int32_t tui_init(void);
cursed *tui_new_shadowed_win(int sy, int sx, int h, int w, char *label);
cursed *tui_new_win(int sy, int sx, int h, int w, char *label, int bckg);
void tui_del_win(cursed *win);
void tui_popup(char *data);
void tui_move_panel(cursed *win, int ch);
void tui_win_label(WINDOW *win, char *string, int attrib);
void tui_print_centered(WINDOW *win, char *string, int h_offset, int attr_mask);
void tui_make_menu (cursed *win, struct dirent **choices, void (*)(cursed *));
void tui_destroy_menu (cursed *win);
char *tui_make_input(const char *info, int32_t max_len);
cursed *tui_new_button(WINDOW *bwin, int sy, int sx, const char *label, size_t len);
void tui_progress_bar(WINDOW *win, double progress);
void tui_toggle_button (cursed *button);
#endif