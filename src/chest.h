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
    float animSpeed     = 0.1f;
    bool  isOpening     = false;
    bool  isOpen        = false;
    bool  isLooted      = false;
};

void initChest(sf::Vector2f pos, std::string mapName);
void updateChest(float dt, std::string currentMap);
void drawChest(sf::RenderWindow& window, std::string currentMap);
bool tryOpenChest(sf::Vector2f playerPos, std::string currentMap);

extern Chest gameChest;

#endif