// void healing(int heal) {
//     player.hp += heal;
//     if (player.hp > player.maxHp) {
//         player.hp = player.maxHp;
//     }
// }
// void damaging(int damage) {
//     player.hp -= damage;
//     if (player.hp < 0)
//         player.hp = 0;
// }
// void initHealthBar() {
//     // 1. إعداد المستطيل الخلفي (الأحمر)
//     healthbarBACK.setSize(sf::Vector2f(200.f, 20.f));
//     healthbarBACK.setFillColor(sf::Color(100, 0, 0));
//
//     // 2. إعداد المستطيل الأمامي (الأخضر)
//     healthbarFRONT.setFillColor(sf::Color::Green);
//
//     // 3. تحميل الخط وإعداد النص
//     if (hpdeclaration.loadFromFile("assets/fonts/pixelsix00.ttf")) {
//         hptexture.setFont(hpdeclaration);
//         hptexture.setCharacterSize(18);
//         hptexture.setFillColor(sf::Color::White);
//     }
// }
//
// void drawHealthBar(sf::RenderWindow& window) {
//     // 1. حساب النسبة المئوية
//     float hpratio = (float)player.hp / (float)player.maxHp;
//     if (hpratio < 0) hpratio = 0;
//
//     // 2. تحديث حجم الشريط الأخضر
//     healthbarFRONT.setSize(sf::Vector2f(200.f * hpratio, 20.f));
//
//     // 3. تحديث النص (90 / 100)
//     std::string hp_string = std::to_string(player.hp) + " / " + std::to_string(player.maxHp);
//     hptexture.setString(hp_string);
//
//     // 4. التموضع (ثابت في ركن الشاشة)
//     sf::Vector2f uiPos(20.f, 20.f);
//     healthbarBACK.setPosition(uiPos);
//     healthbarFRONT.setPosition(uiPos);
//     hptexture.setPosition(uiPos.x + 65.f, uiPos.y - 2.f);
//
//     // 5. الرسم بالترتيب
//     window.draw(healthbarBACK);
//     window.draw(healthbarFRONT);
//     window.draw(hptexture);
// }

#include "healthbar.h"
#include "Data.h"
#include <string>

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
            hpSprite.setOrigin(0.0f, 0.0f);
            hpSprite.setScale(0.296f, 0.6f);
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
    float barHeight = 35.0f * 1.7f;
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

        float xOffset = (177.0f * 0.5f) + (i * (segmentWidth + gap));
        float yOffset = (-60.f * 1.05f);

        bar.setPosition(startX + xOffset, startY + yOffset);
        window.draw(bar);
    }
    window.draw(hpSprite);
}