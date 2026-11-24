#include "board.h"
void DrawBoard() {
    DrawRectangle(0, cfg::Hud, cfg::Width, cfg::Height - cfg::Hud, cfg::Border);
    for (int y = 1; y <= cfg::Rows; ++y)
        for (int x = 1; x <= cfg::Columns; ++x)
            DrawRectangle(cfg::BoardX + (x-1)*cfg::CellSize,
                          cfg::BoardY + (y-1)*cfg::CellSize,
                          cfg::CellSize, cfg::CellSize, (x+y)%2 ? cfg::Dark : cfg::Light);
}
