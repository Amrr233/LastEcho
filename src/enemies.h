#pragma once
#include <SFML/Graphics.hpp>
#include "player.h"
#include "Data.h"

#ifndef LASTECHO_ENEMY_H
#define LASTECHO_ENEMY_H

// Maximum amount of enemies allowed on a map
constexpr int MAX_ENEMIES = 20;

enum EnemyType {
    BASIC_ENEMY,
    BOSS_ENEMY
};

struct enemy {
    sf::Vector2f pos;
    sf::Vector2f velocity;
    float speed;

    // Textures & Animation
    sf::Texture walkTextures[4];
    Direction   facing;
    playerState currentState;
    int   currentFrame;
    float animationTimer;
    bool  isMoving;

    // Dimensions (Mummy sheets are 68x68)
    int frameWidth = 68;
    int frameHeight = 68;

    // Combat & Stats
    int   hp;
    int   maxHp;
    int   attack_damage;
    float detectionRange;
    float attackRange;
    float cooldown_timer;
    float cooldown_maxtime = 0.5f;
    float hurt_timer = 0.4f;

    bool  isInvincible = false;
    bool  isAlive = false;
    bool  isBoss = false; // Fixed typo from 'idBoss'

    EnemyType enemyType;
};

// Function Prototypes
void initEnemy(int index, sf::Vector2f startPos, EnemyType type);
void updateEnemies(float dt);
void drawEnemy(sf::RenderWindow& window);
void checkAttackHits();

// Global array and counter
extern enemy enemies[MAX_ENEMIES];
extern int enemyCount;

#endif // LASTECHO_ENEMY_H