#include "ai_explorer.h"

GameState
TickAiExplorer(AiExplorer *explorer)
{


    GameState new_state = {};
    if(Pressed(vk::esc)) new_state = GameState::MainMenu;
    return new_state;
}