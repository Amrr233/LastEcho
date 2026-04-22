//
// Created by vvolf on 18/04/2026.
//

#ifndef LASTECHO_BOSS_H
#define LASTECHO_BOSS_H

#pragma once
#include <SFML/Graphics.hpp>
#include "Data.h"


constexpr int MAX_FIREBALLS = 4;
// ── Change these values to tune the boss ──────────────────
struct BossConfig {
    int   maxHp          = 300;
    float phase2Threshold = 0.66f;  // switches to phase 2 at 66% HP
    float phase3Threshold = 0.33f;  // switches to phase 3 at 33% HP

    // Phase 1 stats
    float speedPhase1    = 60.f;
    int   damagePhase1   = 10;
    float scalePhase1    = 1.5f;

    // Phase 2 stats
    float speedPhase2    = 85.f;
    int   damagePhase2   = 18;
    float scalePhase2    = 1.9f;

    // Phase 3 stats
    float speedPhase3    = 110.f;
    int   damagePhase3   = 28;
    float scalePhase3    = 2.4f;

    // Where boss spawns relative to guide position
    sf::Vector2f spawnOffset = sf::Vector2f(150.f, 0.f);
};
// ─────────────────────────────────────────────────────────
struct Fireball {
    sf::Vector2f pos;
    sf::Vector2f velocity;
    sf::Sprite   sprite;
    bool         isActive = false;
    float        speed    = 300.f;
};

struct Boss {
    sf::Vector2f pos;
    sf::Vector2f velocity;
    float        speed;
    int          hp;
    int          maxHp;
    int          damage;
    float        scale;

    BossPhase    currentPhase;
    BossState    currentState;

    float        attackCooldown;
    float        attackTimer;
    float        attackRange;
    float        detectionRange;
    float        hurtTimer;
    bool         isInvincible;
    bool         isAlive;
    bool         isActive;     // false = fight hasn't started yet

    sf::Texture  texture;
    sf::Sprite   sprite;
    int          currentFrame;
    float        animTimer;
    // Scorpion textures
    sf::Texture walkTextures[4];
    sf::Texture runTextures[4];
    sf::Texture fireballThrowTextures[4];
    sf::Texture fireballTexture;      // single frame projectile
    sf::Texture crossPunchingTexture[4];

    // Fireball system
    Fireball    fireballs[MAX_FIREBALLS];
    float       fireballCooldown    = 2.f;
    float       fireballTimer       = 0.f;
    float       fireballThrowRange  = 250.f;  // min dist to throw
    float       fireballMinRange    = 80.f;   // too close = melee instead
    bool        isThrowing          = false;
    float       throwAnimTimer      = 0.f;
    int         throwAnimFrame      = 0;
};
struct RoundManager {
    int  currentRound = 1;    // 1, 2, or 3
    int  maxRounds    = 3;
    bool roundOver    = false;
    bool playerWon    = false;
    float breakTimer  = 0.f;
    float breakDuration = 3.f; // seconds between rounds
    bool  inBreak     = false;
};
extern RoundManager roundMan;
void initBoss();
void spawnBoss(sf::Vector2f guidePos);
void updateBoss(float dt);
void drawBoss(sf::RenderWindow& window);
void checkBossPhase();
void startRound(int round);
void updateRounds(float dt);
void checkBossPlayerCollision();
void spawnFireball(int count);
void updateFireballs(float dt);
void drawFireballs(sf::RenderWindow& window);
extern Boss boss;
extern BossConfig bossConfig;


#endif //LASTECHO_BOSS_H
