#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum LichDir   { L_SOUTH=0, L_NORTH=1, L_WEST=2, L_EAST=3 };
enum LichState { L_IDLE=0,  L_WALK=1,  L_ATTACK=2          };
enum LichPhase { PHASE_1=0, PHASE_2=1                      };

struct LichSpell {
    sf::RectangleShape warningBox;
    sf::Sprite visualSprite;

    float chargeTimer = 0.f, chargeTime = 0.f;
    float warningTimer = 0.f, frozenTimer = 0.f, damageTimer = 0.f;
    bool isCharged = false, showWarning = false, hitboxFrozen = false, isFinished = false;

    int currFrame = 0;
    float animTimer = 0.f;
    float warnAlpha = 50.f;
    bool warnFadeIn = true;
};

struct TheLich {
    sf::Texture idleTex[4], walkTex[4], attackTex[4];
    sf::Texture greenFireballTex; // صورة النار الخضراء
    sf::Sprite  sprite;

    LichDir   currDir = L_SOUTH;
    LichState currState = L_IDLE;
    int       currFrame = 0;
    float     animTimer = 0.f;
    int       frameW = 120, frameH = 120, totalFrames = 4;

    sf::Vector2f position;
    float        speed = 80.f;
    bool         alive = true, isDead = false, isReborn = false;
    float        hp = 300.f, maxHp = 300.f;
    float        attackRange = 180.f, idleRange = 500.f;

    LichPhase phase = PHASE_1;
    float     rebornTimer = 0.f, rebornDuration = 3.f;
    LichSpell spells[4];
};

extern TheLich lich;
void initLich(sf::Vector2f spawnPos);
void updateLich(float dt, sf::Vector2f playerPos, sf::FloatRect playerBounds);
void drawLich(sf::RenderWindow& window);
void lichTakeDamage(float amount);
void resetLichSpells();