#ifndef LICH_H
#define LICH_H

#include <SFML/Graphics.hpp>

// تعريف حالات البوس
enum LichState { LICH_IDLE, LICH_ATTACKING, LICH_DYING, LICH_REBORN, LICH_DEAD };

// تعريف هيكل السحر/الطلقات
struct thelichSpells {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float damage;
    bool isActive = false;
};

// تعريف كلاس الليتش
struct Lich {
    sf::Sprite sprite;
    sf::Texture texture;
    LichState currentState;

    float health;
    float maxHealth;
    float attackTimer;

    // المتغيرات اللي الـ Compiler مش شايفها عندك
    bool isInvincible;
    int currentPhase;
    bool alreadyRevived;
    float respawnCooldown;

    // متغيرات الطلقات
    thelichSpells fireballs[4];
    int currentShotsCounter;
    float burstInterval;
    float burstTimer;

    // الدوال الأساسية
    void initial_lich(sf::Vector2f position);
    void update(float deltaTime, sf::Vector2f playerPos);
    void draw(sf::RenderWindow& window);
    void takeDamage(float damageAmount); // التأكد من وجود هذه الدالة
};

#endif