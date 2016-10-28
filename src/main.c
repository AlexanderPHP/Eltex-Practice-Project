#include "global.h"

void mvwprintws(WINDOW* win, int y, int x, int num)
{
    wmove(win, y, x);
    for(int i = 0; i < num; i++)
        waddch(win, ' ');
}

void func(cursed *win)
{
    char item[NAME_MAX];
    int n;
    strncpy(item, (char *)item_name(current_item(win->menu)), NAME_MAX);
    if(strncmp(item, ".", NAME_MAX) == 0) return;
    if(strncmp(item, "..", NAME_MAX) == 0 && strncmp(win->path, "/", PATH_MAX) != 0)
    {
        memset(&win->path[strlen(win->path) - 1], '\0', 1);
        memset(&win->path[strrchr(win->path, '/') - win->path + 1], '\0', 1); //don't ask about it...
    }
    else
    {
        strcat(win->path, item);
        strcat(win->path, "/");
    }
    mvwprintws(win->decoration, 1, 1, getmaxx(win->decoration) - 2);
    if(strlen(win->path) > (size_t)COLS / 2)
        mvwprintw(win->decoration, 1, 1, "...%s", win->path + strlen(win->path) - COLS / 2 + 5);
    else
        mvwprintw(win->decoration, 1, 1, "%s", win->path);
    n = scandir(win->path, &win->files, NULL, alphasort);
    if (n < 0)
    {
        mvwprintws(win->decoration, 1, 1, getmaxx(win->decoration) - 2);
        mvwprintw(win->decoration, 1, 1, "%s", "Permission denied or not a directory!");
        memset(&win->path[strlen(win->path) - 1], '\0', 1);
        memset(&win->path[strrchr(win->path, '/') - win->path + 1], '\0', 1); //don't ask about it...
    }
    else
    {
        tui_destroy_menu(win);
        win->items_num = n;
        tui_make_menu(win , win->files, func);
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

    strncpy(ltab->path, "/", PATH_MAX - 1);
    strncpy(rtab->path, "/", PATH_MAX - 1);
    ltab->items_num = scandir(ltab->path, &ltab->files, 0, alphasort); // освободить память
    rtab->items_num = scandir(rtab->path, &rtab->files, 0, alphasort);

    tui_make_menu(ltab, ltab->files, func);
    tui_make_menu(rtab, rtab->files, func);

    touchwin(panel_window(active_tab->panel));
    update_panels( );
    doupdate( );

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
            case 13:
                p = (void (*)(cursed *))(uintptr_t)item_userptr(current_item(active_tab->menu));
                p(active_tab);
                break;
            default:break;
        }
        touchwin(panel_window(active_tab->panel));
        update_panels();
        doupdate();
    }
    for(size_t i = 0; i < sizeof(ltab->files) / sizeof(ltab->files[0]); ++i)
    {
        free(ltab->files[i]);
    }
    free(ltab->files);
    for(size_t i = 0; i < sizeof(rtab->files) / sizeof(rtab->files[0]); ++i)
    {
        free(rtab->files[i]);
    }
    free(rtab->files);
    tui_destroy_menu(rtab);
    tui_destroy_menu(ltab);
    tui_del_win(ltab);
    tui_del_win(rtab);
    endwin( );
}