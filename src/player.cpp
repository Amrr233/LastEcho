#include "player.h"
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
static sf::RectangleShape playerRect;

// ==============================
// FUNCTIONS
// ==============================
void initPlayer(sf::Vector2f startPos) {
    player.pos      = startPos;
    player.velocity = sf::Vector2f(0, 0);
    player.speed    = 170.f; // السرعة اللي إنت ظبطتها
    player.hp       = 100;
    player.maxHp    = 100;
    player.xp       = 0;
    player.level    = 1;
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

void drawPlayer(sf::RenderWindow& window) {
    playerRect.setPosition(player.pos);
    window.draw(playerRect);
}