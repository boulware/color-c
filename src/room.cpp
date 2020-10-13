#include "room.h"

char *
RoomTypeUserString(RoomType type)
{
    int i = (int)type;
    if(i <= 0 or i >= (int)RoomType::COUNT) return c::empty_string;

    return RoomType_userstrings[i];
}