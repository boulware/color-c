#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "array.h"
#include "string.h"
#include "game_state.h"

struct MainMenu
{
	Array<String> option_strings;
	int selected_option;
    int hovered_option;
};

void InitMainMenu(MainMenu *menu);
GameState TickMainMenu(MainMenu *menu);

#endif