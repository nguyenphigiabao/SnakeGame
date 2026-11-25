#include "ui.h"
#include "apple.h"
#include <algorithm>
#include <string>
namespace {
Font uiFont{};
bool ownedFont=false;
Font TextFont() { return ownedFont?uiFont:GetFontDefault(); }
// 0-2 HUD, 3-4 pause/game-over card, 5-6 mode selection (Easy, Hard).
constexpr Rectangle buttons[]={{654,17,44,50},{704,17,44,50},{754,17,44,50},{264,443,280,54},{264,511,280,48},
                               {214,318,380,100},{214,436,380,100}};
constexpr int ButtonCount=static_cast<int>(sizeof(buttons)/sizeof(buttons[0]));
bool Enabled(int i,State state) {
    if(i<3) return true;
    if(i<5) return state==State::PAUSED||state==State::GAME_OVER||state==State::VICTORY;
    return state==State::MENU;
}
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
void DrawHeart(Vector2 c,float s,Color color) {
    DrawCircleV({c.x-s*0.5f,c.y-s*0.22f},s*0.56f,color);
    DrawCircleV({c.x+s*0.5f,c.y-s*0.22f},s*0.56f,color);
    DrawTriangle({c.x-s*1.04f,c.y-s*0.08f},{c.x,c.y+s*0.98f},{c.x+s*1.04f,c.y-s*0.08f},color);
}
void UI::Shutdown() { if(ownedFont) UnloadFont(uiFont); ownedFont=false; }
void CenterText(const char* text,float x,float y,float size,Color color) {
    Vector2 measure=MeasureTextEx(TextFont(),text,size,1);
    DrawTextEx(TextFont(),text,{x-measure.x/2,y},size,1,color);
}
Action UI::Update(float dt,Vector2 position,State state) {
    mouse=position; overlayAge+=dt;
    Action action=Action::None;
    for (int i=0;i<ButtonCount;++i) {
        bool over=Enabled(i,state)&&CheckCollisionPointRec(mouse,buttons[i]);
        hover[i]+=(static_cast<float>(over)-hover[i])*std::min(1.0f,dt*14);
        if(over&&i>=5) menuChoice=i-5;
    }
    static int pressed=-1;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        pressed=-1;
        for(int i=0;i<ButtonCount;++i) if (Enabled(i,state)&&CheckCollisionPointRec(mouse,buttons[i])) pressed=i;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if (pressed>=0&&Enabled(pressed,state)&&CheckCollisionPointRec(mouse,buttons[pressed])) {
            switch(pressed) {
            case 0: action=Action::Fullscreen; break;
            case 1: action=Action::Mute; break;
            case 2: action=Action::Exit; break;
            case 3: action=state==State::PAUSED?Action::Resume:Action::Restart; break;
            case 4: action=state==State::PAUSED?Action::Restart:Action::Menu; break;
            case 5: action=Action::Easy; break;
            case 6: action=Action::Hard; break;
            }
        }
        pressed=-1;
    }
    bool pointing=false;
    for (float h:hover) if (h>0.5f) pointing=true;
    SetMouseCursor(pointing?MOUSE_CURSOR_POINTING_HAND:MOUSE_CURSOR_DEFAULT);
    return action;
}
void UI::DrawHud(const HudInfo& info) const {
    const int score=info.score,best=info.best; const float pulse=info.pulse; const bool muted=info.muted;
    DrawRectangle(0,0,cfg::Width,cfg::Hud,cfg::HudColor);
    DrawApple({47,45},0.9f);
    DrawTextEx(TextFont(),TextFormat("%d",score),{77,30-pulse*2},30+pulse*5,1,WHITE);
    Trophy({174,40});
    DrawTextEx(TextFont(),TextFormat("%d",best),{205,30},30,1,WHITE);
    if(info.state!=State::MENU) {
        // Lives sit right after the record: 3 hearts in Easy, 1 in Hard.
        int maxLives=info.mode==Mode::Easy?cfg::EasyLives:1;
        float left=std::max(292.0f,215+MeasureTextEx(TextFont(),TextFormat("%d",best),30,1).x+40);
        for(int i=0;i<maxLives;++i) {
            Vector2 c{left+i*36.0f,43};
            DrawHeart({c.x,c.y+2},11,Fade(BLACK,0.12f));
            if(i<info.lives) DrawHeart(c,11,cfg::Heart);
            else {
                DrawHeart(c,11,Fade(WHITE,0.18f));
                if(i==info.lives&&info.heartPulse>0) // The heart just lost pops and fades.
                    DrawHeart({c.x,c.y-(1-info.heartPulse)*10},11*(1+(1-info.heartPulse)*0.5f),Fade(cfg::Heart,info.heartPulse));
            }
        }
    }
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
void UI::DrawOverlay(const HudInfo& info,bool respawned) const {
    const State state=info.state; const int score=info.score,best=info.best;
    if(state==State::PLAYING) return;
    if(state==State::MENU) {
        float fade=std::clamp(overlayAge/0.22f,0.0f,1.0f);
        DrawRectangle(0,cfg::Hud,cfg::Width,cfg::Height-cfg::Hud,Fade(Color{23,45,16,255},0.46f*fade));
        DrawRectangleRounded({174,196,460,440},0.1f,16,Fade(Color{24,47,16,255},0.18f*fade));
        DrawRectangleRounded({174,188,460,440},0.1f,16,Fade(Color{244,250,225,255},fade));
        CenterText("Snake",404,214,44,Fade(cfg::HudColor,fade));
        CenterText("Choose your challenge",404,270,19,Fade(cfg::Ink,fade));
        for(int i=5;i<7;++i) {
            Rectangle r=buttons[i];
            if(CheckCollisionPointRec(mouse,r)&&IsMouseButtonDown(MOUSE_BUTTON_LEFT)) r.y+=2;
            bool selected=menuChoice==i-5;
            float lift=std::max(hover[i],selected?1.0f:0.0f);
            Color fill=selected?cfg::HudColor:ColorBrightness(Color{219,233,193,255},hover[i]*0.06f);
            DrawRectangleRounded(r,0.22f,12,Fade(fill,fade));
            if(selected) DrawRectangleRoundedLinesEx({r.x-4,r.y-4,r.width+8,r.height+8},0.24f,12,3,Fade(cfg::Trophy,fade*lift));
            Color text=selected?WHITE:cfg::HudColor, sub=selected?Color{222,240,200,255}:cfg::Ink;
            CenterText(i==5?"Easy Mode":"Hard Mode",404,r.y+16,30,Fade(text,fade));
            // Hearts plus caption, centred together under the title.
            int hearts=i==5?cfg::EasyLives:1;
            const char* caption=i==5?"3 lives - keep your score":"1 life - no second chances";
            float textWidth=MeasureTextEx(TextFont(),caption,19,1).x;
            float total=hearts*22.0f+8+textWidth, x=404-total/2;
            for(int h=0;h<hearts;++h) DrawHeart({x+11+h*22.0f,r.y+69},8,Fade(cfg::Heart,fade));
            DrawTextEx(TextFont(),caption,{x+hearts*22.0f+8,r.y+59},19,1,Fade(sub,fade));
        }
        CenterText("Click, or W/S + ENTER  -  keys 1 / 2",404,568,17,Fade(cfg::Ink,0.8f*fade));
        return;
    }
    if(state==State::READY) {
        if(respawned) {
            DrawRectangleRounded({174,650,460,92},0.35f,12,Fade(cfg::HudColor,0.92f));
            CenterText(info.lives==1?"Ouch! Last life left":TextFormat("Ouch! %d lives left",info.lives),404,662,26,WHITE);
            CenterText("SPACE or a direction key to continue",404,704,19,Color{222,240,200,255});
            return;
        }
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
        const char* label=i==3?(state==State::PAUSED?"Resume":"Play Again"):(state==State::PAUSED?"Restart":"Change Mode");
        CenterText(label,404,r.y+15,23,Fade(i==3?WHITE:cfg::HudColor,fade));
    }
}
