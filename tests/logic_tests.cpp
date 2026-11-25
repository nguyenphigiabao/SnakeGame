#include "game.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cstdio>
int checks=0;
void Check(bool condition,const char* description) {
    ++checks;
    if(!condition) { std::cerr<<"FAIL: "<<description<<'\n'; std::exit(1); }
}
int main() {
    Snake s; s.Reset();
    Check(s.length==3&&s.cells[0]==Cell{4,8},"initial snake");
    s.Queue(Direction::Left); Check(s.turnCount==0,"reject reversal");
    s.Queue(Direction::Up); s.Queue(Direction::Right); s.Queue(Direction::Left); s.Queue(Direction::Down);
    Check(s.turnCount==2,"keep the imminent turn and latest legal follow-up");
    Check(s.Step({17,15})==MoveResult::Moved&&s.cells[0]==Cell{4,7},"one queued turn per tick");
    Check(s.Step({17,15})==MoveResult::Moved&&s.cells[0]==Cell{3,7},"preserve rapid turn order");
    s.Reset(); Check(s.Step({5,8})==MoveResult::Ate&&s.length==4,"growth");
    Check(s.previous[3]==Cell{2,8}&&s.cells[3]==Cell{2,8},"growth anchors tail interpolation");
    s.Reset(); s.cells[0]={17,8};
    Check(s.Step({1,1})==MoveResult::Collision&&s.cells[0]==Cell{17,8},"wall collision keeps valid state");
    s.Reset(); s.length=4; s.direction=Direction::Up;
    s.cells[0]={2,2}; s.cells[1]={2,3}; s.cells[2]={1,3}; s.cells[3]={1,2};
    s.Queue(Direction::Left);
    Check(s.Step({17,15})==MoveResult::Moved&&s.cells[0]==Cell{1,2},"enter vacated tail");
    s.Reset(); s.length=5; s.direction=Direction::Up;
    s.cells[0]={2,2}; s.cells[1]={2,3}; s.cells[2]={1,3}; s.cells[3]={1,2}; s.cells[4]={1,1};
    s.Queue(Direction::Left); Check(s.Step({17,15})==MoveResult::Collision,"self collision");
    std::mt19937 random(42); Cell apple;
    s.Reset();
    for(int i=0;i<1000;++i) Check(SpawnApple(s,apple,random)&&Inside(apple)&&!s.Occupies(apple),"spawn in empty cell");
    // A contiguous serpentine path fills every square except (1,1).
    std::array<Cell,cfg::Capacity> path{}; int n=0;
    for(int y=1;y<=cfg::Rows;++y) for(int x=1;x<=cfg::Columns;++x)
        path[n++]={y%2?x:cfg::Columns+1-x,y};
    s.length=cfg::Capacity-1; s.turnCount=0; s.direction=Direction::Left;
    for(int i=0;i<s.length;++i) s.cells[i]=path[i+1];
    Check(SpawnApple(s,apple,random)&&apple==Cell{1,1},"only remaining square");
    Check(s.Step(apple)==MoveResult::Victory&&s.length==cfg::Capacity,"victory at exact capacity");
    Check(!SpawnApple(s,apple,random),"full board spawn terminates");
    const std::string file="test-highscore.txt";
    std::remove(file.c_str()); Check(ReadHighScore(file)==0,"missing score");
    for(const char* bad:{"garbage","-1","253","12garbage","99999999999999999999",""}) {
        {std::ofstream f(file);f<<bad;}
        Check(ReadHighScore(file)==0,"invalid score");
    }
    Check(WriteHighScore(file,11)&&ReadHighScore(file)==11,"score write/read");
    Check(WriteHighScore(file,25)&&ReadHighScore(file)==25,"replace saved score");
    Check(!WriteHighScore(file,-1)&&ReadHighScore(file)==25,"invalid write preserves score");
    std::remove(file.c_str());
    std::cout<<"PASS: "<<checks<<" checks\n";
}
