#include "settings.h"
#include "Data.h"
#include <SFML/Audio.hpp>
#include <algorithm>
#include "audio.h"

extern AppState last_state;
settingsMenu settings;

void settingsMenu::init(float W, float H) {
    // 1. تحميل الصور (تأكد إن الأسامي دي بالظبط في فولدر assets)
    backTex.loadFromFile("assets/mainMenu/mainbackground.png");
    bgTex.loadFromFile("assets/settings/settings bg.png");
    volbar_tex.loadFromFile("assets/settings/volumebar.png");
    sliderTexture.loadFromFile("assets/settings/slider.png");

    // 2. إعداد اللوحة الخشبية (Background)
    backSprite.setTexture(backTex);
    settings_window.setTexture(bgTex);
    settings_window.setOrigin(bgTex.getSize().x / 2.f, bgTex.getSize().y / 2.f);
    settings_window.setPosition(W / 2.f, H / 2.f);
    backSprite.setOrigin(backTex.getSize().x / 2.f, backTex.getSize().y / 2.f);
    backSprite.setPosition(W / 2.f, H / 2.f);
    backSprite.setScale(1.35f, 1.35f);

    // 3. إعداد Master Slider
    masterBar.setTexture(volbar_tex);
    masterBar.setOrigin(volbar_tex.getSize().x / 2.f, volbar_tex.getSize().y / 2.f);
    masterBar.setPosition(W / 2.f - 150.0f, H / 2.f - 180.f); // رفعتها فوق شوية عن المركز

    masterHandle.setTexture(sliderTexture);
    masterHandle.setOrigin(sliderTexture.getSize().x / 2.f, sliderTexture.getSize().y / 2.f);

    // 4. إعداد Music Slider
    musicBar.setTexture(volbar_tex);
    musicBar.setOrigin(volbar_tex.getSize().x / 2.f, volbar_tex.getSize().y / 2.f);
    musicBar.setPosition(W / 2.f - 150.0f, H / 2.f);

    musicHandle.setTexture(sliderTexture);
    musicHandle.setOrigin(sliderTexture.getSize().x / 2.f, sliderTexture.getSize().y / 2.f);

    // 5. إعداد SFX Slider
    sfxBar.setTexture(volbar_tex);
    sfxBar.setOrigin(volbar_tex.getSize().x / 2.f, volbar_tex.getSize().y / 2.f);
    sfxBar.setPosition(W / 2.f - 150.0f, H / 2.f + 180.f); // نزلتها تحت شوية

    sfxHandle.setTexture(sliderTexture);
    sfxHandle.setOrigin(sliderTexture.getSize().x / 2.f, sliderTexture.getSize().y / 2.f);

    // تحديث الأماكن بناءً على القيم الافتراضية (50%)
    updateMaster(0);
    updateMusic(0);
    updateSFX(0);
    // 1. تصغير البوردة الخشب (الخلفية)
    settings_window.setScale(2.0f, 2.0f);

    // 2. تصغير البارات التلاتة (الخشب الصغير)
    masterBar.setScale(0.4f, 0.4f);
    musicBar.setScale(0.4f, 0.4f);
    sfxBar.setScale(0.4f, 0.4f);

    // 3. تصغير المقابض (الـ Handles)
    masterHandle.setScale(0.3f, 0.3f);
    musicHandle.setScale(0.3f, 0.3f);
    sfxHandle.setScale(0.3f, 0.3f);
    std::string labelFiles[] = {"assets/settings/Master.png", "assets/settings/Music.png", "assets/settings/SFX.png"};
    sf::Sprite* bars[] = {&masterBar, &musicBar, &sfxBar};

    for (int i = 0; i < 3; i++) {
        labelTex[i].loadFromFile(labelFiles[i]);
        labelSprites[i].setTexture(labelTex[i]);

        // 1. تظبط الـ Scale (لو الصور كبيرة، 0.4f او 0.5f هيكون مثالي)
        labelSprites[i].setScale(0.4f, 0.4f);

        // 2. تظبط الـ Origin في نص الصورة عشان تتحاذى صح مع البار
        labelSprites[i].setOrigin(labelTex[i].getSize().x / 2.f, labelTex[i].getSize().y / 2.f);

        // 3. تظبط المكان (جنب الـ Slider بتاعه بالظبط)
        // بنطرح من الـ x عشان تيجي على الشمال، والـ y بياخد نفس ارتفاع البار
        labelSprites[i].setPosition(bars[i]->getPosition().x + 370.f, bars[i]->getPosition().y);
    }
}

void settingsMenu::updateMaster(float delta) {
    masterVolume = std::max(0.f, std::min(100.f, masterVolume + delta));
    if (masterVolume < 1.0f) {
        masterVolume = 0.0f;
    }
    sf::Listener::setGlobalVolume(masterVolume);

    // بنجيب حدود الخشبة الفعلية بعد الـ Scale
    sf::FloatRect bounds = masterBar.getGlobalBounds();

    // بنسيب هامش صغير (10 بكسل) عشان المقبض ميوصلش للطرف الحاد
    float usableWidth = bounds.width - 110.f;
    float startX = bounds.left + 55.f + (masterVolume / 100.f * usableWidth);

    masterHandle.setPosition(startX, masterBar.getPosition().y - 15.f);
}

void settingsMenu::updateMusic(float delta) {
    musicVolume = std::max(0.f, std::min(100.f, musicVolume + delta));
    if (musicVolume < 1.0f) {
        musicVolume = 0.0f;
    }
    audioManager.setVolume(musicVolume);

    sf::FloatRect bounds = musicBar.getGlobalBounds();
    float usableWidth = bounds.width - 110.f;
    float startX = bounds.left + 55.f + (musicVolume / 100.f * usableWidth);

    musicHandle.setPosition(startX, musicBar.getPosition().y - 15.f);
}

void settingsMenu::updateSFX(float delta) {
    sfxVolume = std::max(0.f, std::min(100.f, sfxVolume + delta));
    if (musicVolume < 1.0f) {
        musicVolume = 0.0f;
    }
    float barWidth = volbar_tex.getSize().x - 620.f;
    float startX = sfxBar.getPosition().x - (barWidth / 2.f) + (sfxVolume / 100.f * barWidth);
    sfxHandle.setPosition(startX, sfxBar.getPosition().y - 15.f);
}

// دالة الرسم (عشان الشاشة متبقاش سودة)
void settingsMenu::draw(sf::RenderWindow &window) {
    window.draw(backSprite);
    window.draw(settings_window);
    window.draw(masterBar);    window.draw(masterHandle);
    window.draw(musicBar);     window.draw(musicHandle);
    window.draw(sfxBar);       window.draw(sfxHandle);
    for (int i = 0; i < 3; i++) {
        window.draw(labelSprites[i]);
    }
}

// دالة التحديث والتحكم (الرجوع بـ Escape وحركة الماوس)
void SettingsUpdate(sf::RenderWindow& window, AppState& currentState) {
    // 1. الرجوع للمنيو
    static bool escWasPressed = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        if (!escWasPressed) {
            currentState = last_state;
            escWasPressed = true;
            sf::sleep(sf::milliseconds(200));
            // currentState = STATE_MENU;
        }
    }
    else {
        escWasPressed = false;
    }


    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // 2. سحب السلايدر بالماوس
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

        // مصفوفة عشان نلف على التلاتة ونوفر كود
        sf::Sprite* bars[] = {&settings.masterBar, &settings.musicBar, &settings.sfxBar};
        float* volumes[] = {&settings.masterVolume, &settings.musicVolume, &settings.sfxVolume};

        for (int i = 0; i < 3; i++) {
            if (bars[i]->getGlobalBounds().contains(mousePos)) {
                // بنجيب حدود الخشبة الفعلية اللي ظاهرة قدامنا (بعد الـ Scale)
                sf::FloatRect bounds = bars[i]->getGlobalBounds();

                // بنحسب الماوس فين بالنسبة لعرض الخشبة (0 لـ 1)
                float percentage = (mousePos.x - bounds.left) / bounds.width;

                // بنحولها لـ Volume (من 0 لـ 100)
                float newVol = percentage * 100.f;
                newVol = std::max(0.f, std::min(100.f, newVol)); // قفل الأطراف

                // تحديث الصوت بناءً على النوع
                if (i == 0) settings.updateMaster(newVol - settings.masterVolume);
                else if (i == 1) settings.updateMusic(newVol - settings.musicVolume);
                else if (i == 2) settings.updateSFX(newVol - settings.sfxVolume);
            }
        }
    }
}