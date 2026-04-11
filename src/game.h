#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Data.h" // تأكد أن الـ Enum موجود هنا

struct Game {
    bool isPaused; // حالة الإيقاف المؤقت
    Game();
    // دالة التحميل الأساسية
    void init(float W, float H);
    // دالة التحديث (المنطق)
    void update(sf::RenderWindow& window, AppState& currentState);
    // دالة الرسم
    void draw(sf::RenderWindow& window);

    sf::Sprite background;
    sf::Texture bgTex;

    sf::Sprite pauseBtn;
    sf::Texture pauseTex;

    sf::Sprite pauseMenuBG;
    sf::Texture pauseMenuTex;

    sf::Texture pauseMenubuttons;
    sf::Sprite buttons[3];
    sf::Text words[3];
    sf::Font word_font;
};

#endif