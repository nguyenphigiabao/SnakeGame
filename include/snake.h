#pragma once
#include "board.h"
#include <array>
enum class Direction { Right, Down, Left, Up };
Cell DirectionDelta(Direction d);
enum class MoveResult { Moved, Ate, Collision, Victory };
struct Snake {
    std::array<Cell, cfg::Capacity> cells{}, previous{};
    int length = 3;
    Direction direction = Direction::Right, previousDirection = Direction::Right;
    std::array<Direction, 3> turns{};
    int turnCount = 0;
    void Reset();
    bool Occupies(Cell c, int count = -1) const;
    void Queue(Direction next);
    MoveResult Step(Cell apple);
};
void DrawSnake(const Snake& snake, float alpha);
