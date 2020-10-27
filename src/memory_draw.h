#ifndef MEMORY_DRAW_H
#define MEMORY_DRAW_H

Rect DrawArenas(Pool<Arena> *arena_pool, Vec2f pos);
Rect DrawArenas(Pool<Arena> volatile *arena_pool, Vec2f pos);

#endif