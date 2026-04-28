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
    footsteps.setVolume(10.f);
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
    typingSound.setVolume(volume);
    hoverSound.setVolume(volume*0.5f);
    clickSound.setVolume(volume*3.f);
    pickupSound.setVolume(volume*0.8f);
}

void AudioManager::startFootsteps() {
    // بنشيك لو الصوت مش شغال أصلاً عشان ميعملش "Restart" مع كل خطوة
    if (footsteps.getStatus() != sf::Sound::Status::Playing) {
        footsteps.setPitch(1.5f); // السرعة اللي تليق مع مشية اللاعب في الفيديو
        footsteps.play();
    }
}

void AudioManager::stopFootsteps() {
    if (footsteps.getStatus() == sf::Sound::Status::Playing) {
        footsteps.stop();
    }
}

void AudioManager::playTypeSound() {
    typingSound.setPitch(1.2f);
    // 3. الحركة دي بتخلي الصوت "يقطع" نفسه ويبدأ من جديد بسرعة
    if (typingSound.getStatus() == sf::Sound::Status::Playing) {
        typingSound.stop();
    }

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