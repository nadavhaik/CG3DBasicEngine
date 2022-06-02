//
// Created by ditto on 26/05/2022.
//

#include "SoundManager.h"

#include <utility>

long CurrentUnixTime(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}


long Sound::GetDuration(const std::string& soundFile) {
    sox_format_t *fmt;
    fmt = sox_open_read(soundFile.c_str(), nullptr, nullptr, nullptr);
    if(!fmt->signal.length) {
        delete fmt;
        throw std::invalid_argument("Cannot determine duration!");
    }
    long dur = (long) (MILLISECONDS_IN_SECOND * ((double)fmt->signal.length / fmt->signal.channels) / fmt->signal.rate);
    delete fmt;

    return dur;
}
Sound::Sound(std::string soundFile):
    soundFile(std::move(soundFile)), times(-1), endTime(CurrentUnixTime() + GetDuration(soundFile)) {}
Sound::Sound(std::string soundFile, int times):
        soundFile(std::move(soundFile)), times(times), endTime(CurrentUnixTime() + GetDuration(soundFile)) {}
