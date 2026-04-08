#include "inventory.h"

void inventory::invt_init(float W, float H) {
    // 1. تحميل صورة الشريط (الـ 6 خانات)
    invTex.loadFromFile("assets/gameplay/inventory_bar.png");
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
void inventory::invt_update(sf::RenderWindow& window, AppState& currentState) {
    // التحكم في الخانات (من 1 لـ 6)
    for (int i = 0; i < 6; i++) {
        if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(sf::Keyboard::Num1 + i))) {
            selectedSlot = i;
        }
    }

    // تحديث مكان المربع الأصفر بناءً على الخانة المختارة
    float startX = invBar.getPosition().x - (invTex.getSize().x / 2.0f) + 5.0f;
    selector.setPosition(startX +270.f + (selectedSlot * 70.5f), invBar.getPosition().y - 95.0f);
}
void inventory::invt_draw(sf::RenderWindow& window) {
    // ... رسم البوز واللوحة ...

    // رسم الانفنتوري (بتترسم دايماً)
    window.draw(invBar);
    window.draw(selector);

}
