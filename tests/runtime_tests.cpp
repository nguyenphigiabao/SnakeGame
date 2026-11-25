#include "game.h"
#include "rlgl.h"
#include <algorithm>
#include <cstdio>
#include <iostream>
int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE|FLAG_MSAA_4X_HINT);
    InitWindow(808,808,"Snake Game - runtime verification");
    SetExitKey(KEY_NULL); SetTargetFPS(120);
    Game game; game.Init("runtime-highscore.txt");
    auto canvas=LoadRenderTexture(1616,1616); SetTextureFilter(canvas.texture,TEXTURE_FILTER_BILINEAR);
    Camera2D camera{{0,0},{0,0},0,2};
    int failures=0;
    auto check=[&](bool value,const char* label) {std::cout<<(value?"PASS: ":"FAIL: ")<<label<<'\n';if(!value)++failures;};
    auto frame=[&]() {
        game.Update(1.0f/120,{-100,-100},false);
        BeginTextureMode(canvas); ClearBackground(cfg::Border);BeginMode2D(camera);game.Draw();EndMode2D();EndTextureMode();
        float scale=std::min(GetRenderWidth()/808.0f,GetRenderHeight()/808.0f);
        BeginDrawing();ClearBackground({24,36,20,255});
        BeginMode2D(Camera2D{{0,0},{0,0},0,1});
        DrawTexturePro(canvas.texture,{0,0,1616,-1616},{(GetRenderWidth()-808*scale)/2,(GetRenderHeight()-808*scale)/2,808*scale,808*scale},{0,0},0,WHITE);
        EndMode2D();
        EndDrawing();
    };
    auto frames=[&](int count) {for(int i=0;i<count;++i)frame();};
    auto capture=[&](const char* file) {
        // Raylib 5.5 LoadImageFromScreen multiplies Windows DPI even when the
        // framebuffer is already in pixels. Read its actual dimensions instead.
        Image image{rlReadScreenPixels(GetRenderWidth(),GetRenderHeight()),GetRenderWidth(),GetRenderHeight(),1,PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
        check(ExportImage(image,file),file);UnloadImage(image);
    };
    check(game.state==State::MENU,"game opens on the mode selection screen");
    frames(20);capture("menu.png");
    game.Handle(Action::Hard);
    check(game.state==State::READY&&game.mode==Mode::Hard&&game.lives==1,"hard mode has a single life");
    frames(30);capture("ready.png");
    // Raylib 5.5 automation event type 2 is INPUT_KEY_DOWN. Feed the normal
    // keyboard path and advance one frame at 120 Hz.
    PlayAutomationEvent({0,2,{KEY_UP}});
    game.Update(1.0f/120,{-100,-100});
    check(game.state==State::PLAYING&&game.snake.cells[0]==Cell{4,7},
          "first arrow starts movement in the input frame");
    PlayAutomationEvent({0,2,{KEY_LEFT}});
    game.Update(1.0f/120,{-100,-100});
    check(game.snake.turnCount==1&&game.snake.turns[0]==Direction::Left,
          "next arrow is registered between ticks");
    frames(17);
    check(game.snake.cells[0]==Cell{3,7},"buffered corner executes at the next boundary");
    game.Reset();
    game.apple={5,8};game.Start();frames(25);
    check(game.score==1&&game.snake.length==4,"eat, grow, score during fixed updates");
    game.Handle(Action::Pause);Cell head=game.snake.cells[0];frames(30);
    check(game.snake.cells[0]==head&&game.state==State::PAUSED,"pause freezes gameplay");capture("paused.png");
    SetWindowSize(1100,650);frames(30);capture("resized.png");
    game.Handle(Action::Fullscreen);frames(30);
    check(IsWindowFullscreen(),"enter fullscreen");capture("fullscreen.png");
    game.Handle(Action::Fullscreen);frames(20);
    check(!IsWindowFullscreen()&&GetScreenWidth()==1100&&GetScreenHeight()==650,"restore window dimensions");
    SetWindowSize(808,808);game.Handle(Action::Resume);frames(300);
    check(game.state==State::GAME_OVER,"wall collision reaches Game Over");capture("game-over.png");
    int record=game.best;game.Handle(Action::Restart);
    check(game.state==State::READY&&game.score==0&&game.best==record,"restart retains record");
    game.snake.Queue(Direction::Up);game.Start();frames(28);capture("turn.png");
    // Instant turn: a key pressed just after the snake left a cell re-plans that move.
    game.Reset();game.Start();game.Update(1.0f/120,{-100,-100},false);
    check(game.snake.cells[0]==Cell{5,8},"straight first move");
    PlayAutomationEvent({0,2,{KEY_UP}});
    game.Update(1.0f/120,{-100,-100});
    check(game.snake.cells[0]==Cell{4,7}&&game.snake.direction==Direction::Up,"early turn applies without waiting a tick");
    game.Reset();game.snake.length=cfg::Capacity-1;game.snake.direction=Direction::Left;
    int n=0;
    for(int y=1;y<=cfg::Rows;++y)for(int x=1;x<=cfg::Columns;++x) {
        Cell cell{y%2?x:cfg::Columns+1-x,y};
        if(cell!=Cell{1,1})game.snake.cells[n++]=cell;
    }
    game.snake.previous=game.snake.cells;game.score=251;game.apple={1,1};game.Start();frames(60);
    check(game.state==State::VICTORY&&game.score==252,"full board reaches Victory");capture("victory.png");
    game.Handle(Action::Easy);
    check(game.state==State::READY&&game.mode==Mode::Easy&&game.lives==3,"easy mode starts with three lives");
    game.Start();frames(400);
    check(game.lives==2&&game.state==State::READY&&game.score==1,"easy crash costs a life and keeps the score");
    capture("easy-life-lost.png");
    frames(90);game.Start();frames(400);game.Start();frames(400);
    check(game.lives==0&&game.state==State::GAME_OVER,"third crash ends an easy game");
    game.Handle(Action::Restart);
    check(game.lives==3&&game.score==0,"restart refills easy lives");
    game.Handle(Action::Menu);check(game.state==State::MENU,"return to mode selection");
    game.Handle(Action::Mute);game.Handle(Action::Mute);
    game.Shutdown();check(ReadHighScore("runtime-highscore.txt")==252,"runtime persistence");
    UnloadRenderTexture(canvas);CloseWindow();std::remove("runtime-highscore.txt");
    std::cout<<"Runtime failures: "<<failures<<'\n';return failures?1:0;
}
