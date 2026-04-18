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

    // Load texture — change this path when you have a real sprite
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
    boss.maxHp        = bossConfig.maxHp;  // ← add this
    boss.isAlive      = true;
    boss.isActive     = true;
    boss.currentPhase = BOSS_PHASE_1;
    boss.currentState = BOSS_IDLE;         // ← reset state
    boss.isInvincible = false;             // ← reset invincibility
    boss.hurtTimer    = 0.f;              // ← reset hurt timer
    boss.attackTimer  = 0.f;             // ← reset attack timer
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

    // 1. نادي دالة التصادم هنا أول حاجة
    checkBossPlayerCollision();

    if (boss.currentState == BOSS_HURT) {
        boss.hurtTimer -= dt;
        if (boss.hurtTimer <= 0.f) {
            boss.isInvincible = false;
            boss.currentState = BOSS_IDLE;
        }
        return;
    }

    checkBossPhase();

    sf::Vector2f dir = player.pos - boss.pos;
    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

    if (dist < boss.detectionRange) {
        // بنجيب حدود البوس واللاعب الفعلية
        sf::FloatRect bossBounds = boss.sprite.getGlobalBounds();
        sf::FloatRect playerBounds = player.sprite.getGlobalBounds();

        // لو مش لامسين بعض (مع ترك مسافة أمان صغيرة للهجوم)
        if (dist > (bossBounds.width / 2.0f) + 5.f) {
            boss.currentState = BOSS_WALKING;
            dir /= dist;
            boss.pos += dir * boss.speed * dt;
        }
        else {
            // هنا اللحظة اللي البوس لمس فيها اللاعب
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

    // تحديث مكان السبرايت والأنيميشن
    boss.animTimer += dt;
    if (boss.animTimer >= 0.15f) {
        boss.animTimer = 0.f;
        boss.currentFrame = (boss.currentFrame + 1) % 6;
    }
    boss.sprite.setTextureRect(sf::IntRect(boss.currentFrame * 68, 0, 68, 68));
    boss.sprite.setPosition(boss.pos);
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

    // Each round boss gets stronger — just change these values
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
        }
    }

    // Player died — restart current round
    if (player.hp <= 0 && !roundMan.roundOver) {
        player.hp = player.maxHp;
        player.currentState = IDLE;
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
    sf::FloatRect b = boss.sprite.getGlobalBounds();
    sf::FloatRect p = player.sprite.getGlobalBounds();

    if (b.intersects(p)) {
        sf::Vector2f pushDir = player.pos - boss.pos;
        float length = std::sqrt(pushDir.x * pushDir.x + pushDir.y * pushDir.y);
        if (length > 0) {
            pushDir /= length;
            // الدفع لبره حدود البوس تماماً
            float minSafeDist = (b.width / 2.0f) + 15.f;
            player.pos = boss.pos + pushDir * minSafeDist;

            // مهم جداً: حدّث مكان سبرايت اللاعب فوراً بعد الدفع
            player.sprite.setPosition(player.pos);
        }
    }
}