//
// Created by The Code Father on 4/6/2026.
//

#ifndef LASTECHO_AUDIO_SOUND_EFFECTS_H
#define LASTECHO_AUDIO_SOUND_EFFECTS_H
#pragma once
#include <iostream>
#include <SFML/Audio.hpp>

struct AudioManager {
    AudioManager(); //الconstructor اللي بيحمل الاصوات


    void playMusic(std::string type) ;// دالة واحدة بتبدل بين المنيو والجيم بلاي
    void setVolume(float volume);
    void startFootsteps();
    void stopFootsteps();
    void playTypingSound();
    void playpickupSound();
    void playClickSound();
    void playhoverSound();


    sf::Music bgm;
    std::string currentMode = "menu";  // ده اللي هيعرفنا احنا بنلعب ولا في الmainmenu
    std::string filePath;  //الpath بتاع الmusic

    sf::SoundBuffer footstepsBuffer;
    sf::SoundBuffer typingBuffer;
    sf::SoundBuffer pickupBuffer , clickBuffer , hoverBuffer;

    sf::Sound pickupSound , clickSound , hoverSound ;
    sf::Sound footsteps;
    sf::Sound typingSound;

};
extern AudioManager audioManager;

#endif //LASTECHO_AUDIO_SOUND_EFFECTS_H