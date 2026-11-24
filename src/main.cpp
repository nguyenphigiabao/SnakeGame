#include "game.h"
#include <algorithm>
int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(cfg::Width, cfg::Height, "Snake Game");
    SetWindowMinSize(404,404);
    SetExitKey(KEY_NULL);
    SetTargetFPS(120);
    SetMouseScale(1,1);
    RenderTexture2D canvas=LoadRenderTexture(cfg::Width*2,cfg::Height*2);
    Camera2D canvasCamera{{0,0},{0,0},0,2};
    SetTextureFilter(canvas.texture,TEXTURE_FILTER_BILINEAR);
    Game game;
    game.Init(std::string(GetApplicationDirectory())+"highscore.txt");
    while (!WindowShouldClose()&&!game.quit) {
        float scale=std::min(static_cast<float>(GetRenderWidth())/cfg::Width,
                             static_cast<float>(GetRenderHeight())/cfg::Height);
        Vector2 offset{(GetRenderWidth()-cfg::Width*scale)/2,(GetRenderHeight()-cfg::Height*scale)/2};
        Vector2 mouse=GetMousePosition();
        mouse={(mouse.x-offset.x)/scale,(mouse.y-offset.y)/scale};
        game.Update(std::min(GetFrameTime(),0.1f),mouse);
        BeginTextureMode(canvas); ClearBackground(cfg::Border);
        BeginMode2D(canvasCamera); game.Draw(); EndMode2D(); EndTextureMode();
        BeginDrawing(); ClearBackground({24,36,20,255});
        // Use framebuffer pixels explicitly: Windows DPI callbacks can otherwise
        // apply a second scale after resizing a Raylib 5.5 window.
        BeginMode2D(Camera2D{{0,0},{0,0},0,1});
        DrawTexturePro(canvas.texture,{0,0,static_cast<float>(cfg::Width*2),-static_cast<float>(cfg::Height*2)},
                       {offset.x,offset.y,cfg::Width*scale,cfg::Height*scale},{0,0},0,WHITE);
        EndMode2D();
        EndDrawing();
    }
    game.Shutdown(); UnloadRenderTexture(canvas); CloseWindow();
    return 0;
}
