#ifndef DEBUG_OVERLAY_H
#define DEBUG_OVERLAY_H

enum class OverlayOption : int
{
    Arenas,
    TimedBlocks,
    Frametime,
    COUNT,
};

const char *OverlayOption_userstrings[] = {
    "Arenas",
    "TimedBlocks",
    "Frametime",
};

struct DebugOverlay
{
    bool option_active[OverlayOption::COUNT];
    Vec2f window_positions[OverlayOption::COUNT];

    int dragging_index = -1; // OverlayOption index for window currently being dragged.
};

#endif