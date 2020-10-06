#ifndef OPTIONS_MENU_H
#define OPTIONS_MENU_H

#include "game_state.h"

struct OptionsMenu
{
    int selected_option;
    bool option_being_modified;
};

GameState TickOptionsMenu(OptionsMenu *menu);

#endif