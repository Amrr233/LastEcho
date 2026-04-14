#include "DialogueManager.h"
#include <iostream>
#include <sstream>

// =====================
// GLOBAL DEFINITIONS
// =====================
sf::Sprite boxSprite;
sf::Texture boxTexture;
sf::Font font;
sf::Text dialogueText;
sf::Text nameText;

sf::SoundBuffer typeBuffer;
sf::Sound typeSound;

bool isOpen = false;

std::string currentMessages[MAX_DIALOGUE_LINES];
int totalLines = 0;
int currentLineIdx = 0;

std::string fullText;
std::string displayText;
float typeTimer = 0.f;
float typeSpeed = 0.00005f;
int charIdx = 0;

float soundTimer = 0.f;
float soundDelay = 0.08f;
float maxWidth = 500.f;

// =====================
// INIT
// =====================
void initDialogue() {

    if (!boxTexture.loadFromFile("assets/gameplay/dialogue.png")) {
        std::cout << "ERROR: box texture\n";
    }

    boxSprite.setTexture(boxTexture);
    boxSprite.setScale(1.4f, 1.4f);

    float x = (SCREEN_W / 2.0f) - (boxSprite.getGlobalBounds().width / 2.0f);
    float y = SCREEN_H - boxSprite.getGlobalBounds().height - 20.0f;
    boxSprite.setPosition(x, y);

    if (!font.loadFromFile("assets/fonts/pixelsix00.ttf")) {
        std::cout << "ERROR: font\n";
    }

    nameText.setFont(font);
    nameText.setCharacterSize(26);
    nameText.setFillColor(sf::Color::Black);

    dialogueText.setFont(font);
    dialogueText.setCharacterSize(22);
    dialogueText.setFillColor(sf::Color::White);

    // sound
    if (!typeBuffer.loadFromFile("C:/Users/saged/Downloads/type.mp3")) {
        std::cout << "ERROR: sound\n";
    }

    typeSound.setBuffer(typeBuffer);
    typeSound.setVolume(50.f);
}

// =====================
void centerText() {
    nameText.setPosition(910.f, 485.f);
    dialogueText.setPosition(300.f, 320.f);
}

// =====================
// AUTO WRAP
// =====================
std::string wrapText(const std::string& text) {

    std::stringstream words(text);
    std::string word;
    std::string result;
    std::string line;

    while (words >> word) {

        std::string test = line + word + " ";
        dialogueText.setString(test);

        if (dialogueText.getGlobalBounds().width > maxWidth) {
            result += line + "\n";
            line = word + " ";
        } else {
            line = test;
        }
    }

    result += line;
    return result;
}

// =====================
// START DIALOGUE
// =====================
void startDialogue(std::string name, std::string messages[], int count) {

    isOpen = true;
    nameText.setString(name);

    totalLines = (count > MAX_DIALOGUE_LINES) ? MAX_DIALOGUE_LINES : count;
    currentLineIdx = 0;

    for (int i = 0; i < totalLines; i++)
        currentMessages[i] = messages[i];

    fullText = currentMessages[0];
    displayText = "";
    charIdx = 0;
    typeTimer = 0.f;

    dialogueText.setString("");
    centerText();
}

// =====================
// NEXT LINE
// =====================
void nextLine() {

    if (charIdx < fullText.length()) {
        charIdx = fullText.length();
        displayText = fullText;
        dialogueText.setString(wrapText(displayText));
        return;
    }

    currentLineIdx++;

    if (currentLineIdx < totalLines) {
        fullText = currentMessages[currentLineIdx];
        displayText = "";
        charIdx = 0;
        typeTimer = 0.f;
        soundTimer = 0.f;

        dialogueText.setString("");
        centerText();
    } else {
        isOpen = false;
    }
}

// =====================
// UPDATE
// =====================
void updateDialogue(float deltaTime) {
    if (!isOpen) return;

    // الشرط الأساسي: لسه فيه حروف بتتكتب
    if (charIdx < fullText.length()) {

        typeTimer += deltaTime;
        soundTimer += deltaTime;

        if (typeTimer >= typeSpeed) {
            typeTimer = 0.f;

            displayText += fullText[charIdx];
            charIdx++;

            dialogueText.setString(wrapText(displayText));

            // 🔥 التعديل هنا:
            // بنشغل الصوت "فقط" لو فيه حرف اتكتب وحالياً الـ Timer جاهز
            if (soundTimer >= soundDelay) {
                soundTimer = 0.f;
                // تأكد إننا لسه مخلصناش الجملة عشان الصوت ميكملش فريم زيادة
                if (typeSound.getStatus() != sf::Sound::Playing) {
                    typeSound.play();
                }
            }
        }
    } else {
        // 🔥 أمان زيادة: لو النص خلص، صفر التايمر ووقف الصوت فوراً
        soundTimer = 0.f;
        if (typeSound.getStatus() == sf::Sound::Playing) {
            typeSound.stop();
        }
    }
}

// =====================
// DRAW
// =====================
void drawDialogue(sf::RenderWindow& window) {

    if (!isOpen) return;

    window.draw(boxSprite);
    window.draw(nameText);
    window.draw(dialogueText);
}