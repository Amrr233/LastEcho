//
// Created by vvolf on 18/04/2026.
//
#include "boss.h"
#include<Data.h>
#include "player.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

#include "healthbar.h"
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

    // Walking textures
    boss.walkTextures[SOUTH].loadFromFile("assets/sprites/scorpion/walking/walkingScorpionSouth.png");
    boss.walkTextures[NORTH].loadFromFile("assets/sprites/scorpion/walking/walkingScorpionNorth.png");
    boss.walkTextures[WEST].loadFromFile("assets/sprites/scorpion/walking/walkingScorpionWest.png");
    boss.walkTextures[EAST].loadFromFile("assets/sprites/scorpion/walking/walkingScorpionEast.png");

    // Running textures
    boss.runTextures[SOUTH].loadFromFile("assets/sprites/scorpion/running/runningScorpionSouth.png");
    boss.runTextures[NORTH].loadFromFile("assets/sprites/scorpion/running/runningScorpionNorth.png");
    boss.runTextures[WEST].loadFromFile("assets/sprites/scorpion/running/runningScorpionWest.png");
    boss.runTextures[EAST].loadFromFile("assets/sprites/scorpion/running/runningScorpionEast.png");

    //cross punching
    boss.crossPunchingTexture[SOUTH].loadFromFile("assets/sprites/scorpion/crossPunching/crossPunchingScorpionSouth.png");
    boss.crossPunchingTexture[NORTH].loadFromFile("assets/sprites/scorpion/crossPunching/crossPunchingScorpionNorth.png");
    boss.crossPunchingTexture[WEST].loadFromFile("assets/sprites/scorpion/crossPunching/crossPunchingScorpionWest.png");
    boss.crossPunchingTexture[EAST].loadFromFile("assets/sprites/scorpion/crossPunching/crossPunchingScorpionEast.png");

    // Fireball throw textures
    boss.fireballThrowTextures[SOUTH].loadFromFile("assets/sprites/scorpion/throwingFireBalls/throwingFireBallScorpionSouth.png");
    boss.fireballThrowTextures[NORTH].loadFromFile("assets/sprites/scorpion/throwingFireBalls/throwingFireBallScorpionNorth.png");
    boss.fireballThrowTextures[WEST].loadFromFile("assets/sprites/scorpion/throwingFireBalls/throwingFireBallScorpionWest.png");
    boss.fireballThrowTextures[EAST].loadFromFile("assets/sprites/scorpion/throwingFireBalls/throwingFireBallScorpionEast.png");

    // Fireball projectile — single frame
    boss.fireballTexture.loadFromFile("assets/sprites/scorpion/fireball.png");
    //DEBUGGING
    std::cout << "Fireball texture size: "
          << boss.fireballTexture.getSize().x << "x"
          << boss.fireballTexture.getSize().y << std::endl;

    boss.sprite.setTexture(boss.walkTextures[SOUTH]);
    boss.sprite.setOrigin(34.f, 34.f);
    //DEBUGGING
    if (!boss.runTextures[SOUTH].loadFromFile("assets/sprites/scorpion/running/runningScorpionSouth.png"))
        std::cout << "FAILED: runSouth" << std::endl;
    if (!boss.runTextures[NORTH].loadFromFile("assets/sprites/scorpion/running/runningScorpionNorth.png"))
        std::cout << "FAILED: runNorth" << std::endl;
    if (!boss.runTextures[WEST].loadFromFile("assets/sprites/scorpion/running/runningScorpionWest.png"))
        std::cout << "FAILED: runWest" << std::endl;
    if (!boss.runTextures[EAST].loadFromFile("assets/sprites/scorpion/running/runningScorpionEast.png"))
        std::cout << "FAILED: runEast" << std::endl;
    if (!boss.fireballTexture.loadFromFile("assets/sprites/scorpion/fireball.png"))
        std::cout << "FAILED: fireball texture" << std::endl;
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

Direction getBossDirection() {
    sf::Vector2f diff = player.pos - boss.pos;
    if (std::abs(diff.x) > std::abs(diff.y))
        return diff.x > 0 ? EAST : WEST;
    else
        return diff.y > 0 ? SOUTH : NORTH;
}
void updateBoss(float dt) {
    if (!boss.isActive || !boss.isAlive) return;

    // Update sprite position FIRST
    boss.sprite.setPosition(boss.pos);

    checkBossPlayerCollision();

    // Invincibility timer
    if (boss.isInvincible) {
        boss.hurtTimer -= dt;
        if (boss.hurtTimer <= 0.f) {
            boss.isInvincible = false;
            boss.currentState = BOSS_IDLE;
        }
        boss.sprite.setPosition(boss.pos);
        goto animate;
    }

    checkBossPhase();

    {
        sf::Vector2f dir = player.pos - boss.pos;
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        Direction bossDir = getBossDirection();

        // Fireball logic — only Round 2 and 3
        bool canThrowFireball = (roundMan.currentRound >= 2)
                             && (dist > boss.fireballMinRange)
                             && (dist < boss.fireballThrowRange);

        if (!boss.isThrowing && canThrowFireball) {
            boss.fireballTimer += dt;
            if (boss.fireballTimer >= boss.fireballCooldown) {
                boss.fireballTimer  = 0.f;
                boss.isThrowing     = true;
                boss.throwAnimFrame = 0;
                boss.throwAnimTimer = 0.f;
                int count = (roundMan.currentRound == 3) ? 2 : 1;
                spawnFireball(count);
            }
        }

        // Throw animation
        if (boss.isThrowing) {
            boss.throwAnimTimer += dt;
            boss.sprite.setTexture(boss.fireballThrowTextures[bossDir]);
            if (boss.throwAnimTimer >= 0.12f) {
                boss.throwAnimTimer = 0.f;
                boss.throwAnimFrame++;
                if (boss.throwAnimFrame >= 6) {
                    boss.isThrowing     = false;
                    boss.throwAnimFrame = 0;
                }
            }
            boss.sprite.setTextureRect(
                sf::IntRect(boss.throwAnimFrame * 68, 0, 68, 68));
        }
        // Normal movement when not throwing
        else if (dist < boss.detectionRange) {
            if (dist > boss.attackRange + 10.f ) {
                // Round 3 uses run texture
                if (roundMan.currentRound == 3)
                    boss.sprite.setTexture(boss.runTextures[bossDir]);
                else
                    boss.sprite.setTexture(boss.walkTextures[bossDir]);

                boss.currentState = BOSS_WALKING;
                dir /= dist;
                boss.pos += dir * boss.speed * dt;

                // Walk animation
                boss.animTimer += dt;
                if (boss.animTimer >= 0.12f) {
                    boss.animTimer    = 0.f;
                    boss.currentFrame = (boss.currentFrame + 1) % 6;
                }
                boss.sprite.setTextureRect(
                    sf::IntRect(boss.currentFrame * 68, 0, 68, 68));
            }
            else {
                // Melee attack
                boss.currentState  = BOSS_ATTACKING;
                boss.attackTimer  += dt;
                if (boss.attackTimer >= boss.attackCooldown) {
                    boss.attackTimer = 0.f;
                    // Use dist only — no directional bias
                    if (dist <= boss.attackRange + 20.f && !player.isInvincible) {
                        damaging(boss.damage);
                        player.currentState = HURT;
                        player.hurt_timer   = 0.4f;
                        player.isInvincible = true;
                    }
                }
            }
        }
        else {
            boss.currentState = BOSS_IDLE;
            boss.sprite.setTexture(boss.walkTextures[bossDir]);
            boss.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));
        }
    }

    animate:
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
void spawnFireball(int count) {

    sf::Vector2f dir = player.pos - boss.pos;
    float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
    if (len > 0) dir /= len;

    int spawned = 0;
    for (int i = 0; i < MAX_FIREBALLS && spawned < count; i++) {


        boss.fireballs[i].sprite.setTexture(boss.fireballTexture);
        boss.fireballs[i].sprite.setTextureRect(sf::IntRect(0, 0,
            boss.fireballTexture.getSize().x,
            boss.fireballTexture.getSize().y));
        boss.fireballs[i].sprite.setOrigin(
            boss.fireballTexture.getSize().x / 2.f,
            boss.fireballTexture.getSize().y / 2.f);


        if (!boss.fireballs[i].isActive) {
            boss.fireballs[i].isActive  = true;
            boss.fireballs[i].pos       = boss.pos;
            boss.fireballs[i].sprite.setTexture(boss.fireballTexture);
            boss.fireballs[i].sprite.setOrigin(16.f, 16.f);

            if (spawned == 0) {
                // First fireball — straight at player
                boss.fireballs[i].velocity = dir * boss.fireballs[i].speed;
            } else {
                // Second fireball — slight angle offset
                sf::Vector2f offset(-dir.y * 0.3f, dir.x * 0.3f);
                sf::Vector2f v2 = dir + offset;
                float l = std::sqrt(v2.x*v2.x + v2.y*v2.y);
                boss.fireballs[i].velocity = (v2/l) * boss.fireballs[i].speed;
            }
            spawned++;
        }
    }
}

void updateFireballs(float dt) {

    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (!boss.fireballs[i].isActive) continue;
        // TEMP DEBUG
        sf::FloatRect fbBounds(
    boss.fireballs[i].pos.x - 16,
    boss.fireballs[i].pos.y - 16,
    32, 32 );

        sf::FloatRect pbBounds(
            player.pos.x - 24,
            player.pos.y - 24,
            48, 48
        );
        std::cout << "FB pos: " << boss.fireballs[i].pos.x << ","
                  << boss.fireballs[i].pos.y
                  << " Player pos: " << player.pos.x << ","
                  << player.pos.y << std::endl;

        if (fbBounds.intersects(pbBounds)) {
            std::cout << "FIREBALL HIT!" << std::endl;


        if (fbBounds.intersects(pbBounds)) {
            if (!player.isInvincible) {
                player.hp           -= boss.damage;
                damaging(boss.damage);        // trigger health bar update
                player.currentState  = HURT;
                player.hurt_timer    = 0.4f;
                player.isInvincible  = true;
            }
            boss.fireballs[i].isActive = false;
        }

            // Deactivate if too far
            sf::Vector2f d = boss.fireballs[i].pos - boss.pos;
            if (std::sqrt(d.x*d.x + d.y*d.y) > 600.f)
                boss.fireballs[i].isActive = false;
        }
    }
}

void drawFireballs(sf::RenderWindow& window) {
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (boss.fireballs[i].isActive)
            window.draw(boss.fireballs[i].sprite);
    }
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