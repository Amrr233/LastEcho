#include "player.h"
#include "GameMap.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include "enemies.h"
#include "NPC.h"
#include  "Cutscene.h"
    using namespace sf;

    extern Player player;
    weapons weapon;
    static Sprite playerSprite;
    static sf::RectangleShape hitboxDebug;

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

    player.hasSword = false;
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
    }

    WeaponConfig sword = {15.f, 32.f, 600.f, 90.f, sf::Color::White};
    WeaponConfig book  = {20.f, 20.f, 300.f, 45.f, sf::Color::Blue};

    if (player.currentState == ATTACKING) {
        weapon.weaponRotation += weapon.weaponSwingSpeed * dt;
        if (weapon.weaponRotation > weapon.weaponSwingTarget)
            weapon.weaponRotation = weapon.weaponSwingTarget;
    } else {
        weapon.weaponRotation = -90.f;
    }
    weapon.weaponShape.setRotation(weapon.weaponRotation);
}

void weapons::switching(weaponType type) {
    currentWeapon = type;
    switch(type) {
        case WEAPON_FIST: weaponShape.setSize({8.f, 32.f}); weaponShape.setFillColor(sf::Color::White); weaponShape.setOrigin(4.f, 28.f); break;
        case WEAPON_BOOK: weaponShape.setSize({20.f, 20.f}); weaponShape.setFillColor(sf::Color::Blue); weaponShape.setOrigin(10.f, 10.f); break;
    }
}

void updatePlayer(float dt, World& world) {
    if (player.hp <= 0) {
        player.currentState = DEAD;
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
            if (velocity.x > 0)      player.facing = EAST;
            else if (velocity.x < 0) player.facing = WEST;
            else if (velocity.y > 0) player.facing = SOUTH;
            else if (velocity.y < 0) player.facing = NORTH;

            float length = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
            velocity /= length;
        } else {
            weapon.weaponRotation = -90.f;
        }

        // 3. Apply rotation to the shape
        weapon.weaponShape.setRotation(weapon.weaponRotation);
    }

    void weapons::switching(weaponType type) {
        currentWeapon = type;
        switch(type) {
            case WEAPON_FIST:   weaponShape.setSize({8.f, 32.f}); weaponShape.setFillColor(sf::Color::White); weaponShape.setOrigin(4.f, 28.f); break;
            case WEAPON_BOOK:   weaponShape.setSize({20.f, 20.f}); weaponShape.setFillColor(sf::Color::Blue); weaponShape.setOrigin(10.f, 10.f); break;
        }
    }

    void updatePlayer(float dt, World& world) {
        // 🔥 إضافة السطر ده: لو فيه كت سين، اخرج ومتحسبش حركة الكيبورد
        // (الكت سين هي اللي هتحرك الـ player.pos مباشرة)
        if (isCutsceneActive()) {
            playerSprite.setPosition(player.pos); // بس حدث مكان السبرايت
            return;
        }
        GameMap* currentMapPtr = worldGetCurrentMap(world);
        if (!currentMapPtr) return;
        GameMap& myMap = *currentMapPtr;

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

        // تحديث hitbox
        sf::FloatRect bounds = playerSprite.getGlobalBounds();
        float hbW = bounds.width * 0.3f;
        float hbH = bounds.height * 0.2f;

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

        // ===== Y AXIS =====
        player.pos.y += movement.y;
        playerSprite.setPosition(player.pos);

        // تحديث hitbox تاني
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

        // ===== حدود الماب =====
        float mapW = (float)(myMap.width * myMap.tileSize);
        float mapH = (float)(myMap.height * myMap.tileSize);

        if (player.pos.x < 0) player.pos.x = 0;
        if (player.pos.x > mapW) player.pos.x = mapW;
        if (player.pos.y < 0) player.pos.y = 0;
        if (player.pos.y > mapH) player.pos.y = mapH;

        // ===== الأنيميشن =====
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

        if (Mouse::isButtonPressed(Mouse::Left) && player.cooldown_timer <= 0.f) {
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
    float mapW = (float)(myMap.width * myMap.tileSize);
    float mapH = (float)(myMap.height * myMap.tileSize);
    if (player.pos.x < 0) player.pos.x = 0;
    if (player.pos.x > mapW) player.pos.x = mapW;
    if (player.pos.y < 0) player.pos.y = 0;
    if (player.pos.y > mapH) player.pos.y = mapH;

    // sword switch with Tab
    static bool tabWasPressed = false;
    bool tabNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Tab);
    if (tabNow && !tabWasPressed && player.hasSword) {
        if (player.swordEquipped == WEAPON_FIST)
            player.swordEquipped = WEAPON_SWORD;
        else
            player.swordEquipped = WEAPON_FIST;
    }
    tabWasPressed = tabNow;

    handlingHurt(dt);
    handlingAttack(dt);

    // animation
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

void handlingAttack(float dt) {
    // if already attacking, advance the animation
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

    // trigger attack on Space
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

sf::FloatRect attackHitBox() {
    float x = player.pos.x;
    float y = player.pos.y;
    float range = player.attack_range;
    float size = 40.f;
    switch (player.facing) {
        case SOUTH: return {x - size/2, y, size, range};
        case NORTH: return {x - size/2, y - range, size, range};
        case EAST:  return {x, y - size/2, range, size};
        case WEST:  return {x - range, y - size/2, range, size};
    }
    return sf::FloatRect(x, y, size, range);
}

void drawPlayer(RenderWindow& window) {
    playerSprite.setPosition(player.pos);
    window.draw(playerSprite);
}

void drawWeapons(sf::RenderWindow& window) {
    window.draw(weapon.weaponShape);
}
