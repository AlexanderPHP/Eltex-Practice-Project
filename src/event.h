#ifndef TEXTEDITOR_EVENT_H
#define TEXTEDITOR_EVENT_H

#include "tui.h"
#include "files.h"

void event_handler(tab_window *win, const char* event);
void select_item(tab_window *tab);
void remove_item(tab_window *win);
void rename_item(tab_window *win);

#endif //TEXTEDITOR_EVENT_H
