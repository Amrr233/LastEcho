#include "inventory.h"
#include <iostream>
#include <cmath>



void inventory::invt_init(float W, float H) {
    // 1. تحميل صورة الشريط (الـ 6 خانات)
    invTex.loadFromFile("assets/gameplay/inventory_bar.png");
    font.loadFromFile("assets/fonts/pixelsix00.ttf");
    countText.setFont(font);
    countText.setCharacterSize(14); // رقم صغير عشان الركن
    countText.setFillColor(sf::Color::White);
    countText.setOutlineColor(sf::Color::Black);
    countText.setOutlineThickness(1);
    invBar.setTexture(invTex);

    // 2. تظبيط مكان الشريط (تحت في النص)
    invBar.setOrigin(invTex.getSize().x / 2.0f, invTex.getSize().y);
    invBar.setPosition(SCREEN_W/2,  SCREEN_H - 10.f); // بعيد عن الحافة التحتانية بـ 10 بكسل
    invBar.setScale(0.5f, 0.5f);

    // 3. تظبيط شكل المربع اللي بيحدد الخانة
    selector.setSize(sf::Vector2f(50.0f, 50.0f)); // مقاس المربع (عدله حسب مقاس خانتك)
    selector.setFillColor(sf::Color::Transparent);
    selector.setOutlineColor(sf::Color(212, 181, 125));
    selector.setOutlineThickness(3);
}


void inventory::invt_update(sf::RenderWindow& window, AppState& currentState, sf::Vector2f playerPos, float dt) {
    // 1. التحكم في السلوتس (زي ما هو)
    for (int i = 0; i < 6; i++) {
        if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(sf::Keyboard::Num1 + i))) {
            selectedSlot = i;
        }
    }
    // تحديث مكان السليكتور (زي ما هو)
    float startX = invBar.getPosition().x - (invTex.getSize().x / 2.0f) + 5.0f;
    selector.setPosition(startX + 270.f + (selectedSlot * 70.5f), invBar.getPosition().y - 95.0f);

    // 2. تحديث إيفكت الأيتم (التعديل هنا)
    if (feedbackTimer > 0) {

        // --- إعدادات التحكم اليدوي (عدل هنا) ---
        float travelDistance = 15.0f; // المسافة الكلية اللي هيطلعها (مثلاً 30 بكسل)
        float startHeight = 70.0f;    // الارتفاع البدائي فوق راس اللاعب
        float totalTime = 2.0f;       // الوقت الكلي للأنيماشين (نفس قيمة الـ feedbackTimer)
        // ------------------------------------

        // حساب النسبة المئوية للوقت اللي فات (من 0 لـ 1)
        float progress = (totalTime - feedbackTimer) / totalTime;

        // 1. حساب الارتفاع: الارتفاع البدائي + (النسبة * المسافة الكلية)
        float floatHeight = startHeight + (progress * travelDistance);

        feedbackSprite.setPosition(playerPos.x + 24.f, playerPos.y - floatHeight);

        // 2. تظبيط اللمعان والشفافية (Fade Out)
        sf::Uint8 alpha = static_cast<sf::Uint8>(feedbackTimer / totalTime * 255);
        feedbackSprite.setColor(sf::Color(255, 255, 255, alpha));

        for (int i = 0; i < 5; i++) {
            sparkles[i].setPosition(
                feedbackSprite.getPosition().x + sin(feedbackTimer * 10 + i) * 15.f,
                feedbackSprite.getPosition().y + cos(feedbackTimer * 10 + i) * 15.f
            );
            sparkles[i].setFillColor(sf::Color(255, 255, 200, alpha));
        }

        feedbackTimer -= dt;
    }
}

void inventory::invt_draw(sf::RenderWindow& window) {
    window.draw(invBar);
    window.draw(selector);

    float offsetX = 60.f;
    float offsetY = 70.f;
    float gap     = 70.5f;

    for (int i = 0; i < 6; i++) {
        if (!hasItem[i]) continue;

        // center origin
        itemSprites[i].setOrigin(
            itemTextures[i].getSize().x / 2.0f,
            itemTextures[i].getSize().y / 2.0f
        );

        float startX = invBar.getPosition().x - (invBar.getGlobalBounds().width / 2.0f);

        itemSprites[i].setPosition(
            startX + offsetX + (i * gap),
            invBar.getPosition().y - offsetY
        );

        window.draw(itemSprites[i]);

        // draw counter if more than 1
        if (itemQuantity[i] > 1) {
            countText.setString(std::to_string(itemQuantity[i]));

            sf::Vector2f itemPos = itemSprites[i].getPosition();

            // bottom-right corner of slot like minecraft
            countText.setPosition(
                itemPos.x + 10.f,
                itemPos.y + 12.f
            );

            window.draw(countText);
        }
    }
}


bool inventory::addItem(std::string name, std::string texturePath) {
    // 1. البحث: هل الأيتم ده موجود عندنا فعلاً؟
    for (int i = 0; i < 6; i++) {
        if (hasItem[i] && itemNames[i] == name) { // محتاج تضيف مصفوفة itemNames في الـ h
            itemQuantity[i]++; // زود العدد
            std::cout << "Stacked " << name << " to slot " << i << ". Total: " << itemQuantity[i] << std::endl;
            return false;
        }
    }

    // 2. لو مش موجود: ابحث عن أول خانة فاضية كالعادة
    for (int i = 0; i < 6; i++) {
        if (!hasItem[i]) {
            if (itemTextures[i].loadFromFile(texturePath)) {
                itemSprites[i].setTexture(itemTextures[i]);
                // Auto-fit item to slot size (max 40x40)
                float maxSize = 40.f;
                float texW = (float)itemTextures[i].getSize().x;
                float texH = (float)itemTextures[i].getSize().y;
                float scale = std::min(maxSize / texW, maxSize / texH);
                itemSprites[i].setScale(scale, scale);
                itemNames[i] = name; // خزن الاسم عشان نعرفه المرة الجاية
                itemQuantity[i] = 1; // ابدأ بـ 1
                hasItem[i] = true;
                return true;
            }
        }
        // strings are not usable, key is usable
        if (name == "guitar_string") isUsable[i] = false;
        else isUsable[i] = true;
    }
    return false;
}

void inventory::triggerPickupEffect(std::string texturePath) {

    if (feedbackTex.loadFromFile(texturePath)) {
        feedbackSprite.setTexture(feedbackTex);
        feedbackSprite.setOrigin(feedbackTex.getSize().x / 2.0f, feedbackTex.getSize().y / 2.0f);
        feedbackTimer = 2.0f; // الإيفكت يفضل شغال ثانيتين

        // تجهيز اللمعان (Sparkles)
        for (int i = 0; i < 5; i++) {
            sparkles[i].setRadius(2.f + i);
            sparkles[i].setFillColor(sf::Color(255, 255, 200));
        }
    }
}
