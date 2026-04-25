#include "audio.h"

AudioManager::AudioManager() {
    if (bgm.openFromFile("assets/audio/BGM.mpeg")) { // أتأكد إن اسم التراك صح عندك
        bgm.setLoop(true);
        bgm.setVolume(5.f);
    }

    if (footstepsBuffer.loadFromFile("assets/audio/footsteps.wav")) {
        footsteps.setBuffer(footstepsBuffer);
        footsteps.setLoop(true);
        footsteps.setVolume(100.f);
    }
}

void AudioManager::playBGM() {
    bgm.play();
}
void AudioManager::setVolume(float volume) {
    bgm.setVolume(volume); // بتغير صوت المزيكا للرقم الجديد
    footsteps.setVolume(volume*0.5f);//بيغير صوت الخطوات عشان يبقى ماشي مع المزيكا
}
void AudioManager::startFootsteps() {
    if (footsteps.getStatus() != sf::Sound::Status::Playing) {//لو الصوت مش شغال
        footsteps.setPitch(1.5);//بيظبط الصوت مع خطوات اللاعب
        footsteps.play();

    }
}
void AudioManager::stopFootsteps() {
    footsteps.stop();
}
AudioManager audioManager;