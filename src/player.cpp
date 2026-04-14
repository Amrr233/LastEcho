#include "player.h"
#include "GameMap.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include "enemies.h"

using namespace sf;

extern Player player;
static Sprite playerSprite;

// Helper function for collision detection
bool isColliding(FloatRect rect, GameMap& map) {
    // بنحول إحداثيات الـ hitbox لإحداثيات تايلات (Tiles)
    int leftTile   = static_cast<int>(rect.left / map.tileSize);
    int rightTile  = static_cast<int>((rect.left + rect.width) / map.tileSize);
    int topTile    = static_cast<int>(rect.top / map.tileSize);
    int bottomTile = static_cast<int>((rect.top + rect.height) / map.tileSize);

    // بنلف على كل الليرات اللي عندك
    for (const auto& layer : map.layers) {
        // بنشيك فقط على الليرات اللي إنت محدد إنها للحوائط أو التصادم
        // لو لير الأرضية اسمه "Ground" فإحنا مش عاوزين نصطدم بيه
        if (layer.name == "Ground" || layer.name =="ground" || layer.name=="solid") continue;

        for (int y = topTile; y <= bottomTile; ++y) {
            for (int x = leftTile; x <= rightTile; ++x) {
                if (x >= 0 && x < map.width && y >= 0 && y < map.height) {
                    // هنا بنوصل للداتا الحقيقية من الـ struct بتاعك
                    if (layer.data[y * map.width + x] != 0) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void initPlayer(Vector2f startPos) {
    player.pos = startPos;
    player.speed = 240.f;
    player.facing = SOUTH;
    player.currentFrame = 0;
    player.animationTimer = 0.f;
    player.isMoving = false;

    player.walkTextures[SOUTH].loadFromFile("assets/sprites/player/walking/walking.south.png");
    player.walkTextures[NORTH].loadFromFile("assets/sprites/player/walking/walking.north.png");
    player.walkTextures[WEST].loadFromFile("assets/sprites/player/walking/walking.west.png");
    player.walkTextures[EAST].loadFromFile("assets/sprites/player/walking/walking.east.png");

    player.attackTextures[SOUTH].loadFromFile("assets/sprites/player/punching/crosspunching.south.png");
    player.attackTextures[NORTH].loadFromFile("assets/sprites/player/punching/crosspunching.north.png");
    player.attackTextures[WEST].loadFromFile("assets/sprites/player/punching/crosspunching.west.png");
    player.attackTextures[EAST].loadFromFile("assets/sprites/player/punching/crosspunching.east.png");

    player.attack_damage = 10;
    player.cooldown_timer = 0.f;
    player.cooldown_maxtime = 0.5f;
    player.attack_range = 50.f;
    player.hurt_timer = 0.f;
    player.isInvincible = false;
    player.currentState = IDLE;
    playerSprite.setTexture(player.walkTextures[SOUTH]);
    playerSprite.setScale(1.7f, 1.7f);
    playerSprite.setOrigin(24.f, 24.f);
}

void updatePlayer(float dt, World& world) {
    GameMap* currentMapPtr = worldGetCurrentMap(world);
    if (!currentMapPtr) return;
    GameMap& myMap = *currentMapPtr;

    Vector2f velocity(0.f, 0.f);

    if (player.currentState != ATTACKING && player.currentState != HURT) {
        if (Keyboard::isKeyPressed(Keyboard::W)) velocity.y -= 1;
        if (Keyboard::isKeyPressed(Keyboard::S)) velocity.y += 1;
        if (Keyboard::isKeyPressed(Keyboard::A)) velocity.x -= 1;
        if (Keyboard::isKeyPressed(Keyboard::D)) velocity.x += 1;

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
    } else {
        player.isMoving = false;
    }

    // --- COLLISION RESOLUTION ---
    Vector2f moveStep = velocity * player.speed * dt;
    float hbWidth = 20.f;
    float hbHeight = 15.f;

    // 1. Try X movement
    Vector2f oldPos = player.pos;
    player.pos.x += moveStep.x;
    FloatRect hbX(player.pos.x - hbWidth/2 + 20.f, player.pos.y + 41.f, hbWidth, hbHeight);
    if (isColliding(hbX, myMap)) {
        player.pos.x = oldPos.x;
    }

    // 2. Try Y movement
    player.pos.y += moveStep.y;
    FloatRect hbY(player.pos.x - hbWidth/2 + 20.f, player.pos.y + 41.f, hbWidth, hbHeight);
    if (isColliding(hbY, myMap)) {
        player.pos.y = oldPos.y;
    }

    // 3. Boundary constraints
    float mapW = (float)(myMap.width * myMap.tileSize);
    float mapH = (float)(myMap.height * myMap.tileSize);
    if (player.pos.x < 0) player.pos.x = 0;
    if (player.pos.x > mapW) player.pos.x = mapW;
    if (player.pos.y < 0) player.pos.y = 0;
    if (player.pos.y > mapH) player.pos.y = mapH;

    // --- REMAINING LOGIC ---
    handlingHurt(dt);
    handlingAttack(dt);

    if (player.currentState == ATTACKING) {
        playerSprite.setTexture(player.attackTextures[player.facing]);
    } else {
        playerSprite.setTexture(player.walkTextures[player.facing]);
        if (player.isMoving) {
            player.animationTimer += dt;
            if (player.animationTimer >= 0.1f) {
                player.animationTimer = 0.f;
                player.currentFrame = (player.currentFrame + 1) % 6;
            }
        } else {
            player.currentFrame = 0;
        }
    }

    playerSprite.setTextureRect(IntRect(player.currentFrame * 68, 0, 68, 68));
}

void handlingAttack(float dt) {
    if (player.currentState == ATTACKING) {
        player.animationTimer += dt;
        if (player.animationTimer >= 0.12f) {
            player.animationTimer = 0.f;
            player.currentFrame++;
            if (player.currentFrame == 3) {
                checkAttackHits();
            }
            if (player.currentFrame >= 6) {
                player.currentFrame = 0;
                player.currentState = IDLE;
            }
        }
        return;
    }

    if (player.cooldown_timer > 0.f)
        player.cooldown_timer -= dt;

    if (Keyboard::isKeyPressed(Keyboard::Space) && player.cooldown_timer <= 0.f) {
        player.currentState = ATTACKING;
        player.cooldown_timer = player.cooldown_maxtime;
        player.currentFrame = 0;
        player.animationTimer = 0.f;
    }
}

void handlingHurt(float dt) {
    if (player.currentState != HURT) return;
    player.hurt_timer -= dt;
    if (player.hurt_timer <= 0.f) {
        player.isInvincible = false;
        player.currentState = IDLE;
    }
}

sf::FloatRect attackHitBox() {
    float x = player.pos.x;
    float y = player.pos.y;
    float range = player.attack_range;
    float size = 40.f;
    switch (player.facing) {
        case SOUTH: return {x - size/2, y, size, range};
        case NORTH: return {x - size/2, y - range, size, range};
        case EAST : return {x, y - size/2, range, size};
        case WEST : return {x - range, y - size/2, range, size};
    }
    return sf::FloatRect(x, y, size, range);
}

void drawPlayer(RenderWindow& window) {
    playerSprite.setPosition(player.pos);
    window.draw(playerSprite);
}