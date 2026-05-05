#include "settings.h"
#include "Data.h"
#include <SFML/Audio.hpp>
#include <algorithm>
#include "audio.h"

extern AppState last_state;
settingsMenu settings;

void settingsMenu::init(float W, float H) {

    backTex.loadFromFile("assets/mainMenu/mainbackground.png");
    settings_window_tex.loadFromFile("assets/settings/settings bg.png");
    volbar_tex.loadFromFile("assets/settings/volumebar.png");
    sliderTexture.loadFromFile("assets/settings/slider.png");


    backSprite.setTexture(backTex);
    settings_window.setTexture(settings_window_tex);
    settings_window.setOrigin(settings_window_tex.getSize().x / 2.f, settings_window_tex.getSize().y / 2.f);
    settings_window.setPosition(W / 2.f, H / 2.f);
    backSprite.setOrigin(backTex.getSize().x / 2.f, backTex.getSize().y / 2.f);
    backSprite.setPosition(W / 2.f, H / 2.f);
    backSprite.setScale(1.35f, 1.35f);


    masterBar.setTexture(volbar_tex);
    masterBar.setOrigin(volbar_tex.getSize().x / 2.f, volbar_tex.getSize().y / 2.f);
    masterBar.setPosition(W / 2.f - 150.0f, H / 2.f - 180.f); // رفعتها فوق شوية عن المركز

    masterHandle.setTexture(sliderTexture);
    masterHandle.setOrigin(sliderTexture.getSize().x / 2.f, sliderTexture.getSize().y / 2.f);


    musicBar.setTexture(volbar_tex);
    musicBar.setOrigin(volbar_tex.getSize().x / 2.f, volbar_tex.getSize().y / 2.f);
    musicBar.setPosition(W / 2.f - 150.0f, H / 2.f);

    musicHandle.setTexture(sliderTexture);
    musicHandle.setOrigin(sliderTexture.getSize().x / 2.f, sliderTexture.getSize().y / 2.f);


    sfxBar.setTexture(volbar_tex);
    sfxBar.setOrigin(volbar_tex.getSize().x / 2.f, volbar_tex.getSize().y / 2.f);
    sfxBar.setPosition(W / 2.f - 150.0f, H / 2.f + 180.f); // نزلتها تحت شوية

    sfxHandle.setTexture(sliderTexture);
    sfxHandle.setOrigin(sliderTexture.getSize().x / 2.f, sliderTexture.getSize().y / 2.f);


    updateMaster(0);
    updateMusic(0);
    updateSFX(0);

    settings_window.setScale(2.0f, 2.0f);


    masterBar.setScale(0.4f, 0.4f);
    musicBar.setScale(0.4f, 0.4f);
    sfxBar.setScale(0.4f, 0.4f);


    masterHandle.setScale(0.3f, 0.3f);
    musicHandle.setScale(0.3f, 0.3f);
    sfxHandle.setScale(0.3f, 0.3f);
    std::string labelFiles[] = {"assets/settings/Master.png", "assets/settings/Music.png", "assets/settings/SFX.png"};
    sf::Sprite* bars[] = {&masterBar, &musicBar, &sfxBar};// هتساعدنا في تظبيط مكان الlableTex

    for (int i = 0; i < 3; i++) {
        labelTex[i].loadFromFile(labelFiles[i]);
        labelSprites[i].setTexture(labelTex[i]);


        labelSprites[i].setScale(0.4f, 0.4f);


        labelSprites[i].setOrigin(labelTex[i].getSize().x / 2.f, labelTex[i].getSize().y / 2.f);


        labelSprites[i].setPosition(bars[i]->getPosition().x + 370.f, bars[i]->getPosition().y);
    }
}

void settingsMenu::updateMaster(float delta) {
    masterVolume = std::max(0.f, std::min(100.f, masterVolume + delta));
    if (masterVolume < 1.0f) {
        masterVolume = 0.0f;
    }
    sf::Listener::setGlobalVolume(masterVolume);


    sf::FloatRect bounds = masterBar.getGlobalBounds();


    float usableWidth = bounds.width - 110.f;
    float startX = bounds.left + 55.f + (masterVolume / 100.f * usableWidth);

    masterHandle.setPosition(startX, masterBar.getPosition().y - 15.f);
}

void settingsMenu::updateMusic(float delta) {
    musicVolume = std::max(0.f, std::min(100.f, musicVolume + delta));
    if (musicVolume < 1.0f) {
        musicVolume = 0.0f;
    }
    audioManager.setMusicVolume(musicVolume);

    sf::FloatRect bounds = musicBar.getGlobalBounds();
    float usableWidth = bounds.width - 110.f;
    float startX = bounds.left + 55.f + (musicVolume / 100.f * usableWidth);

    musicHandle.setPosition(startX, musicBar.getPosition().y - 15.f);
}

void settingsMenu::updateSFX(float delta) {
    sfxVolume = std::max(0.f, std::min(100.f, sfxVolume + delta));
    if (sfxVolume < 1.0f) {
        sfxVolume = 0.0f;
    }
    audioManager.setSFXVolume(sfxVolume);
    sf::FloatRect bounds = sfxBar.getGlobalBounds();
    float usableWidth = bounds.width - 110.f;
    float startX = bounds.left + 55.f +(sfxVolume / 100.f * usableWidth);
    sfxHandle.setPosition(startX, sfxBar.getPosition().y - 15.f);
}


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


void SettingsUpdate(sf::RenderWindow& window, AppState& currentState) {
    static bool escWasPressed = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        audioManager.playClickSound();
        if (!escWasPressed) {
            currentState = last_state;
            escWasPressed = true;
            sf::sleep(sf::milliseconds(200));

        }
    }
    else {
        escWasPressed = false;
    }


    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));


    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {


        sf::Sprite* bars[] = {&settings.masterBar, &settings.musicBar, &settings.sfxBar};
        float* volumes[] = {&settings.masterVolume, &settings.musicVolume, &settings.sfxVolume};

        for (int i = 0; i < 3; i++) {
            if (bars[i]->getGlobalBounds().contains(mousePos)) {

                sf::FloatRect bounds = bars[i]->getGlobalBounds();


                float percentage = (mousePos.x - bounds.left) / bounds.width;


                float newVol = percentage * 100.f;
                newVol = std::max(0.f, std::min(100.f, newVol));


                if (i == 0) settings.updateMaster(newVol - settings.masterVolume);
                else if (i == 1) settings.updateMusic(newVol - settings.musicVolume);
                else if (i == 2) settings.updateSFX(newVol - settings.sfxVolume);
            }
        }
    }
}