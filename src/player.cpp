#include "player.h"
#include "GameMap.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include "enemies.h"
using namespace sf;
extern Player player;
extern GameMap myMap;
static Sprite playerSprite;
void initPlayer(Vector2f startPos) {

player.pos = startPos;

player.speed = 140.f; // سرعة متوسطة عشان السلاسة

player.facing = SOUTH;

player.currentFrame = 0;

player.animationTimer = 0.f;

player.isMoving = false;


player.frameWidth = 48;

player.frameHeight = 48;


// حملنا ملفات المشي فقط

player.walkTextures[SOUTH].loadFromFile("assets/sprites/player/walking.south.png");

player.walkTextures[NORTH].loadFromFile("assets/sprites/player/walking.north.png");

player.walkTextures[WEST].loadFromFile("assets/sprites/player/walking.west.png");

player.walkTextures[EAST].loadFromFile("assets/sprites/player/walking.east.png");

// attacking sprites

player.attackTextures[SOUTH].loadFromFile("C:/lastecho/LastEcho/assets/sprites/player/punching/crosspunching.south.png");

player.attackTextures[NORTH].loadFromFile("C:/lastecho/LastEcho/assets/sprites/player/punching/crosspunching.north.png");

player.attackTextures[WEST].loadFromFile("C:/lastecho/LastEcho/assets/sprites/player/punching/crosspunching.west.png");

player.attackTextures[EAST].loadFromFile("C:/lastecho/LastEcho/assets/sprites/player/punching/crosspunching.east.png");


playerSprite.setTexture(player.walkTextures[SOUTH]);

playerSprite.setScale(1.8f, 1.8f); // التكبير عشان الحجم يظبط مع الماب

playerSprite.setOrigin(24.f, 24.f);


player.attack_damage =10;

player.cooldown_timer =0.f;

player.cooldown_maxtime =.5f;

player.attack_range =50.f;

player.hurt_timer =0.f;

player.isInvincible = false;

player.currentState = IDLE;

}



void updatePlayer(float dt) {

Vector2f velocity(0.f, 0.f);


// 1. Input

if (Keyboard::isKeyPressed(Keyboard::W)) velocity.y -= 1;

if (Keyboard::isKeyPressed(Keyboard::S)) velocity.y += 1;

if (Keyboard::isKeyPressed(Keyboard::A)) velocity.x -= 1;

if (Keyboard::isKeyPressed(Keyboard::D)) velocity.x += 1;


// 2. الحركة والاتجاه

if (velocity.x != 0.f || velocity.y != 0.f) {

player.isMoving = true;


if (velocity.x > 0) player.facing = EAST;

else if (velocity.x < 0) player.facing = WEST;

else if (velocity.y > 0) player.facing = SOUTH;

else if (velocity.y < 0) player.facing = NORTH;


float length = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

velocity /= length;

} else {

player.isMoving = false;

}


// 3. الموقع والحدود

Vector2f nextPos = player.pos + (velocity * player.speed * dt);

float mapW = (float)(myMap.width * myMap.tileSize);

float mapH = (float)(myMap.height * myMap.tileSize);


if (nextPos.x > 0 && nextPos.x < mapW) player.pos.x = nextPos.x;

if (nextPos.y > 0 && nextPos.y < mapH) player.pos.y = nextPos.y;


if (player.currentState == ATTACKING) {

playerSprite.setTexture(player.attackTextures[player.facing]);

} else {

playerSprite.setTexture(player.walkTextures[player.facing]);


if (player.isMoving) {

player.animationTimer += dt;

if (player.animationTimer >= 0.1f) { // سرعة الأنميشن (0.1 ثانية للفريم)

player.animationTimer = 0.f;

player.currentFrame = (player.currentFrame + 1) % 6; // الـ 6 فريمات بتوعك

}

} else {

// لو مش بيتحرك، نثبت على الفريم رقم 0 (وضعية الوقوف الطبيعية في شيت المشي)

player.currentFrame = 0;

}


playerSprite.setTextureRect(IntRect(player.currentFrame * 68, 0, 68, 68));

handlingHurt(dt);

if ( player.currentState == HURT) {
    handlingAttack(dt);
}

}

}

void handlingAttack(float dt) {

if (player.currentState == ATTACKING) {

player.animationTimer += dt;

if (player.animationTimer >= 0.12f) {

player.animationTimer = 0.f;

player.currentFrame++;

// damage happens exactly on frame 3

if (player.currentFrame == 3) {

checkAttackHits();

}
if (player.currentFrame >= 6) {

player.currentFrame = 0;

player.currentState = IDLE;

}
}

return;

}

if (player.cooldown_timer > 0.f)

player.cooldown_timer -= dt;


if (Keyboard::isKeyPressed(Keyboard::Space)

&& player.cooldown_timer <= 0.f

&& player.currentState != ATTACKING ) {

player.currentState = ATTACKING;

player.cooldown_timer = player.cooldown_maxtime;

player.currentFrame = 0;

}

}

void handlingHurt(float dt) {

if (player.currentState != HURT) return;

player.hurt_timer -= dt;

if (player.hurt_timer <= 0.f) {

player.isInvincible = false;

player.currentState = IDLE;

}


}


sf::FloatRect attackHitBox () {

float x = player.pos.x;

float y = player.pos.y;

float range = player.attack_range;

float size = 40.f;


switch (player.facing) {

case SOUTH: return {x - size/2, y, size, range};

case NORTH: return {x - size/2, y -range , size, range};

case EAST : return {x, y - size/2, range, size};

case WEST : return {x - range, y - size/2, range, size};

}

return sf::FloatRect(x, y, size, range);

}


void drawPlayer(RenderWindow& window) {

playerSprite.setPosition(player.pos);

window.draw(playerSprite);

} 