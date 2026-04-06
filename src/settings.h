#ifndef LASTECHO_SETTINGS_H
#define LASTECHO_SETTINGS_H

#endif
#include <SFML/Graphics.hpp>

struct settingsMenu {
 sf::Sprite settings_window;
 sf::Texture bgTex;
 sf::Sprite volumeBar;
 sf::Texture volbar_tex;
 sf::Sprite sliderHandle;
 sf::Texture sliderTexture;
 sf::Sprite backButton;
 sf::Texture backBtnTex;

 float currentVolume = 50.0f;

 // تعريف الدوال فقط
 void volumeup();
 void volumedown();
 void init(float W, float H); // دالة لتحميل الصور وتظبيط الأبعاد
};

extern settingsMenu settings; // إعلان (Declaration)

