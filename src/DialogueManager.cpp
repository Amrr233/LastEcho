#include "DialogueManager.h"
#include "Data.h"
#include <iostream>

DialogueManager dialogueSystem;

DialogueManager::DialogueManager() : isOpen(false), totalLines(0), currentLineIdx(0) {}

void DialogueManager::init() {
    if (!boxTexture.loadFromFile("C:/Users/saged/Downloads/dialogue9.png")) {
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

// 🔥 التعديل هنا: تثبيت أول حرف (Left Alignment)
void DialogueManager::centerText() {
    // 1. تثبيت الاسم من الشمال
    // بنخلي الـ Origin هو الركن الشمال (Left & Top) عشان أول حرف م يتحركش
    sf::FloatRect nameBounds = nameText.getLocalBounds();
    nameText.setOrigin(nameBounds.left, nameBounds.top);
    nameText.setPosition(7000.f, 495.f); // أول حرف هيبدأ دايماً هنا

    // 2. تثبيت الحوار من الشمال
    sf::FloatRect diagBounds = dialogueText.getLocalBounds();
    dialogueText.setOrigin(diagBounds.left, diagBounds.top);
    dialogueText.setPosition(300.f, 340.f); // أول حرف هيبدأ دايماً هنا
}

void DialogueManager::startDialogue(std::string name, std::string messages[], int count) {
    isOpen = true;
    nameText.setString(name);
    totalLines = (count > MAX_DIALOGUE_LINES) ? MAX_DIALOGUE_LINES : count;
    currentLineIdx = 0;

    for (int i = 0; i < totalLines; i++) {
        currentMessages[i] = messages[i];
    }

    dialogueText.setString(currentMessages[currentLineIdx]);

    // 🔥 استدعاء الدالة عشان نثبت مكان أول حرف
    centerText();
}

void DialogueManager::nextLine() {
    currentLineIdx++;
    if (currentLineIdx < totalLines) {
        dialogueText.setString(currentMessages[currentLineIdx]);
        // 🔥 إعادة التثبيت مع كل جملة جديدة
        centerText();
    } else {
        isOpen = false;
    }
}

void DialogueManager::update() {}

void DialogueManager::draw(sf::RenderWindow& window) {
    if (isOpen) {
        window.draw(boxSprite);
        window.draw(nameText);
        window.draw(dialogueText);
    }
}