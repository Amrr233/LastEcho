#include "player.h"
#include "healthbar.h"
#include "GameMap.h"
#include <SFML/Graphics.hpp>

// ==============================
// EXTERN GLOBALS
// ==============================
extern Player    player;
extern GameMap   myMap; // دلوقتى ده Struct ملوش Methods

// ==============================
// LOCAL VARIABLES
// ==============================
//player settings
//============
static sf::RectangleShape playerRect;
//============

// ==============================
// FUNCTIONS
// ==============================
void initPlayer(sf::Vector2f startPos) {
    player.pos      = startPos;
    player.velocity = sf::Vector2f(0, 0);
    player.speed    = 170.f; // السرعة اللي إنت ظبطتها
    player.hp       = 100;
    player.maxHp    = 100;
    player.xp       = 50;
    player.level    = 1;
    player.xpToNextLevel=100;
    player.isMoving = false;
    player.facing   = DIR_DOWN;

    // حجم البلاير (لو الماب متموطة Stretch، ممكن تحتاج تكبر الـ 12 دي لـ 32 مثلاً)
    float size = 12.f;
    playerRect.setSize(sf::Vector2f(size, size));
    playerRect.setFillColor(sf::Color::Green);
    playerRect.setOrigin(
        playerRect.getSize().x / 2.f,
        playerRect.getSize().y / 2.f
    );
    //HEALTH BAR SHAPES
    // healthbarBACK.setSize(sf::Vector2f(200.f, 20.f));
    // healthbarBACK.setFillColor(sf::Color(100, 0, 0)); // خلفية رمادية
    //
    // healthbarFRONT.setFillColor(sf::Color::Green);
    // healthbarBACK.setSize(sf::Vector2f(200.f, 20.f));
    // healthbarBACK.setFillColor(sf::Color(100,0,0));
    // healthbarFRONT.setFillColor(sf::Color::Green);
    // if (hpdeclaration.loadFromFile("assets/fonts/pixelsix00.ttf")) {
    //     hptexture.setFont((hpdeclaration));
    //     hptexture.setCharacterSize(20);
    //     hptexture.setFillColor(sf::Color::White);
    // }

}

void updatePlayer(float dt) {
    sf::Vector2f velocity(0.f, 0.f);

    // WASD Input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        velocity.y = -player.speed;
        player.facing = DIR_UP;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        velocity.y = player.speed;
        player.facing = DIR_DOWN;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocity.x = -player.speed;
        player.facing = DIR_LEFT;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        velocity.x = player.speed;
        player.facing = DIR_RIGHT;
    }

    // Normalize diagonal movement
    if (velocity.x != 0.f && velocity.y != 0.f) {
        velocity.x *= 0.7071f;
        velocity.y *= 0.7071f;
    }


    player.isMoving = (velocity.x != 0.f || velocity.y != 0.f);

    // --- Bounds Checking (حساب حدود الماب مباشرة من الـ Struct) ---

    float mapLimitX = (float)(myMap.width * myMap.tileSize);
    float mapLimitY = (float)(myMap.height * myMap.tileSize);

    // التعديل في محور X
    float nextX = player.pos.x + velocity.x * dt;
    if (nextX > 0 && nextX < mapLimitX) {
        player.pos.x = nextX;
    }

    // التعديل في محور Y
    float nextY = player.pos.y + velocity.y * dt;
    if (nextY > 0 && nextY < mapLimitY) {
        player.pos.y = nextY;
    }
}
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
// void drawhealthbar(sf::RenderWindow& window) {
//     float hpratio = (float)player.hp / (float)player.maxHp;
//     healthbarFRONT.setSize(sf::Vector2f(200.f * hpratio, 20.f));
//
//     std::string hp_string=std::to_string(player.hp) + " / " + std::to_string(player.maxHp);
//     hptexture.setString(hp_string);
//
//     hptexture.setPosition(sf::Vector2f(18.f, 18.f));
//     sf::Vector2f uiPos(20.f, 20.f);
//     healthbarBACK.setPosition(uiPos);
//     healthbarFRONT.setPosition(uiPos);
//
//     hptexture.setPosition(uiPos.x + 65.f, uiPos.y);
//
//     window.draw(healthbarBACK);
//     window.draw(healthbarFRONT);
//     window.draw(hptexture);
// }

void drawPlayer(sf::RenderWindow& window) {
    playerRect.setPosition(player.pos);
    window.draw(playerRect);
}