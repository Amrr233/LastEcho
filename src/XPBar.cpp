#include "XPBar.h"
#include "Data.h"
#include "player.h"

extern Player player;

// بنستخدم static عشان نحمل الصورة مرة واحدة بس مش كل فريم
static sf::Texture xpTexture;
static sf::Sprite xpSprite;
static bool loaded = false;// ضيف دول فوق مع الـ static variables لو مش موجودين
static sf::Font xpFont;
static bool fontLoaded = false;

void drawXPBar(sf::RenderWindow& window) {
    if (!loaded) {
        if (xpTexture.loadFromFile("assets/settings/XP.png")) {
            xpSprite.setTexture(xpTexture);

            // --- تعديل الـ Origin عشان يبقى التوب ليفت (0,0) بدل السنتر ---
            // ده بيسهل رص العناصر جواه بالنسبة للتوب ليفت برضه
            xpSprite.setOrigin(-14.0f, -300.0f);

            xpSprite.setScale(0.3f, 0.4f);
            loaded = true;
        }
        if (!fontLoaded) {
            if (xpFont.loadFromFile("assets/fonts/pixelsix00.ttf")) {
                fontLoaded = true;
            }
        }
    }

    // --- 1. تحديد مكان البار الجديد (فوق عالشمال) ---
    // بنبعد مسافة بسيطة (10 بيكسل) عن الحافة عشان يبقى شكله شيك
    float startX = 10.0f;
    float startY = 10.0f;
    xpSprite.setPosition(startX, startY);

    // --- 2. رسم الـ 10 أجزاء (نفس كودك بالظبط) ---
    float ratio = (float)player.xp / player.xpToNextLevel;
    if (ratio > 1.0f) ratio = 1.0f;

    int totalSegments = 10;
    float totalWidth = 285.0f * 0.63f;
    float barHeight = 35.0f * 1.f;
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
        bar.setFillColor(sf::Color(212, 181, 125));

        // --- تعديل الـ Origin لـ 0.0 عشان يناسب الـ Sprite Origin الجديد ---
        // الـ 60 اللي كنت كاتبها كانت بتشفته بناء على إن الـ Sprite Origin في السنتر
        bar.setOrigin(38.f,  -30.f);

        // --- تعديل حسابات مكان الـ Fill بالنسبة للتوب ليفت ---
        // الـ 110 ده الـ Offset الأفقي عشان يبدأ بعد المربع الصغير
        // الـ (27*1.05) ده عشان يتوسط الخشب طولياً
        float xOffset = (185.0f * 0.5f) + (i * (segmentWidth + gap));
        float yOffset = (95.f * 1.05f);

        bar.setPosition(startX + xOffset, startY + yOffset);
        window.draw(bar);
    }

    // --- 3. رسم الفريم الخشبي ---
    window.draw(xpSprite);

    // --- 4. إضافة نص الـ Level جوه المربع ---
    if (fontLoaded) {
        sf::Text lvlText;
        lvlText.setFont(xpFont);
        lvlText.setString(std::to_string(player.level));
        lvlText.setCharacterSize(34);
        lvlText.setFillColor(sf::Color::White);
        lvlText.setOutlineColor(sf::Color::Black);
        lvlText.setOutlineThickness(1.5f);

        sf::FloatRect textRect = lvlText.getLocalBounds();
        lvlText.setOrigin(textRect.left + textRect.width / 2.0f,
                          textRect.top + textRect.height / 2.0f);

        // --- تعديل مكان النص بالنسبة للتوب ليفت ---
        // الـ 55 والـ 30 دول عشان يتوسط المربع الخشبي الصغير بالظبط
        float lvlXOffset = (48.0f * 0.5f);
        float lvlYOffset = (293.0f * 0.5f);
        lvlText.setPosition(startX + lvlXOffset, startY + lvlYOffset);
        window.draw(lvlText);
    }
}