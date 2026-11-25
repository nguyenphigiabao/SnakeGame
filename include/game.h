#pragma once
#include "animation.h"
#include "audio.h"
#include "apple.h"
#include "ui.h"
#include <string>
int ReadHighScore(const std::string& path);
bool WriteHighScore(const std::string& path,int score);
class Game {
public:
    Snake snake;
    Cell apple{13,8};
    State state=State::MENU;
    Mode mode=Mode::Hard;
    int score=0, best=0, lives=1;
    bool quit=false;
    void Init(const std::string& scoreFile);
    void Reset();
    void Start();
    void Handle(Action action);
    void SelectMode(Mode next);
    void OpenMenu();
    int MaxLives() const { return mode==Mode::Easy ? cfg::EasyLives : 1; }
    void Update(float dt,Vector2 mouse,bool handleInput=true);
    void Draw() const;
    void Shutdown();
private:
    UI ui;
    Audio audio;
    Effects effects;
    std::mt19937 random{std::random_device{}()};
    std::string scorePath;
    float accumulator=0, step=cfg::InitialStep, renderAlpha=1;
    float facingAngle=0;
    int windowWidth=cfg::Width,windowHeight=cfg::Height;
    Vector2 windowPosition{};
    bool recordCelebrated=false;
    bool respawned=false, canRewind=false;
    float heartPulse=0, readyLock=0;
    Snake beforeTick; // State before the latest straight move, for instant turns.
    void SetState(State next);
    void Tick();
    void Respawn();
    bool TryInstantTurn(Direction next);
    void ToggleFullscreenMode();
};
