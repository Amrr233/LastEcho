#include "healthbar.h"
#include "Data.h"
#include <string>
#include "player.h"

extern Player player;

static sf::Texture hpTexture;
static sf::Sprite  hpSprite;
static bool hpLoaded = false;

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
        if (hpTexture.loadFromFile("assets/settings/HP.png")) {
            hpSprite.setTexture(hpTexture);
            // ضبط الـ Origin والـ Scale زي كود الـ XP اللي بعته
            hpSprite.setOrigin(-14.0f, -150.0f);
            hpSprite.setScale(0.3f, 0.4f);
            hpLoaded = true;
        }
    }

    // المكان (فوق عالشمال - تحت الـ XP)
    float startX = 10.0f;
    float startY = 10.0f; // زودنا الـ Y عشان ميبقوش فوق بعض
    hpSprite.setPosition(startX, startY);

    // 1. حساب نسبة الـ HP
    float ratio = (float)player.hp / player.maxHp;
    if (ratio > 1.0f) ratio = 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;

    // 2. نفس متغيرات التقسيم اللي طلبتها بالظبط
    int totalSegments = 10;
    float totalWidth = 285.0f * 0.63f;
    float barHeight = 35.0f * 1.3f;
    float gap = 2.0f;
    float segmentWidth = (totalWidth - (gap * (totalSegments - 1))) / totalSegments;

    for (int i = 0; i < totalSegments; i++) {
        float segmentStart = (float)i / totalSegments;
        float segmentEnd = (float)(i + 1) / totalSegments;

        if (ratio <= segmentStart) continue;

        float currentDrawWidth = segmentWidth;
        if (ratio < segmentEnd) {
            currentDrawWidth = segmentWidth * ((ratio - segmentStart) * totalSegments);
        }

        sf::RectangleShape bar(sf::Vector2f(currentDrawWidth, barHeight));

        // لون أحمر محمر (Red Brown) للـ HP
        bar.setFillColor(sf::Color(165, 42, 42));

        // نفس الـ Origin والـ Offset اللي في كود الـ XP بتاعك
        bar.setOrigin(38.f, -73.f);

        float xOffset = (185.0f * 0.5f) + (i * (segmentWidth + gap));
        float yOffset = (-5.f * 1.05f);

        bar.setPosition(startX + xOffset, startY + yOffset);
        window.draw(bar);
    }
    window.draw(hpSprite);
}