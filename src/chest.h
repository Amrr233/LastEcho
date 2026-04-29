#ifndef CHEST_H
#define CHEST_H

#include <SFML/Graphics.hpp>
#include <string>

struct Chest {
    sf::Vector2f pos;
    std::string mapName;

    sf::Texture spriteSheet;
    sf::Sprite sprite;

    int totalFrames = 5;
    int frameWidth  = 188;
    int frameHeight = 149;

    int   currentFrame  = 0;
    float animTimer     = 0.f;
    float animSpeed     = 0.1f;   // seconds per frame
    bool  isOpening     = false;  // animation playing
    bool  isOpen        = false;  // fully opened
    bool  isLooted      = false;  // strings already taken
};

void initChest(sf::Vector2f pos, std::string mapName);
void updateChest(float dt, std::string currentMap);
void drawChest(sf::RenderWindow& window, std::string currentMap);
bool tryOpenChest(sf::Vector2f playerPos, std::string currentMap);  // returns true if opened

extern Chest gameChest;

#endif