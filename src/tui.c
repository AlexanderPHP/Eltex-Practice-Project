#include "tui.h"
#include "files.h"

void tui_init()
{
    initscr();
    cbreak();
    raw();
    nonl();
    noecho();
    curs_set(1);
    keypad(stdscr, TRUE);
    if (!has_colors())
    {
        endwin();
        printf("\nYour terminal does not support color\n");
        exit(1);
    }
    start_color();
    use_default_colors();
    init_pair( 1, COLOR_WHITE,   COLOR_BLUE);
    init_pair( 2, COLOR_GREEN,    COLOR_BLACK);
    init_pair( 3, COLOR_MAGENTA,  COLOR_BLUE);
    init_pair( 4, COLOR_BLACK,     COLOR_YELLOW);
    init_pair( 5, COLOR_GREEN,   COLOR_WHITE);
    init_pair( 6, COLOR_BLUE,   COLOR_WHITE);
    init_pair( 7, COLOR_MAGENTA,   COLOR_WHITE);
    init_pair( 8, COLOR_WHITE, COLOR_MAGENTA);
    init_pair( 9, COLOR_CYAN,    COLOR_WHITE);
    init_pair(10, COLOR_YELLOW,  COLOR_BLACK);
}

void tui_del_win(tab_window *win)
{
    del_panel(win->panel);
    free(win);
}

void tui_destroy_menu (tab_window *win)
{
    unpost_menu(win->menu);
    free_menu(win->menu);
    for(int i = 0; i < win->items_num + 1; ++i)
    {
        free_item(win->items[i]);
        free(win->str_items[i]);
    }
    free(win->items);
    free(win->str_items);
}

void tui_make_menu (tab_window *win, void (on_item_selected)(tab_window *))
{
    struct tm * timeifo;
    int tabwidth = COLS / 2 - 1;
    int namewidth = tabwidth - 22;
    char timebuf[80];
    char fsizebuf[8];
    char item[tabwidth];
    win->items = calloc((size_t)win->items_num + 1, sizeof(ITEM *));
    win->str_items = calloc((size_t)win->items_num + 1, sizeof(char *));
    for(int i = 0; i < win->items_num; ++i)
    {
        timeifo = localtime(&win->files[i]->last_mod.tv_sec);
        strftime(timebuf, 80, "%b %d %R", timeifo);
        get_bytes_readable(fsizebuf, 8, win->files[i]->size);
        int n = snprintf(
                item
                , (size_t) tabwidth
                , "%-*.*s|%7.7s|%12s"
                , namewidth
                , namewidth
                , win->files[i]->name
                , fsizebuf
                , timebuf);
        win->str_items[i] = calloc((size_t) (n + 1), sizeof(char));
        strncpy(win->str_items[i], item, (size_t) n);
        win->items[i] = new_item(win->str_items[i], "");
        set_item_userptr(win->items[i], (void *)(uintptr_t)on_item_selected);
    }
    win->menu = new_menu(win->items);
    set_menu_win(win->menu, win->overlay);
    set_menu_mark(win->menu, "");
    set_menu_fore(win->menu, COLOR_PAIR(2) | A_REVERSE);
    set_menu_back(win->menu, COLOR_PAIR(2));
    //set_menu_grey(win->menu, COLOR_PAIR(3));
    set_menu_format(win->menu, getmaxy(win->overlay), 1);
    menu_opts_off(win->menu, O_SHOWDESC);
    menu_opts_on(win->menu, O_ROWMAJOR);
    post_menu(win->menu);
}

tab_window *tui_new_win(int sy, int sx, int h, int w, int bckg)
{
    tab_window *new = malloc (sizeof *new);
    new->decoration = newwin(h, w, sy, sx);
    wbkgd(new->decoration, COLOR_PAIR(bckg));
    wborder(new->decoration, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER,
            ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    int x, y;
    getmaxyx(new->decoration, y, x);
    new->overlay = derwin(new->decoration, y - 4, x - 2, 3, 1);
    wbkgd(new->overlay, COLOR_PAIR(bckg));
    new->panel = new_panel(new->decoration);
    mvwhline(new->decoration, 2, 1, ACS_HLINE, getmaxx(new->decoration) - 2);

    update_panels( );
    doupdate( );
    return new;
}