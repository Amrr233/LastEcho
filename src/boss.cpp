//
// Created by vvolf on 18/04/2026.
//
#include "boss.h"
#include<Data.h>
#include "player.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
BossConfig bossConfig;
Boss boss;


using namespace sf;
void initBoss() {
    boss.isActive  = false;
    boss.isAlive   = false;
    boss.hp        = bossConfig.maxHp;
    boss.maxHp     = bossConfig.maxHp;
    boss.currentPhase = BOSS_PHASE_1;
    boss.currentState = BOSS_IDLE;
    boss.attackTimer  = 0.f;
    boss.attackCooldown = 1.5f;
    boss.hurtTimer    = 0.f;
    boss.isInvincible = false;
    boss.currentFrame = 0;
    boss.animTimer    = 0.f;
    boss.detectionRange = 200.f;
    boss.attackRange    = 60.f;

    // Load texture
    boss.texture.loadFromFile("assets/sprites/mummy/walkingmummy/mummywalk(south).png");
    boss.sprite.setTexture(boss.texture);
    boss.sprite.setOrigin(34.f, 34.f);
}

void spawnBoss(sf::Vector2f guidePos) {
    boss.pos          = guidePos + bossConfig.spawnOffset;
    boss.speed        = bossConfig.speedPhase1;
    boss.damage       = bossConfig.damagePhase1;
    boss.scale        = bossConfig.scalePhase1;
    boss.hp           = bossConfig.maxHp;
    boss.maxHp        = bossConfig.maxHp;
    boss.isAlive      = true;
    boss.isActive     = true;
    boss.currentPhase = BOSS_PHASE_1;
    boss.currentState = BOSS_IDLE;
    boss.isInvincible = false;
    boss.hurtTimer    = 0.f;
    boss.attackTimer  = 0.f;
    boss.attackCooldown = 1.5f;
    boss.sprite.setScale(boss.scale, boss.scale);
}
void checkBossPhase() {
    float hpPercent = (float)boss.hp / (float)boss.maxHp;

    if (hpPercent <= bossConfig.phase3Threshold
        && boss.currentPhase != BOSS_PHASE_3) {
        boss.currentPhase = BOSS_PHASE_3;
        boss.speed        = bossConfig.speedPhase3;
        boss.damage       = bossConfig.damagePhase3;
        boss.scale        = bossConfig.scalePhase3;
        boss.sprite.setScale(boss.scale, boss.scale);
    }
    else if (hpPercent <= bossConfig.phase2Threshold
             && boss.currentPhase == BOSS_PHASE_1) {
        boss.currentPhase = BOSS_PHASE_2;
        boss.speed        = bossConfig.speedPhase2;
        boss.damage       = bossConfig.damagePhase2;
        boss.scale        = bossConfig.scalePhase2;
        boss.sprite.setScale(boss.scale, boss.scale);
    }
}

void updateBoss(float dt) {
    if (!boss.isActive || !boss.isAlive) return;

    boss.sprite.setTextureRect(sf::IntRect(boss.currentFrame * 68, 0, 68, 68));
    boss.sprite.setPosition(boss.pos);
    checkBossPlayerCollision();

    // Invincibility timer
    if (boss.isInvincible) {
        boss.hurtTimer -= dt;
        if (boss.hurtTimer <= 0.f) {
            boss.isInvincible = false;
            boss.currentState = BOSS_IDLE;
        }
        // still move while invincible but don't run full logic
        boss.pos += (player.pos - boss.pos) /
                    std::max(1.f, std::sqrt(
                        (player.pos.x-boss.pos.x)*(player.pos.x-boss.pos.x)+
                        (player.pos.y-boss.pos.y)*(player.pos.y-boss.pos.y)
                    )) * (boss.speed * 0.5f) * dt;
        goto animate;
    }

    checkBossPhase();

    {
        sf::Vector2f dir = player.pos - boss.pos;
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        if (dist < boss.detectionRange) {
            sf::FloatRect bossBounds = boss.sprite.getGlobalBounds();
            if (dist > (bossBounds.width / 2.0f) + 5.f) {
                boss.currentState = BOSS_WALKING;
                dir /= dist;
                boss.pos += dir * boss.speed * dt;
            } else {
                boss.currentState = BOSS_ATTACKING;
                boss.attackTimer += dt;
                if (boss.attackTimer >= boss.attackCooldown) {
                    boss.attackTimer = 0.f;
                    if (!player.isInvincible) {
                        player.hp -= boss.damage;
                        player.currentState = HURT;
                        player.hurt_timer = 0.4f;
                        player.isInvincible = true;
                        std::cout << "Player Damaged! HP: " << player.hp << std::endl;
                    }
                }
            }
        } else {
            boss.currentState = BOSS_IDLE;
        }
    }

    animate:
    boss.animTimer += dt;
    if (boss.animTimer >= 0.15f) {
        boss.animTimer = 0.f;
        boss.currentFrame = (boss.currentFrame + 1) % 6;
    }
}
void drawBoss(sf::RenderWindow& window) {
    if (!boss.isActive || !boss.isAlive) return;

    // Flash red when hurt
    if (boss.isInvincible)
        boss.sprite.setColor(sf::Color::Red);
    else
        boss.sprite.setColor(sf::Color::White);

    boss.sprite.setPosition(boss.pos);
    window.draw(boss.sprite);
}
RoundManager roundMan;

void startRound(int round) {
    roundMan.currentRound = round;
    roundMan.roundOver    = false;
    roundMan.inBreak      = false;

    // Each round boss gets stronger
    if (round == 1) {
        bossConfig.maxHp      = 100;
        bossConfig.speedPhase1 = 60.f;
        bossConfig.damagePhase1 = 10;
        bossConfig.scalePhase1  = 1.5f;
    } else if (round == 2) {
        bossConfig.maxHp      = 150;
        bossConfig.speedPhase1 = 85.f;
        bossConfig.damagePhase1 = 18;
        bossConfig.scalePhase1  = 1.9f;
    } else if (round == 3) {
        bossConfig.maxHp      = 200;
        bossConfig.speedPhase1 = 110.f;
        bossConfig.damagePhase1 = 28;
        bossConfig.scalePhase1  = 2.4f;
    }
    spawnBoss(sf::Vector2f(500.f, 400.f));
}

void updateRounds(float dt) {
    // Boss died — round won
    if (!boss.isAlive && !roundMan.roundOver && !roundMan.inBreak) {
        roundMan.roundOver = true;

        if (roundMan.currentRound >= roundMan.maxRounds) {
            roundMan.playerWon = true;
        } else {
            roundMan.inBreak    = true;
            roundMan.breakTimer = 0.f;
            //refill player each round
            player.hp           = player.maxHp;
            player.isInvincible = false;
            player.hurt_timer   = 0.f;
            player.currentState = IDLE;
        }
    }

    // Player died — restart current round
    if (player.hp <= 0 && !roundMan.roundOver) {
        player.hp = player.maxHp;
        player.currentState = IDLE;
        player.isInvincible = false;
        player.hurt_timer   = 0.f;
        startRound(roundMan.currentRound);
    }

    // Break between rounds
    if (roundMan.inBreak) {
        roundMan.breakTimer += dt;
        if (roundMan.breakTimer >= roundMan.breakDuration) {
            startRound(roundMan.currentRound + 1);
        }
    }
}
void checkBossPlayerCollision() {
    if (!boss.isActive || !boss.isAlive) return;

    float bossRadius   = 20.f * boss.scale;
    float playerRadius = 20.f;
    float minDist      = bossRadius + playerRadius;

    sf::Vector2f diff = player.pos - boss.pos;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    if (dist < minDist && dist > 0.f) {
        sf::Vector2f pushDir = diff / dist;
        float overlap = minDist - dist;

        // Push player away by half
        player.pos += pushDir * (overlap * 0.5f);
        player.sprite.setPosition(player.pos);

        // Push boss away by half — stops it pinning player to walls
        boss.pos -= pushDir * (overlap * 0.5f);
        boss.sprite.setPosition(boss.pos);
    }
}