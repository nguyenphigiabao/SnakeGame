#include "game.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>
#endif
int ReadHighScore(const std::string& path) {
    std::ifstream file(path);
    int value=0; std::string extra;
    if (!(file>>value)||value<0||value>cfg::Capacity-3||(file>>extra)) return 0;
    return value;
}
bool WriteHighScore(const std::string& path,int score) {
    if(score<0||score>cfg::Capacity-3) return false;
    const std::string temporary=path+".tmp";
    {
        std::ofstream file(temporary,std::ios::trunc);
        if(!file) return false;
        file<<score<<'\n'; file.flush();
        if(!file) return false;
    }
    // Replace only after a complete write; preserve the previous record on failure.
#ifdef _WIN32
    return MoveFileExA(temporary.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
#else
    return std::rename(temporary.c_str(),path.c_str())==0;
#endif
}
void Game::Init(const std::string& path) {
    scorePath=path; best=ReadHighScore(path); audio.Init(); ui.Init(); Reset();
}
void Game::SetState(State next) { state=next; ui.overlayAge=0; }
void Game::Reset() {
    snake.Reset(); apple={13,8}; score=0; accumulator=0; renderAlpha=1;
    step=cfg::InitialStep; facingAngle=0; effects={}; recordCelebrated=false; SetState(State::READY);
}
void Game::Start() {
    if(state==State::READY) {
        // The first frame can already interpolate away from the starting cell.
        accumulator=step;
        SetState(State::PLAYING);
        audio.Play(Cue::Start);
    }
}
void Game::ToggleFullscreenMode() {
    if(!IsWindowFullscreen()) {
        windowWidth=GetScreenWidth(); windowHeight=GetScreenHeight(); windowPosition=GetWindowPosition();
        int monitor=GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor),GetMonitorHeight(monitor));
        ToggleFullscreen();
    } else {
        ToggleFullscreen(); SetWindowSize(windowWidth,windowHeight);
        SetWindowPosition(static_cast<int>(windowPosition.x),static_cast<int>(windowPosition.y));
    }
}
void Game::Handle(Action action) {
    switch(action) {
    case Action::Fullscreen: ToggleFullscreenMode(); break;
    case Action::Mute: audio.muted=!audio.muted; break;
    case Action::Exit: quit=true; break;
    case Action::Resume: if(state==State::PAUSED) SetState(State::PLAYING); break;
    case Action::Pause: if(state==State::PLAYING) SetState(State::PAUSED); break;
    case Action::Restart: Reset(); break;
    case Action::None: break;
    }
    if(action!=Action::None) audio.Play(Cue::Click);
}
void Game::Tick() {
    MoveResult result=snake.Step(apple);
    if(result==MoveResult::Collision) {
        renderAlpha=1; SetState(State::GAME_OVER); audio.Play(Cue::Lose); return;
    }
    if(result==MoveResult::Ate||result==MoveResult::Victory) {
        ++score; effects.Burst(Center(apple));
        if(score>best) {
            best=score;
            if(!WriteHighScore(scorePath,best)) TraceLog(LOG_WARNING,"Could not save high score: %s",scorePath.c_str());
            if(!recordCelebrated) {audio.Play(Cue::Record); recordCelebrated=true;}
            else audio.Play(Cue::Eat);
        } else audio.Play(Cue::Eat);
        if(result==MoveResult::Victory||!SpawnApple(snake,apple,random)) {
            renderAlpha=1; SetState(State::VICTORY); return;
        }
    }
}
void Game::Update(float dt,Vector2 mouse,bool handleInput) {
    if(handleInput) {
        Handle(ui.Update(dt,mouse,state));
        if(IsKeyPressed(KEY_F11)) Handle(Action::Fullscreen);
        if(IsKeyPressed(KEY_M)) Handle(Action::Mute);
        if(IsKeyPressed(KEY_R)) Reset();
        if(IsKeyPressed(KEY_SPACE)) {
            if(state==State::READY) Start();
            else if(state==State::PLAYING) SetState(State::PAUSED);
            else if(state==State::PAUSED) SetState(State::PLAYING);
            else {Reset();Start();}
        }
        if(IsKeyPressed(KEY_ESCAPE)) {
            if(state==State::PLAYING) SetState(State::PAUSED);
            else if(state==State::PAUSED||state==State::GAME_OVER||state==State::VICTORY) Reset();
        }
        if(state==State::PLAYING&&!IsWindowFocused()) SetState(State::PAUSED);
        // The event queue preserves rapid direction presses in arrival order.
        for(int key=GetKeyPressed();key;key=GetKeyPressed()) {
            Direction direction;
            switch(key) {
            case KEY_W: case KEY_UP: direction=Direction::Up; break;
            case KEY_S: case KEY_DOWN: direction=Direction::Down; break;
            case KEY_A: case KEY_LEFT: direction=Direction::Left; break;
            case KEY_D: case KEY_RIGHT: direction=Direction::Right; break;
            default: continue;
            }
            if(state==State::READY||state==State::PLAYING) {snake.Queue(direction);Start();}
        }
    } else ui.overlayAge+=dt;
    if(state!=State::PAUSED) effects.Update(dt);
    if(state!=State::PLAYING) return;
    accumulator+=dt;
    // Fixed logic ticks are independent of the render frame rate.
    while(accumulator>=step&&state==State::PLAYING) {
        accumulator-=step; Tick();
        step=std::max(cfg::MinimumStep,cfg::InitialStep-score*0.0025f);
    }
    if(state==State::PLAYING) {
        renderAlpha=std::clamp(accumulator/step,0.0f,1.0f);
        // Show the turn as soon as input arrives. Body and collision checks
        // still follow cell boundaries at the original movement speed.
        Direction facing=snake.turnCount ? snake.turns[0] : snake.direction;
        float target=static_cast<int>(facing)*PI/2;
        float difference=std::remainder(target-facingAngle,2*PI);
        float maxRotation=dt*cfg::HeadTurnRate;
        facingAngle=std::remainder(facingAngle+std::clamp(difference,-maxRotation,maxRotation),2*PI);
    }
}
void Game::Draw() const {
    DrawBoard();
    if(state!=State::VICTORY) {
        float t=std::clamp(effects.appleAge/0.22f,0.0f,1.0f);
        DrawApple(Center(apple),1-(1-t)*(1-t)*(1-t));
    }
    DrawSnake(snake,renderAlpha,facingAngle); effects.Draw();
    ui.DrawHud(score,best,effects.scorePulse,audio.muted);
    ui.DrawOverlay(state,score,best);
}
void Game::Shutdown() {
    if(!WriteHighScore(scorePath,best)) TraceLog(LOG_WARNING,"High score could not be saved: %s",scorePath.c_str());
    audio.Shutdown(); ui.Shutdown();
}
