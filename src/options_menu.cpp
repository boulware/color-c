#include "options_menu.h"

GameState
TickOptionsMenu(OptionsMenu *menu)
{
    if(Pressed(vk::down) or Repeated(vk::down))
    {
        ++menu->selected_option;
        if(menu->selected_option >= (int)KeyBind::COUNT) menu->selected_option = 0;
    }
    if(Pressed(vk::up) or Repeated(vk::up))
    {
        --menu->selected_option;
        if(menu->selected_option < 0) menu->selected_option = (int)KeyBind::COUNT-1;
    }

    if(Pressed(vk::enter) and menu->option_being_modified == false)
    {
        menu->option_being_modified = true;
        platform->AnyKey();
    }

    u8 anykey = GetAnyKey();
    if(menu->option_being_modified and anykey != 0)
    {
        keybinds::vk_mappings[menu->selected_option] = anykey;
        menu->option_being_modified = false;
    }

    Vec2f pen = {10.f,10.f};
    for(int i=0; i<ArrayCount(keybinds::userstrings); ++i)
    {
        u8 virtual_key = KeyBindToVirtualKey((KeyBind)i);

        Vec2f text_size = {};
        if(i == menu->selected_option)
        {
            if(menu->option_being_modified)
            {
                text_size = DrawText(c::options_modifying_text_layout, pen,
                                     "%s => [Press Key]",
                                     keybinds::userstrings[i]).size;
            }
            else
            {
                text_size = DrawText(c::options_selected_text_layout, pen,
                                     "%s => %s",
                                     keybinds::userstrings[i],
                                     VirtualKey_userstrings[virtual_key]).size;
            }
        }
        else
        {
            text_size = DrawText(c::options_unselected_text_layout, pen,
                                 "%s => %s",
                                 keybinds::userstrings[i],
                                 VirtualKey_userstrings[virtual_key]).size;
        }

        pen.y += text_size.y;
    }

    GameState new_state = {};
    if(Pressed(KeyBind::Exit)) new_state = GameState::MainMenu;

    return new_state;
}