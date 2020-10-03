#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "array.h"
#include "string.h"

struct MainMenu
{
	Array<String> option_strings;
	int option_count;
	int cur_option;
};

#endif