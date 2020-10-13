#include "main_menu.h"

void
InitMainMenu(MainMenu *menu)
{
	menu->selected_option = 0;
    menu->option_strings = CreatePermanentArray<String>(6);
    menu->option_strings += StringFromCString("Battle");
    menu->option_strings += StringFromCString("Campaign");
    menu->option_strings += StringFromCString("Editor");
    menu->option_strings += StringFromCString("Options");
    menu->option_strings += StringFromCString("Test");
    menu->option_strings += StringFromCString("Quit");
}

// Updates and draws main menu
// Returns the game state that should be entered (returns GameState::None if we should stay in menu)
GameState
TickMainMenu(MainMenu *menu, bool entered)
{
	if(entered)
	{
		MoveCameraToWorldRect(&game->camera, Rect{{0.f,0.f}, game->window_size});
	}

	if(Pressed(vk::down) or Repeated(vk::down))
	{
		++menu->selected_option;
		if(menu->selected_option >= menu->option_strings.count) menu->selected_option = 0;
	}
	if(Pressed(vk::up) or Repeated(vk::up))
	{
		--menu->selected_option;
		if(menu->selected_option < 0) menu->selected_option = menu->option_strings.count-1;
	}

	GameState new_state = GameState::None;
	if(Pressed(vk::enter) or Pressed(vk::LMB))
	{
		if(menu->selected_option == 0)
		{
			new_state = GameState::Battle;
		}
		else if(menu->selected_option == 1)
		{ // Campaign
			new_state = GameState::Campaign;
		}
		else if(menu->selected_option == 2)
		{ // Editor
			new_state = GameState::Editor;
		}
		else if(menu->selected_option == 3)
		{ // Options
			new_state = GameState::Options;
		}
		else if(menu->selected_option == 4)
		{
			new_state = GameState::Test;
		}
		else if(menu->selected_option == 5)
		{ // Quit
			new_state = GameState::Quit;
		}
	}

	// Draw
	Vec2f pen = 0.5f*game->window_size;
	pen.y -= 0.5f*(menu->option_strings.count * LineHeight(c::main_menu_unselected_text_layout));

	for(int i=0; i<menu->option_strings.count; ++i)
	{
		String &string = menu->option_strings[i]; //alias

		Rect text_rect = {};
		if(i == menu->selected_option)
		{
			text_rect = DrawText(c::main_menu_selected_text_layout, pen, string);
		}
		else
		{
			text_rect = DrawText(c::main_menu_unselected_text_layout, pen, string);
		}

		if(MouseInRect(text_rect) and MouseMoved())
		{
			menu->selected_option = i;
		}

		pen.y += text_rect.size.y;
	}

	if(Pressed(vk::esc)) new_state = GameState::Quit;

	return new_state;
}

