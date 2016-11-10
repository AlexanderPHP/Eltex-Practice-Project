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

void tui_del_win(tab_window *tab)
{
    del_panel(tab->win->panel);
    delwin(tab->win->decoration);
    delwin(tab->win->overlay);
    free(tab->win);
}

void tui_destroy_menu (tab_window *tab)
{
    unpost_menu(tab->menu);
    free_menu(tab->menu);
    for(int i = 0; i < tab->items_num + 1; ++i)
    {
        free_item(tab->items[i]);
        free(tab->str_items[i]);
    }
    free(tab->items);
    free(tab->str_items);
}

void tui_make_menu (tab_window *tab, void (event_handler)(tab_window *, const char *))
{
    struct tm * timeifo;
    int tabwidth = COLS / 2 - 1;
    int namewidth = tabwidth - 22;
    char timebuf[80];
    char fsizebuf[8];
    char item[tabwidth];
    tab->items = calloc((size_t)tab->items_num + 1, sizeof(ITEM *));
    tab->str_items = calloc((size_t)tab->items_num + 1, sizeof(char *));
    for(int i = 0; i < tab->items_num; ++i)
    {
        timeifo = localtime(&tab->files[i]->last_mod.tv_sec);
        strftime(timebuf, 80, "%b %d %R", timeifo);
        get_bytes_readable(fsizebuf, 8, tab->files[i]->size);
        int n = snprintf(
                item
                , (size_t) tabwidth
                , "%-*.*s|%7.7s|%12s"
                , namewidth
                , namewidth
                , tab->files[i]->name
                , fsizebuf
                , timebuf);
        tab->str_items[i] = calloc((size_t) (n + 1), sizeof(char));
        strncpy(tab->str_items[i], item, (size_t) n);
        tab->items[i] = new_item(tab->str_items[i], "");
        set_item_userptr(tab->items[i], (void *)(uintptr_t)event_handler);
    }
    tab->menu = new_menu(tab->items);
    set_menu_win(tab->menu, tab->win->overlay);
    set_menu_mark(tab->menu, "");
    set_menu_fore(tab->menu, COLOR_PAIR(2) | A_REVERSE);
    set_menu_back(tab->menu, COLOR_PAIR(2));
    //set_menu_grey(win->menu, COLOR_PAIR(3));
    set_menu_format(tab->menu, getmaxy(tab->win->overlay), 1);
    menu_opts_off(tab->menu, O_SHOWDESC);
    menu_opts_on(tab->menu, O_ROWMAJOR);
    post_menu(tab->menu);
}

cursed_window *tui_new_win(int sy, int sx, int h, int w, int bckg)
{
    cursed_window *new = malloc (sizeof *new);
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

    update_panels();
    doupdate();
    return new;
}

char *tui_make_input(int32_t max_len)
{
    curs_set(1);
    cursed_window *neww = malloc (sizeof *neww);
    int x, y;
    getmaxyx(stdscr, y, x);
    neww->decoration = newwin(7, max_len + 2, y/2 - 2, x/2 - (max_len + 2)/2);
    wbkgd(neww->decoration, COLOR_PAIR(2));

    wborder(neww->decoration, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER,
            ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

    getmaxyx(neww->decoration, y, x);
    neww->overlay = derwin(neww->decoration, y - 3, x - 2, 2, 1);
    wbkgd(neww->overlay, COLOR_PAIR(2));
    neww->panel = new_panel(neww->decoration);

    FIELD *field[2];
    field[0] = new_field(1, max_len - 2, 2, 1, 0, 0);
    field[1] = NULL;
    FORM  *my_form;
    // set_field_type(field[0],TYPE_ALPHA);
    set_field_fore(field[0], COLOR_PAIR(2));/* Put the field with blue background */
    set_field_back(field[0], COLOR_PAIR(2) | A_UNDERLINE);
    field_opts_off(field[0], O_AUTOSKIP); /* Don't go to next field when this */


    /* Create the form and post it */
    my_form = new_form(field);
    set_form_sub(my_form, neww->overlay);
    //set_field_buffer(field[0], 0, buf);
    post_form(my_form);
    mvwprintw(neww->decoration, 1, (getmaxx(neww->decoration) - 19) / 2, "ENTER NEW FILE NAME");
    mvwprintw(neww->decoration, 2, (getmaxx(neww->decoration) - 21) / 2, "OR PRESS ESC TO EXIT");
    int32_t user_key=0;
    keypad(neww->overlay, TRUE);

    touchwin(panel_window(neww->panel));
    update_panels();
    doupdate();
    bool flag = FALSE;
    char *result = NULL;
    do
    {
        if(flag || (user_key = getch()) == 27)
            break;
        switch(user_key)
        {
            case 0xD:
                form_driver(my_form, REQ_VALIDATION);
                flag = TRUE;
                break;
            case KEY_BACKSPACE:
                form_driver(my_form, REQ_DEL_PREV);
                break;
            default:
                form_driver(my_form, user_key);
                break;
        }
        touchwin(panel_window(neww->panel));
        update_panels();
        doupdate();
        printw("%d\n", flag);
    } while(true);

    asprintf(&result, "%s", field_buffer(field[0], 0));
    unpost_form(my_form);
    free_field(field[0]);
    free_form(my_form);
    delwin(neww->decoration);
    delwin(neww->overlay);
    curs_set(0);
    return result;
}