#ifndef DIALOGUE_MANAGER_H
#define DIALOGUE_MANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include "Data.h"

// =====================
// GLOBAL VARIABLES
// =====================
extern sf::Sprite boxSprite;
extern sf::Texture boxTexture;
extern sf::Font font;
extern sf::Text dialogueText;
extern sf::Text nameText;

extern sf::SoundBuffer typeBuffer;
extern sf::Sound typeSound;

extern bool isOpen;

extern std::string currentMessages[MAX_DIALOGUE_LINES];
extern int totalLines;
extern int currentLineIdx;

// Typewriter
extern std::string fullText;
extern std::string displayText;
extern float typeTimer;
extern float typeSpeed;
extern int charIdx;

// Sound
extern float soundTimer;
extern float soundDelay;

// Auto wrap
extern float maxWidth;

// =====================
// FUNCTIONS
// =====================
void initDialogue();
void startDialogue(std::string name, std::string messages[], int count);
void nextLine();
void updateDialogue(float deltaTime);
void drawDialogue(sf::RenderWindow& window);
std::string wrapText(const std::string& text);
void centerText();

#endif