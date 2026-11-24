#include "snake.h"
#include <algorithm>
#include <cmath>
Cell DirectionDelta(Direction d) {
    switch (d) {
    case Direction::Right: return {1,0};
    case Direction::Down: return {0,1};
    case Direction::Left: return {-1,0};
    case Direction::Up: return {0,-1};
    }
    return {1,0};
}
void Snake::Reset() {
    length = 3; turnCount = 0;
    direction = previousDirection = Direction::Right;
    cells[0] = {4,8}; cells[1] = {3,8}; cells[2] = {2,8};
    previous = cells;
}
bool Snake::Occupies(Cell c, int count) const {
    if (count < 0) count = length;
    for (int i=0; i<count; ++i) if (cells[i] == c) return true;
    return false;
}
void Snake::Queue(Direction next) {
    Direction last = turnCount ? turns[turnCount-1] : direction;
    if (next == last || (static_cast<int>(next)+2)%4 == static_cast<int>(last)) return;
    if (turnCount < static_cast<int>(turns.size())) turns[turnCount++] = next;
}
MoveResult Snake::Step(Cell apple) {
    Direction nextDirection = turnCount ? turns[0] : direction;
    Cell delta = DirectionDelta(nextDirection);
    Cell next{cells[0].x + delta.x, cells[0].y + delta.y};
    bool eating = next == apple;
    // On a normal move the old tail is vacated in the same tick.
    if (!Inside(next) || Occupies(next, length - (eating ? 0 : 1))) return MoveResult::Collision;
    previous = cells;
    previousDirection = direction;
    direction = nextDirection;
    if (turnCount) {
        for (int i=1; i<turnCount; ++i) turns[i-1] = turns[i];
        --turnCount;
    }
    if (eating && length < cfg::Capacity) {
        previous[length] = cells[length-1]; // New tail grows from the old endpoint.
        ++length;
    }
    for (int i=length-1; i>0; --i) cells[i] = cells[i-1];
    cells[0] = next;
    if (length == cfg::Capacity) return MoveResult::Victory;
    return eating ? MoveResult::Ate : MoveResult::Moved;
}
void DrawSnake(const Snake& s, float alpha) {
    alpha = std::clamp(alpha, 0.0f, 1.0f);
    std::array<Vector2, cfg::Capacity> points{};
    for (int i=0; i<s.length; ++i) {
        Vector2 a=Center(s.previous[i]), b=Center(s.cells[i]);
        points[i]={a.x+(b.x-a.x)*alpha, a.y+(b.y-a.y)*alpha};
    }
    // Overlapping round joints make a continuous capsule stroke at every turn.
    for (int pass=0; pass<2; ++pass) {
        Color color = pass ? cfg::Body : Color{130,174,62,255};
        float offset = pass ? 0.0f : 5.0f;
        for (int i=s.length-1; i>=0; --i) {
            Vector2 p{points[i].x,points[i].y+offset};
            if (i) DrawLineEx(p,{points[i-1].x,points[i-1].y+offset},cfg::Radius*2,color);
            DrawCircleV(p,cfg::Radius,color);
        }
    }
    Vector2 head=points[0];
    DrawCircleV(head,cfg::Radius+1,cfg::Head);
    float a=static_cast<int>(s.previousDirection)*PI/2;
    float b=static_cast<int>(s.direction)*PI/2;
    float difference=std::atan2(std::sin(b-a), std::cos(b-a));
    float angle=a+difference*(alpha*alpha*(3-2*alpha));
    Vector2 forward{std::cos(angle),std::sin(angle)}, side{-forward.y,forward.x};
    auto local = [&](float x,float y) {return Vector2{head.x+forward.x*x+side.x*y,head.y+forward.y*x+side.y*y};};
    for (float sign : {-1.0f,1.0f}) {
        DrawCircleV(local(-3,sign*15.5f),13,cfg::Eye);
        DrawCircleV(local(-3,sign*15.5f),10.5f,RAYWHITE);
        DrawCircleV(local(0.5f,sign*15.5f),5.5f,cfg::Pupil);
        DrawCircleV(local(1.5f,sign*15.5f-2),1.6f,WHITE);
        DrawCircleV(local(12,sign*6),2.1f,Color{44,82,176,255});
    }
}
