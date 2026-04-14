#include "audio.h"

AudioManager::AudioManager() {
    if (bgm.openFromFile("assets/audio/BGM.mpeg")) { // أتأكد إن اسم التراك صح عندك
        bgm.setLoop(true);
        bgm.setVolume(5.f);
    }
}

void AudioManager::playBGM() {
    bgm.play();
}
void AudioManager::setVolume(float volume) {
    bgm.setVolume(volume); // بتغير صوت المزيكا للرقم الجديد
}