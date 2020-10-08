#ifndef TEST_MODE_H
#define TEST_MODE_H

struct TestMode
{
    Vec2f a0, a1;
    Vec2f b0, b1;
};

GameState TickTestMode(TestMode *test);

#endif