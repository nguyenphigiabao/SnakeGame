#include "animation.h"
#include "apple.h"
#include <algorithm>
#include <cmath>
void Effects::Burst(Vector2 p) {
    eatenPosition=p; eatenTime=0.22f; appleAge=0; scorePulse=1;
    for (int i=0;i<14;++i) {
        Particle* slot=nullptr;
        for (auto& particle:particles) if (particle.life<=0) {slot=&particle; break;}
        if (!slot) break;
        float angle=GetRandomValue(0,628)/100.0f, speed=static_cast<float>(GetRandomValue(45,145));
        *slot={p,{std::cos(angle)*speed,std::sin(angle)*speed},0.5f,0.5f,
               static_cast<float>(GetRandomValue(2,5)),i%3==0?cfg::Trophy:cfg::Apple};
    }
}
void Effects::Update(float dt) {
    appleAge+=dt; eatenTime=std::max(0.0f,eatenTime-dt); scorePulse=std::max(0.0f,scorePulse-dt*4);
    for (auto& p:particles) if (p.life>0) {
        p.life-=dt; p.position.x+=p.velocity.x*dt; p.position.y+=p.velocity.y*dt; p.velocity.y+=180*dt;
    }
}
void Effects::Draw() const {
    if (eatenTime>0) DrawApple(eatenPosition,0.5f+eatenTime/0.22f*0.5f,eatenTime/0.22f);
    for (const auto& p:particles) if (p.life>0) DrawCircleV(p.position,p.radius*p.life/p.duration,Fade(p.color,p.life/p.duration));
}
