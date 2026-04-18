#include "thelich.h"
#include <cmath>
#include <iostream>
#include "player.h"

extern Player player;
extern void damaging(int amount);

void Lich::initial_lich(sf::Vector2f spawnPos) {
    if (!texture.loadFromFile("assets/enemies/lich.png")) {
        std::cerr << "Error: Lich texture missing!" << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
    sprite.setPosition(spawnPos);

    maxHealth = 500.f;
    health = maxHealth;
    currentState = LICH_IDLE; // تم التعديل
    attackTimer = 0.f;
    isInvincible = false;

    currentPhase = 1;
    alreadyRevived = false;
    respawnCooldown = 0.f;

    currentShotsCounter = 0;
    burstInterval = 0.3f;
    burstTimer = 0.f;

    for (int i = 0; i < 4; i++) {
        fireballs[i].isActive = false;
        fireballs[i].shape.setRadius(12.f);
        fireballs[i].shape.setFillColor(sf::Color(255, 69, 0));
        fireballs[i].shape.setOutlineThickness(2.f);
        fireballs[i].shape.setOutlineColor(sf::Color::Yellow);
    }
}

void Lich::takeDamage(float damageAmount) {
    if (isInvincible || currentState == LICH_DYING || currentState == LICH_DEAD) return;

    health -= damageAmount;
    sprite.setColor(sf::Color::Red);
}

void Lich::update(float dt, sf::Vector2f playerPos) {
    if (sprite.getColor() != sf::Color::White && currentState != LICH_DYING) {
        sprite.setColor(sf::Color::White);
    }

    // 1. منطق الموت والبعث
    if (health <= 0) {
        if (!alreadyRevived) {
            currentState = LICH_DYING; // تم التعديل هنا
            isInvincible = true;
            sprite.setColor(sf::Color(100, 100, 100, 150));

            respawnCooldown += dt;
            if (respawnCooldown >= 3.0f) {
                alreadyRevived = true;
                currentPhase = 2;
                health = maxHealth * 1.5f;
                currentState = LICH_REBORN; // تم التعديل هنا
                isInvincible = false;
                sprite.setColor(sf::Color(200, 50, 255));
                sprite.setScale(1.8f, 1.8f);
                std::cout << "The Lich has Resurrected!" << std::endl;
            }
        } else {
            currentState = LICH_DEAD; // تم التعديل هنا
        }
        return;
    }

    // 2. تحديث حركة الطلقات
    for (int i = 0; i < 4; i++) {
        if (fireballs[i].isActive) {
            fireballs[i].shape.move(fireballs[i].velocity * dt);

            sf::FloatRect playerBounds(player.pos.x - 24.f, player.pos.y - 24.f, 48.f, 48.f);
            if (fireballs[i].shape.getGlobalBounds().intersects(playerBounds)) {
                damaging((int)fireballs[i].damage);
                fireballs[i].isActive = false;
            }

            float dist = std::sqrt(std::pow(fireballs[i].shape.getPosition().x - sprite.getPosition().x, 2) +
                                   std::pow(fireballs[i].shape.getPosition().y - sprite.getPosition().y, 2));
            if (dist > 1200.f) fireballs[i].isActive = false;
        }
    }

    // 3. لوجيك الهجوم
    attackTimer += dt;
    float attackCooldown = (currentPhase == 2) ? 1.5f : 3.0f;

    if (attackTimer >= attackCooldown) {
        burstTimer += dt;

        if (burstTimer >= burstInterval && currentShotsCounter < 4) {
            int idx = currentShotsCounter;
            fireballs[idx].isActive = true;
            fireballs[idx].shape.setPosition(sprite.getPosition());
            fireballs[idx].damage = (currentPhase == 2) ? 20.f : 10.f;

            sf::Vector2f dir = playerPos - sprite.getPosition();
            float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
            sf::Vector2f unitDir = (len != 0) ? dir / len : sf::Vector2f(0, 1);

            float projectileSpeed = (currentPhase == 2) ? 650.f : 450.f;
            fireballs[idx].velocity = unitDir * projectileSpeed;

            currentShotsCounter++;
            burstTimer = 0.f;
        }

        if (currentShotsCounter >= 4) {
            currentShotsCounter = 0;
            attackTimer = 0.f;
        }
    }
}

void Lich::draw(sf::RenderWindow& window) {
    if (currentState != LICH_DEAD) { // تم التعديل هنا
        window.draw(sprite);

        for (int i = 0; i < 4; i++) {
            if (fireballs[i].isActive) {
                window.draw(fireballs[i].shape);
            }
        }
    }
}