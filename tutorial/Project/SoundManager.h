#include <sox.h>
#include <memory>
#include <chrono>
#include <ctime>

#ifndef ENIGEREWORK_SOUNDMANAGER_H
#define ENIGEREWORK_SOUNDMANAGER_H

const long MILLISECONDS_IN_SECOND = 1000;
long CurrentUnixTime();
class Sound{
private:
    std::string soundFile;
    long endTime;
    int times;
    static long GetDuration(const std::string& soundFile);
public:
    Sound(std::string soundFile, int times);
    Sound(std::string soundFile);
};
class SoundManager {
private:
    static std::shared_ptr<SoundManager> instance;
    static bool IsInitialized();
    bool running;
    void ManageSounds();
    SoundManager();
public:
    void PlayOneShot(std::string soundFile);
    void PlayLoop(std::string soundFile, int times);
    void PlayLoop(std::string soundFile);
    void StopSound(std::string soundFile);
    static std::shared_ptr<SoundManager> GetInstance();
    static void Start();
    static void Kill();

};


#endif //ENIGEREWORK_SOUNDMANAGER_H
