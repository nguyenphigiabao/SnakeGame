#pragma once
#include "config.h"
#include <array>
enum class State { READY, PLAYING, PAUSED, GAME_OVER, VICTORY };
enum class Action { None, Fullscreen, Mute, Exit, Resume, Restart, Pause };
struct UI {
    void Init();
    void Shutdown();
    std::array<float,5> hover{};
    Vector2 mouse{};
    float overlayAge=0;
    Action Update(float dt, Vector2 position, State state);
    void DrawHud(int score,int best,float pulse,bool muted) const;
    void DrawOverlay(State state,int score,int best) const;
};
void CenterText(const char* text, float x, float y, float size, Color color);
