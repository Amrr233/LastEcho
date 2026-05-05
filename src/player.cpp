#include "player.h"
#include "GameMap.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include "enemies.h"
#include "NPC.h"
#include "Cutscene.h"
#include "audio.h"
using namespace sf;

extern Player player;
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
    player.swordTextures[SOUTH].loadFromFile("assets/sprites/player/swingingSword/swingingSwordSouth.png");
    player.swordTextures[NORTH].loadFromFile("assets/sprites/player/swingingSword/swingingSwordNorth.png");
    player.swordTextures[EAST].loadFromFile("assets/sprites/player/swingingSword/swingingSwordEast.png");
    player.swordTextures[WEST].loadFromFile("assets/sprites/player/swingingSword/swingingSwordWest.png");

    player.attack_damage = 10;
    player.cooldown_timer = 0.f;
    player.cooldown_maxtime = 0.2f;
    player.attack_range = 50.f;
    player.hurt_timer = 0.f;
    player.isInvincible = false;
    player.currentState = IDLE;

    // for testing — revert to WEAPON_FIST and hasSword=false when done
    player.hasSword = true;
    player.weaponEquipped = WEAPON_FIST;

    playerSprite.setTexture(player.walkTextures[SOUTH]);
    playerSprite.setScale(1.7f, 1.7f);
    playerSprite.setOrigin(24.f, 24.f);
}

void handlingAttack(float dt) {
    if (player.currentState == ATTACKING) {
        int totalFrames = (player.weaponEquipped == WEAPON_SWORD) ? 9 : 6;

        player.animationTimer += dt;
        if (player.animationTimer >= 0.08f) {
            player.animationTimer = 0.f;
            player.currentFrame++;

            if (player.currentFrame == 3)
                checkAttackHits();

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

void updatePlayer(float dt, World& world) {
    if (player.hp <= 0) {
        player.currentState = DEAD;
        return;
    }

    if (isCutsceneActive()) {
        playerSprite.setPosition(player.pos);
        return;
    }

    GameMap* currentMapPtr = worldGetCurrentMap(world);
    if (!currentMapPtr) return;
    GameMap& myMap = *currentMapPtr;

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

            // normalize so diagonal isn't faster
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

    // X axis collision
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

    // Y axis collision
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

    // map bounds
    float mapW = static_cast<float>(myMap.width * myMap.tileSize);
    float mapH = static_cast<float>(myMap.height * myMap.tileSize);
    if (player.pos.x < 0) player.pos.x = 0;
    if (player.pos.x > mapW) player.pos.x = mapW;
    if (player.pos.y < 0) player.pos.y = 0;
    if (player.pos.y > mapH) player.pos.y = mapH;

    // toggling between sword and fist by pressing t
    static bool tWasPressed = false;
    bool tNow = sf::Keyboard::isKeyPressed(sf::Keyboard::T);
    if (tNow && !tWasPressed && player.hasSword) {
        if (player.weaponEquipped == WEAPON_FIST)
            player.weaponEquipped = WEAPON_SWORD;
        else
            player.weaponEquipped = WEAPON_FIST;
    }
    tWasPressed = tNow;

    handlingHurt(dt);
    handlingAttack(dt);

    // animation
    if (player.currentState == ATTACKING) {
        if (player.weaponEquipped == WEAPON_SWORD)
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