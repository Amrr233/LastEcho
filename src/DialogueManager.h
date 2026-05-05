#ifndef DIALOGUE_MANAGER_H
#define DIALOGUE_MANAGER_H
#include <SFML/Graphics.hpp>
#include <string>

<<<<<<< HEAD
=======

>>>>>>> ed506049a4b519d1ddad2caa3a10bfc20004137b
void initDialogue();
void startDialogue(std::string name,std::string messages[],int count,sf::Texture& avatarTexture);
void nextLine();
void updateDialogue(float deltaTime);
void drawDialogue(sf::RenderWindow& window);
bool isDialogueActive();

#endif