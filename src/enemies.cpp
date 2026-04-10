#include "enemies.h"
#include <vector>
#include <Data.h>
#include <player.h>
#include <SFML/Graphics.hpp>

using namespace sf;
enemy enemies[MAX_ENEMIES];
int enemyCount = 0;
static Sprite enemySprite;

void initEnemy(int index, Vector2f startPos, EnemyType type) {
    if (index >= MAX_ENEMIES) return;

    enemies[index].pos = startPos;
    enemies[index].speed = 60.f;
    enemies[index].facing = SOUTH;
    enemies[index].currentFrame = 0;
    enemies[index].animationTimer = 0.f;
    enemies[index].isMoving = false;
    enemies[index].isAlive = true;
    enemies[index].enemyType = type;
    enemies[index].cooldown_timer = 0.f;
    enemies[index].hurt_timer = 0.f;
    enemies[index].isInvincible = false;
    enemies[index].currentState = IDLE;
    //just trying smth out

    if (type == BASIC_ENEMY) {
        enemies[index].hp = 50;
        enemies[index].maxHp = 50;
        enemies[index].attack_damage = 10;
        enemies[index].detectionRange = 150.f;
        enemies[index].attackRange = 40.f;
    } else if (type == BOSS_ENEMY) {
        enemies[index].hp = 300;
        enemies[index].maxHp = 300;
        enemies[index].attack_damage = 25;
        enemies[index].detectionRange = 250.f;
        enemies[index].attackRange = 60.f;
    }

    enemies[index].walkTextures[SOUTH].loadFromFile("assets/sprites/mummy/walkingmummy/mummywalk(south).png");
    enemies[index].walkTextures[NORTH].loadFromFile("assets/sprites/mummy/walkingmummy/mummywalk(north).png");
    enemies[index].walkTextures[WEST].loadFromFile("assets/sprites/mummy/walkingmummy/mummywalk(west).png");
    enemies[index].walkTextures[EAST].loadFromFile("assets/sprites/mummy/walkingmummy/mummywalk(east).png");

    enemySprite.setScale(1.8f, 1.8f);
    enemySprite.setOrigin(34.f, 34.f); // Half of 68
    enemyCount++;
}

void updateEnemies(float dt) {
    for (int i = 0; i < enemyCount; i++) {
        if (!enemies[i].isAlive) continue;

        // Mummy Animation Logic
        enemies[i].animationTimer += dt;
        if (enemies[i].animationTimer >= 0.15f) {
            enemies[i].animationTimer = 0.f;
            enemies[i].currentFrame = (enemies[i].currentFrame + 1) % 6;
        }

        if (enemies[i].currentState == HURT) {
            enemies[i].hurt_timer -= dt;
            if (enemies[i].hurt_timer <= 0.f) {
                enemies[i].isInvincible = false;
                enemies[i].currentState = IDLE;
            }
        }
    }
}

void drawEnemy(RenderWindow& window) {
    for (int i = 0; i < enemyCount; i++) {
        if (!enemies[i].isAlive) continue;
        enemySprite.setTexture(enemies[i].walkTextures[enemies[i].facing]);
        enemySprite.setPosition(enemies[i].pos);
        enemySprite.setTextureRect(IntRect(enemies[i].currentFrame * 68, 0, 68, 68));

        if (enemies[i].isInvincible)
            enemySprite.setColor(sf::Color::Red);
        else
            enemySprite.setColor(sf::Color::White);

        window.draw(enemySprite);
    }
}

void checkAttackHits() {
    if (player.currentState != ATTACKING || player.currentFrame != 3) return;
    FloatRect hitbox = attackHitBox();
    for (int i = 0; i < enemyCount; i++) {
        if (!enemies[i].isAlive) continue;
        sf::FloatRect enemyBounds(enemies[i].pos.x - 34, enemies[i].pos.y - 34, 68, 68);
        if (hitbox.intersects(enemyBounds)) {
            if (enemies[i].isInvincible) continue;
            enemies[i].hp -= player.attack_damage;
            enemies[i].currentState = HURT;
            enemies[i].isInvincible = true;
            enemies[i].hurt_timer = 0.4f;
            if (enemies[i].hp <= 0) enemies[i].isAlive = false;
        }
    }
}