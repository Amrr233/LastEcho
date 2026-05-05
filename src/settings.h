#ifndef LASTECHO_SETTINGS_H
#define LASTECHO_SETTINGS_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Data.h" // ضروري عشان الـ AppState والـ States

struct settingsMenu {
 sf::Texture labelTex[3];
 sf::Sprite labelSprites[3];
 sf::Sprite settings_window;
 sf::Texture settings_window_tex ;
 sf::Sprite backSprite;
 sf::Texture backTex;

 sf::Texture volbar_tex;
 sf::Texture sliderTexture;



 sf::Sprite masterBar;
 sf::Sprite masterHandle;
 float masterVolume = 50.0f;

 sf::Sprite musicBar;
 sf::Sprite musicHandle;
 float musicVolume = 50.0f;

 sf::Sprite sfxBar;
 sf::Sprite sfxHandle;
 float sfxVolume = 50.0f;




 void init(float W, float H);
 void updateMaster(float delta);
 void updateMusic(float delta);
 void updateSFX(float delta);


 void draw(sf::RenderWindow &window);
};

extern settingsMenu settings;


void SettingsUpdate(sf::RenderWindow& window, AppState& currentState);

#endif