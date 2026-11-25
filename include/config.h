#pragma once
#include "raylib.h"
namespace cfg {
constexpr int Width = 808, Height = 808, Hud = 85;
constexpr int Columns = 17, Rows = 15, CellSize = 44;
constexpr int Capacity = Columns * Rows;
constexpr int BoardX = 30, BoardY = 117;
constexpr float Radius = CellSize * 0.4f;
constexpr float InitialStep = 0.155f, MinimumStep = 0.075f;
constexpr float HeadTurnRate = PI / (2 * 0.05f); // Quarter turn in 50 ms.
// A turn pressed within this fraction of a step after the snake left a cell
// re-plans that move, so the corner happens at the cell the player just saw.
constexpr float TurnGrace = 0.35f;
constexpr int EasyLives = 3;
constexpr float RespawnLock = 0.6f; // Ignore start keys briefly after losing a life.
constexpr Color HudColor{69,110,40,255}, Border{87,138,52,255};
constexpr Color Light{170,215,81,255}, Dark{162,209,73,255};
constexpr Color Body{67,110,232,255}, Head{75,120,245,255};
constexpr Color Eye{83,133,255,255}, Pupil{24,52,130,255};
constexpr Color Apple{234,67,27,255}, Leaf{76,185,39,255};
constexpr Color Trophy{255,203,33,255}, Ink{39,68,26,255};
constexpr Color Heart{238,62,84,255};
}
