//
// Created by The Code Father on 4/6/2026.
//

#ifndef LASTECHO_AUDIO_SOUND_EFFECTS_H
#define LASTECHO_AUDIO_SOUND_EFFECTS_H
#pragma once
#include <SFML/Audio.hpp>

class AudioManager {
public:
    AudioManager(); // ده اللي بيحمل المزيكا
    void playBGM(); // ده اللي بيشغلها
    void setVolume(float volume); // دالة لتغيير مستوى الصوت
private:
    sf::Music bgm;
};
#endif //LASTECHO_AUDIO_SOUND_EFFECTS_H