// Code obtained from the top answer in the stackoverflow web page,
// Beep function is only available in Windows and I'm on Mac LOL
//https://stackoverflow.com/questions/10110905/simple-sound-wave-generator-with-sdl-in-c
#ifndef BEEP
#define BEEP
#include <queue>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

const int AMPLITUDE = 28000;
const int FREQUENCY = 44100;
const double DEFFREQ = 440;
const double SEMITONE = std::pow(2, 1.0/12);

struct BeepObject
{
    double freq;
    int samplesLeft;
};

class Beeper
{
private:
    double v;
    std::queue<BeepObject> beeps;
public:
    Beeper();
    ~Beeper();
    void beep(double freq, int duration);
    void generateSamples(Sint16 *stream, int length);
    void wait();
};
#endif