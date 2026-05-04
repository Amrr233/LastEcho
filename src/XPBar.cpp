#include "XPBar.h"
#include "Data.h"
#include "player.h"

extern Player player;

static sf::Texture xpTexture;
static sf::Sprite xpSprite;
static bool loaded = false;
static sf::Font xpFont;
static bool fontLoaded = false;

void addXP(int amount) {
    player.xp += amount;
    while (player.xp >= player.xpToNextLevel) {
        player.xp -= player.xpToNextLevel;
        player.level++;
    }
}

void drawXPBar(sf::RenderWindow& window) {
    if (!loaded) {
        if (xpTexture.loadFromFile("assets/settings/XP.png")) {
            xpSprite.setTexture(xpTexture);
            xpSprite.setOrigin(0.f, 0.f);
            xpSprite.setScale(0.3f, 0.4f);
            loaded = true;
        }
        if (!fontLoaded && xpFont.loadFromFile("assets/fonts/pixelsix00.ttf")) {
            fontLoaded = true;
        }
    }

    float startX = 10.0f;
    float startY = 70.0f;
    xpSprite.setPosition(startX, startY);

    sf::FloatRect frameBounds = xpSprite.getGlobalBounds();


    float innerLeftOffset = frameBounds.width * 0.165f;
    float innerTopOffset  = frameBounds.height * 0.25f;
    float innerTotalWidth = frameBounds.width * 0.72f;
    float barHeight       = frameBounds.height * 0.50f;

    float ratio = (float)player.xp / player.xpToNextLevel;
    if (ratio > 1.0f) ratio = 1.0f;

    int totalSegments = 10;
    float gap = 2.0f;
    float segmentWidth = (innerTotalWidth - (gap * (totalSegments - 1))) / totalSegments;

    window.draw(xpSprite);

    for (int i = 0; i < totalSegments; i++) {
        float segmentStart = (float)i / totalSegments;
        float segmentEnd = (float)(i + 1) / totalSegments;
        if (ratio <= segmentStart) continue;

        float currentDrawWidth = segmentWidth;
        if (ratio < segmentEnd) {
            currentDrawWidth = segmentWidth * ((ratio - segmentStart) * totalSegments);
        }

        sf::RectangleShape bar(sf::Vector2f(currentDrawWidth, barHeight));
        bar.setFillColor(sf::Color(212, 181, 125));
        bar.setOrigin(0.f, 0.f);

        float xPos = frameBounds.left + innerLeftOffset + (i * (segmentWidth + gap));
        float yPos = frameBounds.top + innerTopOffset;

        bar.setPosition(xPos, yPos);
        window.draw(bar);
    }

    if (fontLoaded) {
        sf::Text lvlText;
        lvlText.setFont(xpFont);
        lvlText.setString(std::to_string(player.level));
        lvlText.setCharacterSize(player.level < 10 ? 30 : 22);
        lvlText.setFillColor(sf::Color::White);
        lvlText.setOutlineColor(sf::Color::Black);
        lvlText.setOutlineThickness(1.5f);


        sf::FloatRect textBounds = lvlText.getLocalBounds();
        lvlText.setOrigin(textBounds.left + textBounds.width / 2.0f,
                          textBounds.top + textBounds.height / 2.0f);


        float leftBoxCenter = innerLeftOffset / 2.0f;

        lvlText.setPosition(frameBounds.left + leftBoxCenter,
                            frameBounds.top + (frameBounds.height / 2.0f));

        window.draw(lvlText);
    }
}