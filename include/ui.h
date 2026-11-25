#pragma once
#include "config.h"
#include <array>
enum class State { MENU, READY, PLAYING, PAUSED, GAME_OVER, VICTORY };
enum class Mode { Easy, Hard };
enum class Action { None, Fullscreen, Mute, Exit, Resume, Restart, Pause, Easy, Hard, Menu };
struct HudInfo { int score=0, best=0; float pulse=0; bool muted=false; State state=State::MENU; Mode mode=Mode::Hard; int lives=1; float heartPulse=0; };
struct UI {
    void Init();
    void Shutdown();
    std::array<float,7> hover{};
    Vector2 mouse{};
    float overlayAge=0;
    int menuChoice=0; // 0 = Easy, 1 = Hard (keyboard selection on the mode screen)
    Action Update(float dt, Vector2 position, State state);
    void DrawHud(const HudInfo& info) const;
    void DrawOverlay(const HudInfo& info,bool respawned) const;
};
void CenterText(const char* text, float x, float y, float size, Color color);
void DrawHeart(Vector2 center, float size, Color color);
