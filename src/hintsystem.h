#ifndef HINT_SYSTEM_H
#define HINT_SYSTEM_H

#include <SFML/Graphics.hpp>
#include <string>

struct HintSystem {
    // الأيكون الصغير
    sf::Texture iconTex;
    sf::Sprite  iconSprite;

    // الورقة الكاملة
    sf::Texture pageTex;
    sf::Sprite  pageSprite;

    sf::Font    font;

    // الهنتات
    std::string hints[4] = {
        "I hold what falls but never keep it,\nlook beneath where water sleeps.",
        "Thousands sat upon my back,\nbut no one checked beneath my legs.",
        "I'm here to stop what burns,\nbut something golden hides behind me.",
        "I show you music frozen in time,\nwhat you seek rests behind my frame."
    };

    int  hintsUnlocked  = 0;   // كام هنت اتفتح
    bool isOpen         = false;
    bool hasNewHint     = false; // النقطة الحمرا
    bool allDone        = false; // لما يلم الكل
    bool initialized    = false;
};

void initHintSystem(HintSystem& hs);
void unlockHint(HintSystem& hs, int stringIndex); // 0-3
void markAllStringsCollected(HintSystem& hs);
void updateHintSystem(HintSystem& hs);
void drawHintIcon(sf::RenderWindow& window, HintSystem& hs);
void drawHintPage(sf::RenderWindow& window, HintSystem& hs);

#endif