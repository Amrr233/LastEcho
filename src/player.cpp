#include "player.h"
#include "GameMap.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include "enemies.h"
#include "NPC.h"
#include "Cutscene.h"
#include  "audio.h"
using namespace sf;

extern Player player;
weapons weapon;
static Sprite playerSprite;
static sf::RectangleShape hitboxDebug;
extern AudioManager audioManager;

void initPlayer(Vector2f startPos) {
    player.pos = startPos;
    player.speed = 240.f;
    player.facing = SOUTH;
    player.currentFrame = 0;
    player.animationTimer = 0.f;
    player.isMoving = false;

    // walk textures
    player.walkTextures[SOUTH].loadFromFile("assets/sprites/player/walking/walking.south.png");
    player.walkTextures[NORTH].loadFromFile("assets/sprites/player/walking/walking.north.png");
    player.walkTextures[WEST].loadFromFile("assets/sprites/player/walking/walking.west.png");
    player.walkTextures[EAST].loadFromFile("assets/sprites/player/walking/walking.east.png");

    // punch textures
    player.attackTextures[SOUTH].loadFromFile("assets/sprites/player/punching/crosspunching.south.png");
    player.attackTextures[NORTH].loadFromFile("assets/sprites/player/punching/crosspunching.north.png");
    player.attackTextures[WEST].loadFromFile("assets/sprites/player/punching/crosspunching.west.png");
    player.attackTextures[EAST].loadFromFile("assets/sprites/player/punching/crosspunching.east.png");

    // sword textures
    player.swordTextures[NORTH].loadFromFile("assets/sprites/player/swingingSword/swingingSwordSouth.png");
    player.swordTextures[SOUTH].loadFromFile("assets/sprites/player/swingingSword/swingingSwordNorth.png");
    player.swordTextures[EAST].loadFromFile("assets/sprites/player/swingingSword/swingingSwordEast.png");
    player.swordTextures[WEST].loadFromFile("assets/sprites/player/swingingSword/swingingSwordWest.png");

    player.attack_damage = 10;
    player.cooldown_timer = 0.f;
    player.cooldown_maxtime = 0.2f;
    player.attack_range = 50.f;
    player.hurt_timer = 0.f;
    player.isInvincible = false;
    player.currentState = IDLE;

    player.hasSword = true;
    player.swordEquipped = WEAPON_FIST;

    playerSprite.setTexture(player.walkTextures[SOUTH]);
    playerSprite.setScale(1.7f, 1.7f);
    playerSprite.setOrigin(24.f, 24.f);
}

void initweapon(Vector2f startPos) {
    weapon.currentWeapon = WEAPON_FIST;
    weapon.weaponOffset = Vector2f(0.f, 0.f);
    weapon.weaponRotation = 0.f;
    weapon.weaponSwingSpeed = 600.f;
    weapon.weaponSwingTarget = 90.f;
    weapon.weaponShape.setSize(sf::Vector2f(8.f, 32.f));
    weapon.weaponShape.setFillColor(sf::Color::Red);
    weapon.weaponShape.setOrigin(28.f, 15.f);
}


WeaponConfig sword = {15.f, 32.f, 600.f, 90.f, sf::Color::White};
WeaponConfig book  = {20.f, 20.f, 300.f, 45.f, sf::Color::Blue};

Vector2f getWeaponOffset(Direction dir) {
    switch (dir) {
        case SOUTH: return Vector2f(0.f, 10.f);
        case NORTH: return Vector2f(0.f, -10.f);
        case EAST:  return Vector2f(15.f, 0.f);
        case WEST:  return Vector2f(-15.f, 0.f);
        default:    return Vector2f(0.f, 0.f);
    }
}
void updateWeapon(float dt) {
    // 1. Get correct offset based on facing direction
    sf::Vector2f offset = getWeaponOffset(player.facing);
    weapon.weaponShape.setPosition(player.pos.x + offset.x, player.pos.y + offset.y);

    // 2. Swing logic
    if (player.currentState == ATTACKING) {
        weapon.weaponRotation += weapon.weaponSwingSpeed * dt;
        if (weapon.weaponRotation > weapon.weaponSwingTarget) {
            weapon.weaponRotation = weapon.weaponSwingTarget;
        }
    } else {
        weapon.weaponRotation = -90.f; // Rest position
    }

    // 3. Apply rotation to the shape
    weapon.weaponShape.setRotation(weapon.weaponRotation);
}

void weapons::switching(weaponType type) {
    currentWeapon = type;
    switch(type) {
        case WEAPON_FIST: weaponShape.setSize({8.f, 32.f}); weaponShape.setFillColor(sf::Color::White); weaponShape.setOrigin(4.f, 28.f); break;
        case WEAPON_BOOK: weaponShape.setSize({20.f, 20.f}); weaponShape.setFillColor(sf::Color::Blue); weaponShape.setOrigin(10.f, 10.f); break;
        default: break;
    }
}

void handlingAttack(float dt) {
    if (player.currentState == ATTACKING) {
        int totalFrames = (player.swordEquipped == WEAPON_SWORD) ? 9 : 6;

        player.animationTimer += dt;
        if (player.animationTimer >= 0.08f) {
            player.animationTimer = 0.f;
            player.currentFrame++;

            if (player.currentFrame == 3) {
                checkAttackHits();
            }

            if (player.currentFrame >= totalFrames) {
                player.currentFrame = 0;
                player.currentState = IDLE;
            }
        }
        return;
    }

    if (Keyboard::isKeyPressed(Keyboard::Scancode::Space) && player.cooldown_timer <= 0.f) {
        player.currentState = ATTACKING;
        player.cooldown_timer = player.cooldown_maxtime;
        player.currentFrame = 0;
        player.animationTimer = 0.f;
    }

    if (player.cooldown_timer > 0.f)
        player.cooldown_timer -= dt;
}

void handlingHurt(float dt) {
    if (player.currentState != HURT) return;
    player.hurt_timer -= dt;
    if (player.hurt_timer <= 0.f) {
        player.isInvincible = false;
        player.currentState = IDLE;
    }
}
void updatePlayerAnimation(Direction dir, float dt, bool moving) {
    player.facing = dir;

    if (player.currentState == ATTACKING) {
        if (player.swordEquipped == WEAPON_SWORD)
            playerSprite.setTexture(player.swordTextures[player.facing]);
        else
            playerSprite.setTexture(player.attackTextures[player.facing]);
    } else {
        playerSprite.setTexture(player.walkTextures[player.facing]);
        if (moving) {
            player.animationTimer += dt;
            if (player.animationTimer >= 0.1f) {
                player.animationTimer = 0.f;
                player.currentFrame = (player.currentFrame + 1) % 6;
            }
        } else {
            player.currentFrame = 0;
        }
    }

    playerSprite.setTextureRect(sf::IntRect(player.currentFrame * 68, 0, 68, 68));
}
void updatePlayer(float dt, World& world) {
    if (player.hp <= 0) {
        player.currentState = DEAD;
        return;
    }

    // Check for cutscenes first
    if (isCutsceneActive()) {
        playerSprite.setPosition(player.pos);
        return;
    }

    // GameMap* currentMapPtr = worldGetCurrentMap(world);
    // if (!currentMapPtr) return;
    GameMap& myMap = *worldGetCurrentMap(world);

    Vector2f velocity(0.f, 0.f);

    // movement input
    if (player.currentState != ATTACKING && player.currentState != HURT) {
        if (Keyboard::isKeyPressed(Keyboard::W)) velocity.y -= 1;
        if (Keyboard::isKeyPressed(Keyboard::S)) velocity.y += 1;
        if (Keyboard::isKeyPressed(Keyboard::A)) velocity.x -= 1;
        if (Keyboard::isKeyPressed(Keyboard::D)) velocity.x += 1;

        if (velocity.x != 0.f || velocity.y != 0.f) {
            player.isMoving = true;
            audioManager.startFootsteps();
            if (velocity.x > 0)      player.facing = EAST;
            else if (velocity.x < 0) player.facing = WEST;
            else if (velocity.y > 0) player.facing = SOUTH;
            else if (velocity.y < 0) player.facing = NORTH;

            float length = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
            velocity /= length;
        } else {
            if (player.isMoving) {
                player.isMoving = false;
                audioManager.stopFootsteps();
            }
        }
    }

    Vector2f movement = velocity * player.speed * dt;
    Vector2f oldPos = player.pos;

    // ===== X AXIS COLLISION =====
    player.pos.x += movement.x;
    playerSprite.setPosition(player.pos);

    sf::FloatRect bounds = playerSprite.getGlobalBounds();
    float hbW = bounds.width * 0.4f;
    float hbH = bounds.height * 0.25f;

    hitboxDebug.setSize({hbW, hbH});
    hitboxDebug.setPosition(
        bounds.left + (bounds.width - hbW) / 2.f,
        bounds.top + bounds.height - hbH
    );

    if (mapCheckCollision(myMap, hitboxDebug.getGlobalBounds(), world.currentMapName) ||
        checkNPCCollision(hitboxDebug.getGlobalBounds(), world.currentMapName)) {
        player.pos.x = oldPos.x;
        playerSprite.setPosition(player.pos);
    }

    // ===== Y AXIS COLLISION =====
    player.pos.y += movement.y;
    playerSprite.setPosition(player.pos);

    bounds = playerSprite.getGlobalBounds();
    hitboxDebug.setPosition(
        bounds.left + (bounds.width - hbW) / 2.f,
        bounds.top + bounds.height - hbH
    );

    if (mapCheckCollision(myMap, hitboxDebug.getGlobalBounds(), world.currentMapName) ||
        checkNPCCollision(hitboxDebug.getGlobalBounds(), world.currentMapName)) {
        player.pos.y = oldPos.y;
        playerSprite.setPosition(player.pos);
    }

    // ===== MAP BOUNDS =====
    float mapW = static_cast<float>(myMap.width * myMap.tileSize);
    float mapH = static_cast<float>(myMap.height * myMap.tileSize);
    if (player.pos.x < 0) player.pos.x = 0;
    if (player.pos.x > mapW) player.pos.x = mapW;
    if (player.pos.y < 0) player.pos.y = 0;
    if (player.pos.y > mapH) player.pos.y = mapH;

    // ===== SWORD SWITCHING =====
    static bool tWasPressed = false;
    bool tNow = sf::Keyboard::isKeyPressed(sf::Keyboard::T);
    if (tNow && !tWasPressed && player.hasSword) {
        if (player.swordEquipped == WEAPON_FIST)
            player.swordEquipped = WEAPON_SWORD;
        else
            player.swordEquipped = WEAPON_FIST;
    }
    tWasPressed = tNow;

    handlingHurt(dt);
    handlingAttack(dt);

    // ===== ANIMATION =====
    if (player.currentState == ATTACKING) {
        if (player.swordEquipped == WEAPON_SWORD)
            playerSprite.setTexture(player.swordTextures[player.facing]);
        else
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

sf::FloatRect attackHitBox() {
    float x = player.pos.x;
    float y = player.pos.y;
    float range = player.attack_range;
    float size = 40.f;
    switch (player.facing) {
        case SOUTH: return {x - size / 2.f, y, size, range};
        case NORTH: return {x - size / 2.f, y - range, size, range};
        case EAST:  return {x, y - size / 2.f, range, size};
        case WEST:  return {x - range, y - size / 2.f, range, size};
        default:    return {x, y, size, range};
    }
}

void drawPlayer(RenderWindow& window) {
    playerSprite.setPosition(player.pos);
    window.draw(playerSprite);
}

void drawWeapons(sf::RenderWindow& window) {
    window.draw(weapon.weaponShape);
}