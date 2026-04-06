#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Data.h"
#include "MainMenu.h"
#include "settings.h"
#include <cmath>
using namespace sf;

settingsMenu settings;
void volumeup(float &currentVolume) {
    if (currentVolume >= 100.0f) {
        // currentVolume = 100.0f; // Cap it at max
    } else {
        currentVolume += 5.0f;
    }
    sf::Listener::setGlobalVolume(currentVolume); // Updates the actual game sound
}

void volumedown(float &currentVolume) {
    if (currentVolume <= 0.0f) {
        currentVolume = 0.0f; // Cap it at min
    } else {
        currentVolume -= 5.0f; // Fixed the -= syntax
    }
    sf::Listener::setGlobalVolume(currentVolume); // Updates the actual game sound
}
// 1. تعريف الكائن (عشان الـ extern يشتغل)
void settingsMenu::init(float W, float H) {
    // 1. تحميل الصور من الملفات
    bgTex.loadFromFile("assets/settings bg.png");
    volbar_tex.loadFromFile("assets/volumebar.png"); // الصورة اللي قصيتها
    sliderTexture.loadFromFile("assets/slider.png");

    // 2. ربط الـ Sprites بالـ Textures (دي الخطوة اللي ناقصاك)
    settings_window.setTexture(bgTex);
    volumeBar.setTexture(volbar_tex);
    sliderHandle.setTexture(sliderTexture);

    // 3. تظبيط مكان اللوحة الخشب (في نص الشاشة)
    settings_window.setOrigin(bgTex.getSize().x / 2.f, bgTex.getSize().y / 2.f);
    settings_window.setPosition(W / 2.f, H / 2.f);

    // 4. تظبيط مكان الـ Slider فوق اللوحة
    volumeBar.setOrigin(volbar_tex.getSize().x / 2.f, volbar_tex.getSize().y / 2.f);
    volumeBar.setPosition(W / 2.f, H / 2.f); // حطها في نص البوردة

    // تظبيط مكان الـ Knob (الزرار اللي بيتحرك)
    sliderHandle.setOrigin(sliderTexture.getSize().x / 2.f, sliderTexture.getSize().y / 2.f);
    // مكانه بيعتمد على الـ currentVolume
    float startX = volumeBar.getPosition().x - (volbar_tex.getSize().x / 2.f) + (currentVolume / 100.f * volbar_tex.getSize().x);
    sliderHandle.setPosition(startX, volumeBar.getPosition().y);
}

