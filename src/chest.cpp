#include "Chest.h"
#include "player.h"
#include "inventory.h"
#include "DialogueManager.h"
#include "NPC.h"
#include <cmath>
#include <iostream>

extern inventory inv;
extern Player    player;

Chest gameChest;

void initChest(sf::Vector2f pos, std::string mapName) {
    gameChest.pos     = pos;
    gameChest.mapName = mapName;

    if (!gameChest.spriteSheet.loadFromFile("assets/sprites/items/chest.png"))
        std::cout << "ERROR: chest.png not found!\n";

    gameChest.sprite.setTexture(gameChest.spriteSheet);
    gameChest.sprite.setTextureRect(sf::IntRect(0, 0, 188, 149));
    gameChest.sprite.setPosition(pos);
    gameChest.sprite.setScale(0.5f, 0.5f);

    gameChest.currentFrame = 0;
    gameChest.isOpening    = false;
    gameChest.isOpen       = false;
    gameChest.isLooted     = false;
}

void updateChest(float dt, std::string currentMap) {
    if (gameChest.mapName != currentMap) return;
    if (!gameChest.isOpening || gameChest.isOpen) return;

    gameChest.animTimer += dt;
    if (gameChest.animTimer >= gameChest.animSpeed) {
        gameChest.animTimer = 0.f;
        gameChest.currentFrame++;

        if (gameChest.currentFrame >= gameChest.totalFrames) {
            gameChest.currentFrame = gameChest.totalFrames - 1;
            gameChest.isOpen       = true;
            gameChest.isOpening    = false;

            if (!gameChest.isLooted) {
                gameChest.isLooted = true;
                player.stringsCollected += 3;

                inv.addItem("guitar_string", "assets/sprites/items/guitar_string.png");
                inv.addItem("guitar_string", "assets/sprites/items/guitar_string.png");
                inv.addItem("guitar_string", "assets/sprites/items/guitar_string.png");

                // trigger pickup effect like the key
                inv.triggerPickupEffect("assets/sprites/items/guitar_string.png");

                std::string lines[] = {
                    "You found 3 guitar strings!",
                    "Keep searching for the rest..."
                };
                startDialogue("Chest", lines, 2, gameChest.spriteSheet);
            }
        }

        gameChest.sprite.setTextureRect(
            sf::IntRect(gameChest.currentFrame * 188, 0, 188, 149)
        );
        gameChest.sprite.setPosition(gameChest.pos); // prevents drifting
    }
}

void drawChest(sf::RenderWindow& window, std::string currentMap) {
    if (gameChest.mapName != currentMap) return;
    window.draw(gameChest.sprite);
}

bool tryOpenChest(sf::Vector2f playerPos, std::string currentMap) {
    if (gameChest.mapName != currentMap)         return false;
    if (gameChest.isOpen || gameChest.isOpening) return false;

    // distance check
    float dist = std::sqrt(
        std::pow(playerPos.x - gameChest.pos.x, 2) +
        std::pow(playerPos.y - gameChest.pos.y, 2)
    );
    if (dist > 80.f) return false;

    // player must have key selected in inventory
    int slot = inv.selectedSlot;
    if (!inv.hasItem[slot] || inv.itemNames[slot] != "key") {
        std::string noKey[] = {
            "The chest is locked.",
            "Select the key in your inventory first."
        };
        startDialogue("Chest", noKey, 2, gameChest.spriteSheet);
        return false;
    }

    // consume the key
    inv.hasItem[slot]      = false;
    inv.itemNames[slot]    = "";
    inv.itemQuantity[slot] = 0;

    // start opening animation
    gameChest.isOpening    = true;
    gameChest.currentFrame = 0;
    gameChest.animTimer    = 0.f;
    return true;
}