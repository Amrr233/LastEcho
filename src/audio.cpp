#include "audio.h"
AudioManager audioManager;

AudioManager::AudioManager() {
    // تحميل صوت الخطوات مرة واحدة في البداية
    if (!footstepsBuffer.loadFromFile("assets/audio/footsteps.wav")) {
        std::cout << "Error loading footsteps.wav" << std::endl;
    }
    if (!typingBuffer.loadFromFile("assets/audio/dialogue.wav")) {
        std::cout << "Error loading typing sound!" << std::endl;
    }
    if (!pickupBuffer.loadFromFile("assets/audio/pickupsound.wav")) {
        std::cout << "Error loading pickup.wav" << std::endl;
    }
    if (!clickBuffer.loadFromFile("assets/audio/clicksound.wav")) {
        std::cout << "Error loading click.wav" << std::endl;
    }
    if (!hoverBuffer.loadFromFile("assets/audio/hoversound.wav")) {
        std::cout << "Error loading hover.wav" << std::endl;
    }
    pickupSound.setBuffer(pickupBuffer);
    hoverSound.setBuffer(hoverBuffer);
    clickSound.setBuffer(clickBuffer);
    typingSound.setBuffer(typingBuffer);
    footsteps.setBuffer(footstepsBuffer);
    footsteps.setLoop(true);

}

void AudioManager::playMusic(std::string type) {
    //footsteps.play();
    bgm.stop();
    currentMode = type;

    if (type == "menu") {
        filePath = "assets/audio/main theme.ogg";
    } else if (type == "game") {
        filePath = "assets/audio/GPM.mp3";
    }
    if (bgm.openFromFile(filePath)) {
        bgm.setLoop(true);
        bgm.setVolume(40.f); //دي بتظبط الGPM
        bgm.play();
    } else {
        std::cout << "Error loading music file: " << filePath << std::endl;
    }
}

void AudioManager::setMusicVolume(float volume) {
    if (currentMode == "game") {
        bgm.setVolume(volume);
    }
    else {
        bgm.setVolume(volume * 1.f); // دي بتظبط الmain theme
    }
}

void AudioManager::setSFXVolume(float volume) {
    footsteps.setVolume(volume * 0.5f); // الخطوات دايماً أهدى شوية من المزيكا
    typingSound.setVolume(volume);
    hoverSound.setVolume(volume*0.5f);
    clickSound.setVolume(volume*3.f);
    pickupSound.setVolume(volume*0.8f);
}

void AudioManager::startFootsteps() {
    if (footsteps.getStatus() != sf::Sound::Status::Playing) { //بنتاكد ان الصوت مش شغال قبل كده عشان ميمعلش restart كل شويه
        footsteps.setPitch(1.4f); // السرعة اللي تليق مع مشية اللاعب
        footsteps.play();
    }
}

void AudioManager::stopFootsteps() {
    if (footsteps.getStatus() == sf::Sound::Status::Playing) {
        footsteps.stop();
    }
}

void AudioManager::playTypingSound() {
    typingSound.setPitch(1.2f);
    typingSound.play();
}

void AudioManager::playpickupSound() {
    pickupSound.setPitch(1.1f);
    pickupSound.play();
}

void AudioManager::playClickSound() {
    clickSound.setPitch(1.1f);
    clickSound.play();
}

void AudioManager::playhoverSound() {
    hoverSound.setPitch(3.f);
    hoverSound.play();
}