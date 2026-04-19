#pragma once
#include <SFML/Graphics.hpp>

struct Projectile {
    sf::CircleShape shape;
    sf::Vector2f dir;
    float speed = 0.f;
    bool active = false;
};

enum LichState { L_IDLE, L_WALK, L_ATTACK };
enum LichDir { L_NORTH, L_SOUTH, L_EAST, L_WEST };

// ===== GLOBAL LICH =====
extern sf::Vector2f lichPos;
extern LichState lichState;
extern LichDir lichDir;

extern sf::Sprite lichSprite;
extern sf::Texture lichIdle[4];
extern sf::Texture lichWalk[4];
extern sf::Texture lichAttack[4];

extern Projectile lichProjectile;

extern float lichAnimTimer;
extern int lichFrame;

// ===== FUNCTIONS =====
void initLich(sf::Vector2f pos);
void updateLich(float dt, sf::Vector2f playerPos, sf::FloatRect playerBounds);
void drawLich(sf::RenderWindow& window);