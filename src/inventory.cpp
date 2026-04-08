#include "inventory.h"
#include "inventory.h"
void inventory::invt_init(float W, float H) {
    // 1. تحميل صورة الشريط (الـ 6 خانات)
    invTex.loadFromFile("assets/gameplay/test.png");
    invBar.setTexture(invTex);

    // 2. تظبيط مكان الشريط (تحت في النص)
    invBar.setOrigin(invTex.getSize().x / 2.0f, invTex.getSize().y);
    invBar.setPosition(-100.f,  -100.0f); // بعيد عن الحافة التحتانية بـ 10 بكسل

    // 3. تظبيط شكل المربع اللي بيحدد الخانة
    selector.setSize(sf::Vector2f(50.0f, 50.0f)); // مقاس المربع (عدله حسب مقاس خانتك)
    selector.setFillColor(sf::Color::Transparent);
    selector.setOutlineColor(sf::Color::Yellow);
    selector.setOutlineThickness(3);
}
void inventory::invt_update(sf::RenderWindow& window, AppState& currentState) {
    // ... كود البوز القديم ...

    // التحكم في الخانات (من 1 لـ 6)
    for (int i = 0; i < 6; i++) {
        if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(sf::Keyboard::Num1 + i))) {
            selectedSlot = i;
        }
    }

    // تحديث مكان المربع الأصفر بناءً على الخانة المختارة
    float startX = invBar.getPosition().x - (invTex.getSize().x / 2.0f) + 5.0f;
    selector.setPosition(startX + (selectedSlot * 55.0f), invBar.getPosition().y - 55.0f);
}
void inventory::invt_draw(sf::RenderWindow& window) {
    // ... رسم البوز واللوحة ...

    // رسم الانفنتوري (بتترسم دايماً)
    window.draw(invBar);
    window.draw(selector);
}
