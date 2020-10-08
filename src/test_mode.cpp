#include "test_mode.h"

GameState
TickTestMode(TestMode *test)
{
    //SetCameraPos(0.5f*game->window_size);

    if(Pressed(vk::LMB))
    {
        test->a0 = MousePos();
    }
    if(Released(vk::LMB))
    {
        test->a1 = MousePos();
    }

    if(Pressed(vk::RMB))
    {
        test->b0 = MousePos();
    }
    if(Released(vk::RMB))
    {
        test->b1 = MousePos();
    }

    float edge_percent = 0.f;
    float t = 0.f;
    float u = 0.f;
    Color color = c::white;
    if(Down(vk::LMB))
    {
        if(LineSegmentsInnerIntersect(test->a0, MousePos(), test->b0, test->b1, edge_percent))
        {
            color = c::red;
        }
    }
    else if(Down(vk::RMB))
    {
        if(LineSegmentsInnerIntersect(test->a0, test->a1, test->b0, MousePos(), edge_percent))
        {
            color = c::red;
        }
    }
    else
    {
        if(LineSegmentsInnerIntersect(test->a0, test->a1, test->b0, test->b1, edge_percent))
        {
            color = c::red;
        }
    }

    if(Down(vk::LMB))
    {
        DrawLine(test->a0, MousePos(), color);
    }
    else
    {
        DrawLine(test->a0, test->a1, color);
    }

    if(Down(vk::RMB))
    {
        DrawLine(test->b0, MousePos(), color);
    }
    else
    {
        DrawLine(test->b0, test->b1, color);
    }

    GameState new_state = GameState::None;
    if(Pressed(KeyBind::Exit)) new_state = GameState::MainMenu;

    return new_state;
}