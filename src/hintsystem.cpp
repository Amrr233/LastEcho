#include "HintSystem.h"
#include "Data.h"
#include <iostream>

void initHintSystem(HintSystem& hs) {
    // Load icon (small, shown in HUD)
    if (!hs.iconTex.loadFromFile("assets/sprites/items/hints.png")) {
        std::cerr << "[HintSystem] Failed to load hints.png" << std::endl;
    }
    hs.iconSprite.setTexture(hs.iconTex);
    float iconScale = 50.f / hs.iconTex.getSize().x;
    hs.iconSprite.setScale(iconScale, iconScale);
    hs.iconSprite.setPosition(40.f, 200.f);

    // Load page (large, shown when Q is pressed)
    if (!hs.pageTex.loadFromFile("assets/sprites/items/hints.png")) {
        std::cerr << "[HintSystem] Failed to load page texture" << std::endl;
    }
    hs.pageSprite.setTexture(hs.pageTex);
    float pageScaleX = 500.f / hs.pageTex.getSize().x;
    float pageScaleY = 500.f / hs.pageTex.getSize().y;
    hs.pageSprite.setScale(pageScaleX, pageScaleY);
    hs.pageSprite.setPosition(
        SCREEN_W / 2.f - 250.f,
        SCREEN_H / 2.f - 250.f
    );

    if (!hs.font.loadFromFile("assets/fonts/pixelsix00.ttf")) {
        std::cerr << "[HintSystem] Failed to load font" << std::endl;
    }

    hs.hintsUnlocked = 0;
    hs.isOpen        = false;
    hs.hasNewHint    = false;
    hs.allDone       = false;
    hs.initialized   = true;
}

void unlockHint(HintSystem& hs, int stringIndex) {
    if (stringIndex < 0 || stringIndex > 3) return;
    if (hs.hintsUnlocked == stringIndex) {
        hs.hintsUnlocked++;
        hs.hasNewHint = true;
    }
}

void markAllStringsCollected(HintSystem& hs) {
    if (hs.hintsUnlocked >= 4) {
        hs.allDone    = true;
        hs.hasNewHint = true;
    }
}
void drawHintIcon(sf::RenderWindow& window, HintSystem& hs) {
    if (!hs.initialized) return;
    if (hs.hintsUnlocked == 0 && !hs.allDone) return;

    window.draw(hs.iconSprite);

    if (hs.hasNewHint) {
        sf::CircleShape dot(5.f);
        dot.setFillColor(sf::Color::Red);
        dot.setOutlineColor(sf::Color::Black);
        dot.setOutlineThickness(1.f);
        float ix = hs.iconSprite.getPosition().x + hs.iconSprite.getGlobalBounds().width - 3.f;
        float iy = hs.iconSprite.getPosition().y - 3.f;
        dot.setPosition(ix, iy);
        window.draw(dot);
    }
}

void drawHintPage(sf::RenderWindow& window, HintSystem& hs) {
    if (!hs.isOpen) return;

    // dim background
    sf::RectangleShape dim(sf::Vector2f((float)SCREEN_W, (float)SCREEN_H));
    dim.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(dim);

    // draw the note page
    window.draw(hs.pageSprite);

    sf::FloatRect pageBounds = hs.pageSprite.getGlobalBounds();

    // text starts below the pin (top ~80px padding) and left margin
    float startX = pageBounds.left + 80.f;
    float startY = pageBounds.top  + 200.f;

    if (hs.allDone) {
        sf::Text gjText;
        gjText.setFont(hs.font);
        gjText.setString("GJ! You found all the strings!\nGo back to Amr!");
        gjText.setCharacterSize(18);
        gjText.setFillColor(sf::Color(60, 30, 10));
        gjText.setOutlineColor(sf::Color::Black);
        gjText.setOutlineThickness(1);
        gjText.setPosition(
            pageBounds.left + pageBounds.width  / 2.f - gjText.getGlobalBounds().width  / 2.f,
            pageBounds.top  + pageBounds.height / 2.f - gjText.getGlobalBounds().height / 2.f
        );
        window.draw(gjText);

        sf::Text closeText;
        closeText.setFont(hs.font);
        closeText.setString("Press Q to close");
        closeText.setCharacterSize(12);
        closeText.setFillColor(sf::Color(150, 100, 50));
        closeText.setPosition(
            pageBounds.left + pageBounds.width / 2.f - closeText.getGlobalBounds().width / 2.f,
            pageBounds.top  + pageBounds.height - 40.f
        );
        window.draw(closeText);
        return;
    }

    // draw current active hint
    if (hs.hintsUnlocked > 0 && hs.hintsUnlocked <= 4) {
        int i = hs.hintsUnlocked - 1;

        // "Hint X of 4" label
        sf::Text labelText;
        labelText.setFont(hs.font);
        labelText.setString("Hint " + std::to_string(i + 1) + " of 4");
        labelText.setCharacterSize(13);
        labelText.setFillColor(sf::Color(120, 70, 20));
        labelText.setPosition(startX, startY);
        window.draw(labelText);

        // hint body
        sf::Text hintText;
        hintText.setFont(hs.font);
        hintText.setString(hs.hints[i]);
        hintText.setCharacterSize(14);
        hintText.setFillColor(sf::Color(60, 30, 10));
        hintText.setPosition(startX, startY + 30.f);
        window.draw(hintText);
    }

    sf::Text closeText;
    closeText.setFont(hs.font);
    closeText.setString("Press Q to close");
    closeText.setCharacterSize(12);
    closeText.setFillColor(sf::Color(150, 100, 50));
    closeText.setPosition(
        pageBounds.left + pageBounds.width / 2.f - closeText.getGlobalBounds().width / 2.f,
        pageBounds.top  + pageBounds.height - 40.f
    );
    window.draw(closeText);
}