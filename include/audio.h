#pragma once
#include "raylib.h"
#include <array>
enum class Cue { Eat, Lose, Start, Record, Click, Count };
class Audio {
    std::array<Sound,static_cast<int>(Cue::Count)> sounds{};
    bool ready=false;
public:
    bool muted=false;
    void Init();
    void Play(Cue cue);
    void Shutdown();
};
