#include "healthbar.h"
#include "Data.h" // عشان نوصل لـ player.hp و player.maxHp
#include <string>

// ==============================
// EXTERN GLOBALS
// ==============================
extern Player player; // بنقول للملف إن فيه لاعب متعرف في مكان تاني

// ==============================
// LOCAL VARIABLES (Static)
// ==============================
static sf::RectangleShape healthbarBACK;
static sf::RectangleShape healthbarFRONT;
static sf::Font           hpdeclaration;
static sf::Text           hptexture;

// ==============================
// FUNCTIONS
// ==============================
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


void initHealthBar() {
    // 1. إعداد المستطيل الخلفي (الأحمر)
    healthbarBACK.setSize(sf::Vector2f(200.f, 20.f));
    healthbarBACK.setFillColor(sf::Color(100, 0, 0));

    // 2. إعداد المستطيل الأمامي (الأخضر)
    healthbarFRONT.setFillColor(sf::Color::Green);

    // 3. تحميل الخط وإعداد النص
    if (hpdeclaration.loadFromFile("assets/fonts/pixelsix00.ttf")) {
        hptexture.setFont(hpdeclaration);
        hptexture.setCharacterSize(18);
        hptexture.setFillColor(sf::Color::White);
    }
}

void drawHealthBar(sf::RenderWindow& window) {
    // 1. حساب النسبة المئوية
    float hpratio = (float)player.hp / (float)player.maxHp;
    if (hpratio < 0) hpratio = 0;

    // 2. تحديث حجم الشريط الأخضر
    healthbarFRONT.setSize(sf::Vector2f(200.f * hpratio, 20.f));

    // 3. تحديث النص (90 / 100)
    std::string hp_string = std::to_string(player.hp) + " / " + std::to_string(player.maxHp);
    hptexture.setString(hp_string);

    // 4. التموضع (ثابت في ركن الشاشة)
    sf::Vector2f uiPos(20.f, 20.f);
    healthbarBACK.setPosition(uiPos);
    healthbarFRONT.setPosition(uiPos);
    hptexture.setPosition(uiPos.x + 65.f, uiPos.y - 2.f);

    // 5. الرسم بالترتيب
    window.draw(healthbarBACK);
    window.draw(healthbarFRONT);
    window.draw(hptexture);
}