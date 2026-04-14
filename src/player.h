#pragma once
#include <SFML/Graphics.hpp>
#include "world.h"



enum Direction { SOUTH = 0, NORTH = 1, WEST = 2, EAST = 3 };
enum playerState { IDLE, WALKING  , HURT , ATTACKING , DEAD};

struct Player {
    sf::Vector2f pos;
    sf::Vector2f velocity;
    float speed;
    sf::Sprite sprite;

    // البيانات الأساسية
    int hp=100, maxHp=100, xp=0, level=1;

    // الأنميشن (الصور والمنطق)
    sf::Texture walkTextures[4]; // شلنا الـ Idle عشان نخفف الرام
    sf::Texture attackTextures[4];
    Direction   facing;
    playerState currentState;
    int   currentFrame;
    float animationTimer;
    bool  isMoving;
    int xpToNextLevel = 100;

    // المقاسات الجديدة اللي اتفقنا عليها (48x48)
    int frameWidth = 48;
    int frameHeight = 48;

    // Combat Logic
    int   attack_damage = 10;
    float cooldown_timer;
    float cooldown_maxtime = 0.5f;
    float attack_range = 50.f;
    float hurt_timer = 0.4f;
    bool  isInvincible = false;
};




// ==============================
// FUNCTION PROTOTYPES
// ==============================
void initPlayer(sf::Vector2f startPos);
void updatePlayer(float dt, World& world);  
void handlingAttack(float dt);
void handlingHurt(float dt);
sf::FloatRect attackHitBox();
void drawPlayer(sf::RenderWindow& window);


extern Player   player;