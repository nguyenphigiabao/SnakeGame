#pragma once
#include "config.h"
// Board coordinates deliberately start at 1.
struct Cell {
    int x = 1, y = 1;
    bool operator==(Cell other) const { return x == other.x && y == other.y; }
    bool operator!=(Cell other) const { return !(*this == other); }
};
inline Vector2 Center(Cell c) {
    return {cfg::BoardX + (c.x - 0.5f) * cfg::CellSize,
            cfg::BoardY + (c.y - 0.5f) * cfg::CellSize};
}
inline bool Inside(Cell c) {
    return c.x >= 1 && c.x <= cfg::Columns && c.y >= 1 && c.y <= cfg::Rows;
}
void DrawBoard();
