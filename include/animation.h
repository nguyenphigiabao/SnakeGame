#pragma once
#include "config.h"
#include <array>
struct Particle { Vector2 position{}, velocity{}; float life=0, duration=1, radius=3; Color color{}; };
struct Effects {
    std::array<Particle,48> particles{};
    Vector2 eatenPosition{};
    float eatenTime=0, appleAge=1, scorePulse=0;
    void Burst(Vector2 position);
    void Update(float dt);
    void Draw() const;
};
