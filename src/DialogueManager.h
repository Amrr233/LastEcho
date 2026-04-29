#ifndef DIALOGUE_MANAGER_H
#define DIALOGUE_MANAGER_H
#include <SFML/Graphics.hpp>
#include <string>
#include "Data.h"

// الدوال الأساسية للنظام
void initDialogue();
void startDialogue(std::string name,
                   std::string messages[],
                   int count,
                   sf::Texture& avatarTexture);
void nextLine();
void updateDialogue(float deltaTime);
void drawDialogue(sf::RenderWindow& window);
bool isDialogueActive();

#endif