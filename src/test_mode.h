#ifndef TEST_MODE_H
#define TEST_MODE_H

struct TestMode
{
    Vec2f a0 = Vec2f{400.f,400.f};
    Vec2f a1 = Vec2f{800.f, 600.f};
    int n = 1;
};

GameState TickTestMode(TestMode *test);

#endif