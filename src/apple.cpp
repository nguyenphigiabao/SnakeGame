#include "apple.h"
bool SpawnApple(const Snake& snake, Cell& apple, std::mt19937& random) {
    std::array<Cell,cfg::Capacity> freeCells{};
    int count=0;
    for (int y=1;y<=cfg::Rows;++y)
        for (int x=1;x<=cfg::Columns;++x)
            if (!snake.Occupies({x,y})) freeCells[count++]={x,y};
    if (!count) return false;
    apple=freeCells[std::uniform_int_distribution<int>(0,count-1)(random)];
    return true;
}
void DrawApple(Vector2 p, float s, float opacity) {
    if (s<=0) return;
    auto color=[&](Color c) {return Fade(c,opacity*c.a/255.0f);};
    DrawEllipse(static_cast<int>(p.x+1*s),static_cast<int>(p.y+17*s),16*s,6*s,color({67,105,34,45}));
    DrawLineEx({p.x,p.y-12*s},{p.x+1*s,p.y-24*s},3*s,color({112,78,29,255}));
    DrawEllipse(static_cast<int>(p.x+7*s),static_cast<int>(p.y-23*s),9*s,4*s,color(cfg::Leaf));
    DrawCircleV({p.x,p.y},18*s,color(cfg::Apple));
    DrawEllipse(static_cast<int>(p.x-6*s),static_cast<int>(p.y-8*s),4*s,6*s,color({255,143,91,230}));
}
