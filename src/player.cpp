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

        // تحميل الصور بمسارات صحيحة
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
        playerSprite.setScale(1.7f, 1.7f); // التكبير عشان الحجم يظبط مع الماب
        playerSprite.setOrigin(24.f, 24.f);

        //hitbox
        hitboxDebug.setFillColor(sf::Color(255, 0, 0, 100)); // أحمر شفاف
        hitboxDebug.setOutlineColor(sf::Color::Red);
        hitboxDebug.setOutlineThickness(1.f);
    }
    void initweapon(Vector2f startPos){
        weapon.currentWeapon = WEAPON_FIST;
        weapon.weaponOffset =Vector2f (0.f, 0.f);
        weapon.weaponRotation =0.f;
        weapon.weaponSwingSpeed =600.f;
        weapon.weaponSwingTarget =90.f;
        // Placeholder weapon shape
        weapon.weaponShape.setSize(sf::Vector2f(8.f, 32.f));
        weapon.weaponShape.setFillColor(sf::Color::Red);
        weapon.weaponShape.setOrigin(28.f, 15.f);
    }

    WeaponConfig sword = {15.f, 32.f, 600.f, 90.f, sf::Color::White};
    WeaponConfig book  = {20.f, 20.f, 300.f, 45.f, sf::Color::Blue};

    Vector2f getWeaponOffset(Direction dir) {
        switch (dir) {
            case SOUTH: return Vector2f(0.f, 10.f);  // Sword is slightly below player center
            case NORTH: return Vector2f(0.f, -10.f); // Sword is above player center
            case EAST:  return Vector2f(15.f, 0.f);  // Sword is to the right
            case WEST:  return Vector2f(-15.f, 0.f);// Sword is to the left
        }
        return Vector2f(0.f, 0.f);
    }

    void updateWeapon(float dt) {
        Vector2f offset = getWeaponOffset(player.facing);
        weapon.weaponShape.setPosition(player.pos.x + offset.x, player.pos.y + offset.y);

        // 2. Swing Logic
        if (player.currentState == ATTACKING) {
            weapon.weaponRotation += weapon.weaponSwingSpeed * dt;
            if (weapon.weaponRotation > weapon.weaponSwingTarget) {
                weapon.weaponRotation = weapon.weaponSwingTarget;
            }
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

        Vector2f velocity(0.f, 0.f);

        // ===== الحركة =====
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
        }

        Vector2f movement = velocity * player.speed * dt;
        Vector2f oldPos = player.pos;
        // ===== X AXIS =====
        player.pos.x += movement.x;
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
        window.draw(hitboxDebug);
    }
    void drawWeapons(sf::RenderWindow& window) {
        window.draw(weapon.weaponShape);
    }

// دالة لتحديث أنيميشن اللاعب من بره (للجت سين)
    void updatePlayerAnimation(Direction dir, float dt, bool moving) {
        player.facing = dir;
        player.isMoving = moving;
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
        playerSprite.setTextureRect(IntRect(player.currentFrame * 68, 0, 68, 68));
    }