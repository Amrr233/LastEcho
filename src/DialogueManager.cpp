#include "DialogueManager.h"
#include "Data.h"
#include <iostream>


DialogueManager dialogueSystem;

DialogueManager::DialogueManager() : isOpen(false), totalLines(0), currentLineIdx(0), charIdx(0), typeTimer(0.f) {}

void DialogueManager::init() {
    if (!boxTexture.loadFromFile("assets/gameplay/dialogue.png")) {
        std::cout << "ERROR: Dialogue Box Texture not found!" << std::endl;
    }
    boxSprite.setTexture(boxTexture);
    boxSprite.setScale(1.4f, 1.4f);

    float x = (SCREEN_W / 2.0f) - (boxSprite.getGlobalBounds().width / 2.0f);
    float y = SCREEN_H - boxSprite.getGlobalBounds().height - 20.0f;
    boxSprite.setPosition(x, y);

    if (!font.loadFromFile("assets/fonts/pixelsix00.ttf")) {
        std::cout << "ERROR: Font not found!" << std::endl;
    }

    nameText.setFont(font);
    nameText.setCharacterSize(26);
    nameText.setFillColor(sf::Color::Black);

    dialogueText.setFont(font);
    dialogueText.setCharacterSize(22);
    dialogueText.setFillColor(sf::Color::White);
}

void DialogueManager::centerText() {
    sf::FloatRect nameBounds = nameText.getLocalBounds();
    nameText.setOrigin(nameBounds.left, nameBounds.top);
    nameText.setPosition(910.f, 485.f);

    sf::FloatRect diagBounds = dialogueText.getLocalBounds();
    dialogueText.setOrigin(diagBounds.left, diagBounds.top);
    dialogueText.setPosition(300.f, 320.f);
}

void DialogueManager::startDialogue(std::string name, std::string messages[], int count) {
    isOpen = true;
    nameText.setString(name);
    totalLines = (count > MAX_DIALOGUE_LINES) ? MAX_DIALOGUE_LINES : count;
    currentLineIdx = 0;

    for (int i = 0; i < totalLines; i++) {
        currentMessages[i] = messages[i];
    }

    // تجهيز أول سطر
    fullText = currentMessages[currentLineIdx];
    displayText = "";
    charIdx = 0;
    typeTimer = 0.f;
    dialogueText.setString("");
    centerText();
}

void DialogueManager::nextLine() {
    // Skip Animation: لو لسه بيكتب، اظهر الكلام كله فوراً
    if (charIdx < fullText.length()) {
        charIdx = fullText.length();
        displayText = fullText;
        dialogueText.setString(displayText);
        return;
    }

    currentLineIdx++;
    if (currentLineIdx < totalLines) {
        fullText = currentMessages[currentLineIdx];
        displayText = "";
        charIdx = 0;
        typeTimer = 0.f;
        dialogueText.setString("");
        centerText();
    } else {
        isOpen = false;
    }
}

// 🔥 تنفيذ الدالة بالـ floatdeltaTime
void DialogueManager::update(float deltaTime) {
    if (!isOpen) return;

    if (charIdx < fullText.length()) {
        typeTimer += deltaTime;
        if (typeTimer >= typeSpeed) {
            typeTimer = 0.f;
            displayText += fullText[charIdx];
            charIdx++;
            dialogueText.setString(displayText);
        }
    }
}

void DialogueManager::draw(sf::RenderWindow& window) {
    if (isOpen) {
        window.draw(boxSprite);
        window.draw(nameText);
        window.draw(dialogueText);
    }
}