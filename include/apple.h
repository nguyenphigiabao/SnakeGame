#pragma once
#include "snake.h"
#include <random>
bool SpawnApple(const Snake& snake, Cell& apple, std::mt19937& random);
void DrawApple(Vector2 position, float scale = 1.0f, float opacity = 1.0f);
