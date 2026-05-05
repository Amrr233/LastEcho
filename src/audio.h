//
// Created by The Code Father on 4/6/2026.
//

#ifndef LASTECHO_AUDIO_SOUND_EFFECTS_H
#define LASTECHO_AUDIO_SOUND_EFFECTS_H
#pragma once
#include <iostream>
#include <SFML/Audio.hpp>

struct AudioManager {
    AudioManager();
    void playMusic(std::string type) ;
    void setMusicVolume(float volume);
    void setSFXVolume(float volume);
    void startFootsteps();
    void stopFootsteps();
    void playTypingSound();
    void playpickupSound();
    void playClickSound();
    void playhoverSound();


    sf::Music bgm;
    std::string currentMode = "menu";
    std::string filePath;

    sf::SoundBuffer footstepsBuffer;
    sf::SoundBuffer typingBuffer;
    sf::SoundBuffer pickupBuffer , clickBuffer , hoverBuffer;

    sf::Sound pickupSound , clickSound , hoverSound ;
    sf::Sound footsteps;
    sf::Sound typingSound;

};
extern AudioManager audioManager;

#endif //LASTECHO_AUDIO_SOUND_EFFECTS_H