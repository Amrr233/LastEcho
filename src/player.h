#pragma once
#include <SFML/Graphics.hpp>
#include "Data.h"



enum Direction { SOUTH = 0, NORTH = 1, WEST = 2, EAST = 3 };
enum playerState { IDLE, WALKING };

struct Player {
    sf::Vector2f pos;
    sf::Vector2f velocity;
    float speed;

    // البيانات الأساسية
    int hp=100, maxHp=100, xp=0, level=1;

    // الأنميشن (الصور والمنطق)
    sf::Texture walkTextures[4]; // شلنا الـ Idle عشان نخفف الرام
    Direction   facing;
    playerState currentState;
    int   currentFrame;
    float animationTimer;
    bool  isMoving;
    int xpToNextLevel = 100;

    // المقاسات الجديدة اللي اتفقنا عليها (48x48)
    int frameWidth = 48;
    int frameHeight = 48;
};




// ==============================
// FUNCTION PROTOTYPES
// ==============================
void initPlayer(sf::Vector2f startPos);
void updatePlayer(float dt);
void drawPlayer(sf::RenderWindow& window);


extern Player   player;