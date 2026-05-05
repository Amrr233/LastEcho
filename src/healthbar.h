#pragma once
#include <SFML/Graphics.hpp>

void initHealthBar();
void drawHealthBar(sf::RenderWindow& window);
void damaging(int damage);
void healing(int heal);