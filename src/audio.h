//
// Created by The Code Father on 4/6/2026.
//

#ifndef LASTECHO_AUDIO_SOUND_EFFECTS_H
#define LASTECHO_AUDIO_SOUND_EFFECTS_H
#pragma once
#include <iostream>
#include <SFML/Audio.hpp>

class AudioManager {
public:
    AudioManager(); // ده اللي بيحمل المزيكا
    std::string currentMode = "menu"; // ده اللي هيعرفنا إحنا فين

    void playMusic(std::string type) ;// دالة واحدة بتبدل بين المنيو والجيم بلاي
    void stopMusic();
    void setVolume(float volume); // دالة لتغيير مستوى الصوت
    void startFootsteps();
    void stopFootsteps();
    void playTypeSound();
    void playpickupSound();
    void playClickSound();
    void playhoverSound();
private:

    sf::Music bgm;
    sf::SoundBuffer footstepsBuffer;
    sf::SoundBuffer typingBuffer;
    sf::SoundBuffer pickupBuffer , clickBuffer , hoverBuffer;
    sf::Sound pickupSound , clickSound , hoverSound ;
    sf::Sound footsteps;
    sf::Sound typingSound;
};
extern AudioManager audioManager;

#endif //LASTECHO_AUDIO_SOUND_EFFECTS_H