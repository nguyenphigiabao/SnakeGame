#include "ui.h"
#include "apple.h"
#include <algorithm>
#include <string>
namespace {
Font uiFont{};
bool ownedFont=false;
Font TextFont() { return ownedFont?uiFont:GetFontDefault(); }
constexpr Rectangle buttons[]={{654,17,44,50},{704,17,44,50},{754,17,44,50},{264,443,280,54},{264,511,280,48}};
void Stroke(Vector2 a,Vector2 b,float width=3.5f,Color color=WHITE) { DrawLineEx(a,b,width,color); }
void Trophy(Vector2 p) {
    DrawRectangleRounded({p.x-14,p.y-18,28,28},0.45f,8,cfg::Trophy);
    DrawRing({p.x-13,p.y-10},9,12,80,280,24,cfg::Trophy);
    DrawRing({p.x+13,p.y-10},9,12,-100,100,24,cfg::Trophy);
    DrawRectangle(static_cast<int>(p.x-3),static_cast<int>(p.y+6),6,15,cfg::Trophy);
    DrawRectangleRounded({p.x-11,p.y+20,22,4},0.4f,4,cfg::Trophy);
}
}
void UI::Init() {
    std::string path=std::string(GetApplicationDirectory())+"assets/fonts/AtkinsonHyperlegible-Regular.ttf";
    if(FileExists(path.c_str())) {
        uiFont=LoadFontEx(path.c_str(),80,nullptr,0);
        ownedFont=IsFontValid(uiFont)&&uiFont.texture.id!=GetFontDefault().texture.id;
        if(ownedFont) SetTextureFilter(uiFont.texture,TEXTURE_FILTER_BILINEAR);
    }
}
void UI::Shutdown() { if(ownedFont) UnloadFont(uiFont); ownedFont=false; }
void CenterText(const char* text,float x,float y,float size,Color color) {
    Vector2 measure=MeasureTextEx(TextFont(),text,size,1);
    DrawTextEx(TextFont(),text,{x-measure.x/2,y},size,1,color);
}
Action UI::Update(float dt,Vector2 position,State state) {
    mouse=position; overlayAge+=dt;
    Action action=Action::None;
    for (int i=0;i<5;++i) {
        bool enabled=i<3||(state==State::PAUSED||state==State::GAME_OVER||state==State::VICTORY);
        bool over=enabled&&CheckCollisionPointRec(mouse,buttons[i]);
        hover[i]+=(static_cast<float>(over)-hover[i])*std::min(1.0f,dt*14);
    }
    static int pressed=-1;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        pressed=-1;
        for(int i=0;i<5;++i) if ((i<3||state==State::PAUSED||state==State::GAME_OVER||state==State::VICTORY)&&CheckCollisionPointRec(mouse,buttons[i])) pressed=i;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if (pressed>=0&&CheckCollisionPointRec(mouse,buttons[pressed])) {
            switch(pressed) {
            case 0: action=Action::Fullscreen; break;
            case 1: action=Action::Mute; break;
            case 2: action=Action::Exit; break;
            case 3: action=state==State::PAUSED?Action::Resume:Action::Restart; break;
            case 4: action=state==State::PAUSED?Action::Restart:Action::Exit; break;
            }
        }
        pressed=-1;
    }
    bool pointing=false;
    for (float h:hover) if (h>0.5f) pointing=true;
    SetMouseCursor(pointing?MOUSE_CURSOR_POINTING_HAND:MOUSE_CURSOR_DEFAULT);
    return action;
}
void UI::DrawHud(int score,int best,float pulse,bool muted) const {
    DrawRectangle(0,0,cfg::Width,cfg::Hud,cfg::HudColor);
    DrawApple({47,45},0.9f);
    DrawTextEx(TextFont(),TextFormat("%d",score),{77,30-pulse*2},30+pulse*5,1,WHITE);
    Trophy({174,40});
    DrawTextEx(TextFont(),TextFormat("%d",best),{205,30},30,1,WHITE);
    for(int i=0;i<3;++i) {
        Rectangle r=buttons[i];
        if (CheckCollisionPointRec(mouse,r)&&IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {r.y+=2;r.height-=4;}
        DrawRectangleRounded(r,0.3f,8,Fade(WHITE,hover[i]*0.16f));
    }
    for(float sx:{-1.0f,1.0f}) for(float sy:{-1.0f,1.0f}) {
        float x=676+sx*13,y=42+sy*13;
        Stroke({x,y},{x-sx*8,y}); Stroke({x,y},{x,y-sy*8});
    }
    DrawRectangle(711,36,8,13,WHITE);
    DrawTriangle({718,36},{718,49},{730,57},WHITE);
    DrawTriangle({718,36},{730,57},{730,28},WHITE);
    if(muted) {Stroke({736,36},{746,49},2.5f);Stroke({746,36},{736,49},2.5f);}
    else DrawRing({726,42},15,18,-58,58,20,WHITE);
    Stroke({766,33},{784,51}); Stroke({784,33},{766,51});
}
void UI::DrawOverlay(State state,int score,int best) const {
    if(state==State::PLAYING) return;
    if(state==State::READY) {
        DrawRectangleRounded({194,674,420,57},0.45f,12,Fade(cfg::HudColor,0.9f));
        CenterText("SPACE or a direction key to start",404,688,20,WHITE);
        return;
    }
    float fade=std::clamp(overlayAge/0.22f,0.0f,1.0f);
    DrawRectangle(0,cfg::Hud,cfg::Width,cfg::Height-cfg::Hud,Fade(Color{23,45,16,255},0.46f*fade));
    DrawRectangleRounded({213,253,382,330},0.12f,16,Fade(Color{24,47,16,255},0.18f*fade));
    DrawRectangleRounded({213,245,382,330},0.12f,16,Fade(Color{244,250,225,255},fade));
    const char* title=state==State::PAUSED?"Paused":state==State::VICTORY?"Victory!":"Game Over";
    CenterText(title,404,277,40,Fade(cfg::HudColor,fade));
    CenterText(state==State::PAUSED?"Take a little breather.":state==State::VICTORY?"Every square. Every apple.":"A fresh start is one click away.",404,332,18,Fade(cfg::Ink,fade));
    CenterText(TextFormat("SCORE  %d     BEST  %d",score,best),404,382,24,Fade(cfg::HudColor,fade));
    for(int i=3;i<5;++i) {
        Rectangle r=buttons[i];
        if(CheckCollisionPointRec(mouse,r)&&IsMouseButtonDown(MOUSE_BUTTON_LEFT)) r.y+=2;
        Color c=i==3?cfg::HudColor:Color{219,233,193,255};
        c=ColorBrightness(c,hover[i]*0.12f);
        DrawRectangleRounded(r,0.3f,12,Fade(c,fade));
        const char* label=i==3?(state==State::PAUSED?"Resume":"Play Again"):(state==State::PAUSED?"Restart":"Exit");
        CenterText(label,404,r.y+15,23,Fade(i==3?WHITE:cfg::HudColor,fade));
    }
}
