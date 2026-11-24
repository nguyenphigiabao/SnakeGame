#include "audio.h"
#include <cmath>
#include <array>
void Audio::Init() {
    InitAudioDevice(); ready=IsAudioDeviceReady();
    if (!ready) return;
    constexpr int rate=22050, count=8820;
    std::array<short,count> samples{};
    const float frequencies[]={740,180,520,880,420};
    for (int effect=0;effect<static_cast<int>(Cue::Count);++effect) {
        int frames=effect==1||effect==3 ? count : count/2;
        float phase=0;
        for (int i=0;i<frames;++i) {
            float t=static_cast<float>(i)/rate, progress=static_cast<float>(i)/frames;
            float frequency=frequencies[effect];
            if (effect==1) frequency*=1-progress*0.65f;
            else if (effect==3) frequency*=1+static_cast<int>(progress*3)*0.25f;
            else frequency*=1+progress*0.3f;
            phase+=2*PI*frequency/rate;
            float envelope=std::fmin(t*60,1.0f)*(1-progress)*(1-progress);
            samples[i]=static_cast<short>(std::sin(phase)*envelope*9500);
        }
        Wave wave{static_cast<unsigned int>(frames),rate,16,1,samples.data()};
        sounds[effect]=LoadSoundFromWave(wave);
    }
}
void Audio::Play(Cue cue) {
    Sound sound=sounds[static_cast<int>(cue)];
    if (ready&&!muted&&IsSoundValid(sound)) PlaySound(sound);
}
void Audio::Shutdown() {
    if (!ready) return;
    for (auto sound:sounds) if (IsSoundValid(sound)) UnloadSound(sound);
    CloseAudioDevice(); ready=false;
}
