#include "audio.h"
AudioManager audioManager;

AudioManager::AudioManager() {
    // تحميل صوت الخطوات مرة واحدة في البداية
    if (!footstepsBuffer.loadFromFile("assets/audio/footsteps.wav")) {
        std::cout << "Error loading footsteps.wav" << std::endl;
    }
    if (!typingBuffer.loadFromFile("assets/audio/378083__bigmonmulgrew__mechanical-key-soft (mp3cut.net).wav")) {
        std::cout << "Error loading typing sound!" << std::endl;
    }
    typingSound.setBuffer(typingBuffer);
    footsteps.setBuffer(footstepsBuffer);
    footsteps.setLoop(true);
    footsteps.setVolume(10.f); // خليها واضحة بس مش أعلى من المزيكا
}

void AudioManager::playMusic(std::string type) {
    bgm.stop();
    currentMode = type;

    std::string filePath;
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

void AudioManager::stopMusic() {
    bgm.stop();
}

void AudioManager::setVolume(float volume) {
    footsteps.setVolume(volume * 0.5f); // الخطوات دايماً أهدى شوية من المزيكا
    if (currentMode == "game") {
     //ملهاش لازمه
        bgm.setVolume(volume*0.f);
    }
    else {
        // دي بتظبط الmain theme
        bgm.setVolume(volume * 1.f);
    }
    typingSound.setVolume(volume * 0.4f);
}

void AudioManager::startFootsteps() {
    // بنشيك لو الصوت مش شغال أصلاً عشان ميعملش "Restart" مع كل خطوة
    if (footsteps.getStatus() != sf::Sound::Status::Playing) {
        footsteps.setPitch(1.5f); // السرعة اللي تليق مع مشية البطل في الفيديو
        footsteps.play();
    }
}

void AudioManager::stopFootsteps() {
    if (footsteps.getStatus() == sf::Sound::Status::Playing) {
        footsteps.stop();
    }
}
void AudioManager::playTypeSound() {
    float randomPitch = 2.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.5f));
    typingSound.setPitch(randomPitch);


    typingSound.play();
}