#include "campaign.h"

GameState
TickCampaign(Campaign *campaign)
{


    GameState new_state = GameState::None;
    if(Pressed(KeyBind::Exit)) new_state = GameState::MainMenu;

    return new_state;
}