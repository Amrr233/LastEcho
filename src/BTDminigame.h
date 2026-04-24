//
// Created by farah on 4/18/2026.
//

#ifndef LASTECHO_BTDMINIGAME_H
#define LASTECHO_BTDMINIGAME_H

#include <SFML/Graphics.hpp>
#include <string>

// مجرد مخزن للبيانات (Variables)
struct BinaryGameData {
    bool active = false;
    bool completed = false;
    std::string targetBinary = "01000011";
    int targetDecimal = 67;
    std::string userInput = "";
    std::string statusMessage = "Enter access key:";
    float displayTimer = 0.0f; // بيحسب الوقت اللي فات
    int messageStep = 0;       // إحنا في أنهي جملة (0 أو 1 أو 2)


    sf::Text promptText;
    sf::Text inputText;
    sf::RectangleShape overlay;
    sf::Font terminalFont;
    sf::Texture terminalTexture;
    sf::Sprite terminalSprite;
};


void initBinaryGame(BinaryGameData& data);
void handleBinaryInput(BinaryGameData& data, sf::Event& event);
void updateBinaryGame(BinaryGameData& data, float deltaTime);
void restartBinaryGame(BinaryGameData& data);
void drawBinaryGame(sf::RenderWindow& window, BinaryGameData& data);


#endif //LASTECHO_BTDMINIGAME_H