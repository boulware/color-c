#include "test_mode.h"

GameState
TickTestMode(TestMode *test)
{
    String line_label = StringFromCString("4");

    DrawDirectedLine(test->a0, test->a1, 350.f*Vec2f{1.f, -1.f}, c::red, 40.f, line_label);

    DrawFilledRect(AlignRect({test->a0, {3.f,3.f}}, c::align_center), c::green);
    DrawFilledRect(AlignRect({test->a1, {3.f,3.f}}, c::align_center), c::red);

    if(Down(vk::LMB))
    {
        test->a0 = MousePos();
    }
    if(Down(vk::RMB))
    {
        test->a1 = MousePos();
    }

    int scroll = MouseScroll();
    test->n += scroll;

    DrawUiText(c::def_text_layout, {}, "n = %d", test->n);

    // if(Pressed(vk::RMB))
    // {
    //     test->b0 = MousePos();
    // }
    // if(Released(vk::RMB))
    // {
    //     test->b1 = MousePos();
    // }

    // float edge_percent = 0.f;
    // float t = 0.f;
    // float u = 0.f;
    // Color color = c::white;
    // if(Down(vk::LMB))
    // {
    //     if(LineSegmentsInnerIntersect(test->a0, MousePos(), test->b0, test->b1, edge_percent))
    //     {
    //         color = c::red;
    //     }
    // }
    // else if(Down(vk::RMB))
    // {
    //     if(LineSegmentsInnerIntersect(test->a0, test->a1, test->b0, MousePos(), edge_percent))
    //     {
    //         color = c::red;
    //     }
    // }
    // else
    // {
    //     if(LineSegmentsInnerIntersect(test->a0, test->a1, test->b0, test->b1, edge_percent))
    //     {
    //         color = c::red;
    //     }
    // }

    // if(Down(vk::LMB))
    // {
    //     DrawLine(test->a0, MousePos(), color);
    // }
    // else
    // {
    //     DrawLine(test->a0, test->a1, color);
    // }

    // if(Down(vk::RMB))
    // {
    //     DrawLine(test->b0, MousePos(), color);
    // }
    // else
    // {
    //     DrawLine(test->b0, test->b1, color);
    // }

    GameState new_state = GameState::None;
    if(Pressed(KeyBind::Exit)) new_state = GameState::MainMenu;

    return new_state;
}