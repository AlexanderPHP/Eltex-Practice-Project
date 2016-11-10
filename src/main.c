#include <stdlib.h>
#include <string.h>

#include "tui.h"
#include "files.h"
#include "event.h"

int main(void)
{
    tui_init();

    tab_window *ltab = malloc(sizeof *ltab);
    tab_window *rtab = malloc(sizeof *ltab);
    int ch;
    void (*p)(tab_window *, const char *);
    tab_window *active_tab;
    ltab->win = tui_new_win(0, 0, LINES, COLS / 2 , 2);
    rtab->win = tui_new_win(0, COLS / 2, LINES, COLS / 2, 2);

    mvwprintw(ltab->win->decoration, 1,1, "/");
    mvwprintw(rtab->win->decoration, 1,1, "/");

    set_panel_userptr(ltab->win->panel, rtab);
    set_panel_userptr(rtab->win->panel, ltab);

    top_panel(ltab->win->panel);

    active_tab = ltab;

    strncpy(ltab->path, "/", PATH_MAX - 1);
    strncpy(rtab->path, "/", PATH_MAX - 1);
    ltab->items_num = scan_dir(ltab->path, &ltab->files, dirsortbyname);
    rtab->items_num = scan_dir(rtab->path, &rtab->files, dirsortbyname);

    tui_make_menu(ltab, event_handler);
    tui_make_menu(rtab, event_handler);

    touchwin(panel_window(active_tab->win->panel));
    update_panels();
    doupdate();

    while ((ch = getch()) != KEY_F(12))
    {
        p = (void (*)(tab_window *, const char *))(uintptr_t)item_userptr(current_item(active_tab->menu));
        switch (ch)
        {
            case '\t':
                active_tab = (tab_window *) panel_userptr(active_tab->win->panel);
                top_panel(active_tab->win->panel);
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
                p(active_tab, "select");
                break;
            case KEY_F(2):
                p(active_tab, "remove");
                break;
            case KEY_F(3):
                p(active_tab, "rename");
                break;
            default:break;
        }
        touchwin(panel_window(active_tab->win->panel));
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
    free(ltab);
    free(rtab);
    endwin();
}