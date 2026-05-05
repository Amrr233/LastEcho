#include "healthbar.h"
#include "Data.h"
#include <string>
#include "player.h"

extern Player player;

static sf::Texture healthbarTexture;
static sf::Sprite  healthbarSprite;
static bool hpLoaded = false;

//hankelha player.cpp ba3den
void healing(int heal) {
    player.hp += heal;
    if (player.hp > player.maxHp) {
        player.hp = player.maxHp;
    }
}
void damaging(int damage) {
    player.hp -= damage;
    if (player.hp < 0)
        player.hp = 0;
}

void drawHealthBar(sf::RenderWindow& window) {
    if (!hpLoaded) {
        if (healthbarTexture.loadFromFile("assets/settings/HP.png")) {
            healthbarSprite.setTexture(healthbarTexture);
            healthbarSprite.setOrigin(0.f, 0.f);
            healthbarSprite.setScale(0.3f, 0.4f);
            hpLoaded = true;
        }
    }

    float startX = 10.0f;
    float startY = 10.0f;
    healthbarSprite.setPosition(startX, startY);

    sf::FloatRect frameBounds = healthbarSprite.getGlobalBounds();

    float ratio = (float)player.hp / player.maxHp;
    if (ratio > 1.0f) ratio = 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;


    float innerLeftOffset = frameBounds.width * 0.23f;
    float innerTopOffset  = frameBounds.height * 0.25f;
    float innerTotalWidth = frameBounds.width * 0.72f;
    float barHeight       = frameBounds.height * 0.50f;

    int totalSegments = 10;
    float gap = 2.0f;
    float segmentWidth = (innerTotalWidth - (gap * (totalSegments - 1))) / totalSegments;

    window.draw(healthbarSprite);

    for (int i = 0; i < totalSegments; i++) {
        float segmentStart = (float)i / totalSegments;
        float segmentEnd = (float)(i + 1) / totalSegments;

        if (ratio <= segmentStart) continue;

        float currentDrawWidth = segmentWidth;
        if (ratio < segmentEnd) {
            currentDrawWidth = segmentWidth * ((ratio - segmentStart) * totalSegments);
        }

        sf::RectangleShape bar(sf::Vector2f(currentDrawWidth, barHeight));
        bar.setFillColor(sf::Color(165, 42, 42));
        bar.setOrigin(0.f, 0.f);

        float xPos = frameBounds.left + innerLeftOffset + (i * (segmentWidth + gap));
        float yPos = frameBounds.top + innerTopOffset;

        bar.setPosition(xPos, yPos);
        window.draw(bar);
    }
}