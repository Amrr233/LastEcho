#include "player.h"
#include "GameMap.h"
#include <SFML/Graphics.hpp>
#include <cmath>

using namespace sf;

extern Player player;
extern GameMap myMap;

static Sprite playerSprite;

void initPlayer(Vector2f startPos) {
    player.pos = startPos;
    player.speed = 800.f; // سرعة متوسطة عشان السلاسة
    player.facing = SOUTH;
    player.currentFrame = 0;
    player.animationTimer = 0.f;
    player.isMoving = false;

    player.frameWidth = 48;
    player.frameHeight = 48;

    // حملنا ملفات المشي فقط
    player.walkTextures[SOUTH].loadFromFile("assets/sprites/player/walking.south.png");
    player.walkTextures[NORTH].loadFromFile("assets/sprites/player/walking.north.png");
    player.walkTextures[WEST].loadFromFile("assets/sprites/player/walking.west.png");
    player.walkTextures[EAST].loadFromFile("assets/sprites/player/walking.east.png");

    playerSprite.setTexture(player.walkTextures[SOUTH]);
    playerSprite.setScale(1.7f, 1.7f); // التكبير عشان الحجم يظبط مع الماب
    playerSprite.setOrigin(24.f, 24.f);
}

void updatePlayer(float dt) {
    Vector2f velocity(0.f, 0.f);

    // 1. Input
    if (Keyboard::isKeyPressed(Keyboard::W)) velocity.y -= 1;
    if (Keyboard::isKeyPressed(Keyboard::S)) velocity.y += 1;
    if (Keyboard::isKeyPressed(Keyboard::A)) velocity.x -= 1;
    if (Keyboard::isKeyPressed(Keyboard::D)) velocity.x += 1;

    // 2. الحركة والاتجاه
    if (velocity.x != 0.f || velocity.y != 0.f) {
        player.isMoving = true;

        if (velocity.x > 0)      player.facing = EAST;
        else if (velocity.x < 0) player.facing = WEST;
        else if (velocity.y > 0) player.facing = SOUTH;
        else if (velocity.y < 0) player.facing = NORTH;

        float length = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        velocity /= length;
    } else {
        player.isMoving = false;
    }

    // 3. الموقع والحدود
    Vector2f nextPos = player.pos + (velocity * player.speed * dt);
    float mapW = (float)(myMap.width * myMap.tileSize);
    float mapH = (float)(myMap.height * myMap.tileSize);

    if (nextPos.x > 0 && nextPos.x < mapW) player.pos.x = nextPos.x;
    if (nextPos.y > 0 && nextPos.y < mapH) player.pos.y = nextPos.y;

    // 4. الأنميشن (المشي فقط)
    playerSprite.setTexture(player.walkTextures[player.facing]);

    if (player.isMoving) {
        player.animationTimer += dt;
        if (player.animationTimer >= 0.1f) { // سرعة الأنميشن (0.1 ثانية للفريم)
            player.animationTimer = 0.f;
            player.currentFrame = (player.currentFrame + 1) % 6; // الـ 6 فريمات بتوعك
        }
    } else {
        // لو مش بيتحرك، نثبت على الفريم رقم 0 (وضعية الوقوف الطبيعية في شيت المشي)
        player.currentFrame = 0;
    }

    playerSprite.setTextureRect(IntRect(player.currentFrame * 68, 0, 68, 68));
}

void drawPlayer(RenderWindow& window) {
    playerSprite.setPosition(player.pos);
    window.draw(playerSprite);
}