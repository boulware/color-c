#ifndef ROOM_H
#define ROOM_H

enum class RoomType : u32
{
    Undefined,
    Boss,
    Battle,
    Shop,
    Camp,
    Fishing,
    COUNT,
};

char *RoomType_userstrings[] = {
    "[Undefined]",
    "Boss",
    "Battle",
    "Shop",
    "Camp",
    "Fishing",
    "[COUNT]"
};

struct Room
{
    RoomType type;
};

char *RoomTypeUserString(RoomType type);

#endif