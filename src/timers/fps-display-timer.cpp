#include <timers.hpp>

#include <SDL_timer.h>


void FPSDisplayTimer::calculateFPS() {
    mAverageFPS = mAccumulatedFrames / (getTicks() / 1000.0f);
    if (mAccumulatedFrames >= std::numeric_limits<unsigned long long int>::max()) mAccumulatedFrames = 0;
    ++mAccumulatedFrames;
}