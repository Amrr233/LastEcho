#pragma once
#include <SFML/Graphics.hpp>
#include "Data.h"

// ==============================
// FUNCTION PROTOTYPES
// ==============================
void initPlayer(sf::Vector2f startPos);
void updatePlayer(float dt);
void drawPlayer(sf::RenderWindow& window);
