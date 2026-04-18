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


    sf::Text promptText;
    sf::Text inputText;
    sf::RectangleShape overlay;
    sf::Font terminalFont;
};

// Functions عادية بتشتغل على الـ struct ده
void initBinaryGame(BinaryGameData& data, sf::Font& font);
void handleBinaryInput(BinaryGameData& data, sf::Event& event);
void updateBinaryGame(BinaryGameData& data);
void drawBinaryGame(sf::RenderWindow& window, BinaryGameData& data);


#endif //LASTECHO_BTDMINIGAME_H