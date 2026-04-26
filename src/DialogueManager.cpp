#include "DialogueManager.h"
#include "Data.h"
#include <iostream>
#include "NPC.h"
#include "audio.h"

using namespace sf;
using namespace std;

// =======================
// GLOBALS
// =======================
Sprite boxSprite;
Texture boxTexture;

extern Font font;

Text dialogueText;
Text nameText;

Sprite avatarSprite;

bool isOpen = false;

string currentMessages[MAX_DIALOGUE_LINES];
int totalLines = 0;
int currentLineIdx = 0;

string fullText;
string displayText;

float typeTimer = 0.f;
float typeSpeed = 0.03f;

int charIdx = 0;
float maxWidth = 500.f;


// =======================
// FIT SPRITE INSIDE BOX
// =======================
void fitSpriteToBox(sf::Sprite& sprite, sf::Vector2f boxSize) {
    sf::FloatRect bounds = sprite.getLocalBounds();

    float scaleX = boxSize.x / bounds.width;
    float scaleY = boxSize.y / bounds.height;

    float scale = std::min(scaleX, scaleY);

    sprite.setScale(scale, scale);

    bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}


// =======================
// TEXT WRAP
// =======================
string wrapTextSimple(const string& text) {

    string word = "";
    string line = "";
    string result = "";

    for (int i = 0; i < text.size(); i++) {

        if (text[i] != ' ')
            word += text[i];

        if (text[i] == ' ' || i == text.size() - 1) {

            string testLine = line + word + " ";

            dialogueText.setString(testLine);

            if (dialogueText.getGlobalBounds().width > maxWidth) {
                result += line + "\n";
                line = word + " ";
            }
            else {
                line = testLine;
            }

            word = "";
        }
    }

    return result + line;
}


// =======================
// CENTER TEXT
// =======================
void centerText() {

    FloatRect nameBounds = nameText.getLocalBounds();
    nameText.setOrigin(nameBounds.left, nameBounds.top);
    nameText.setPosition(910.f, 485.f);

    FloatRect diagBounds = dialogueText.getLocalBounds();
    dialogueText.setOrigin(diagBounds.left, diagBounds.top);
    dialogueText.setPosition(300.f, 320.f);
}


// =======================
// INIT
// =======================
void initDialogue() {

    if (!boxTexture.loadFromFile("assets/gameplay/dialogue.png")) {
        cout << "ERROR: Dialogue Box Texture not found!" << endl;
    }

    boxSprite.setTexture(boxTexture);
    boxSprite.setScale(1.4f, 1.4f);

    float x = (SCREEN_W / 2.0f) - (boxSprite.getGlobalBounds().width / 2.0f);
    float y = SCREEN_H - boxSprite.getGlobalBounds().height - 20.0f;

    boxSprite.setPosition(x, y);

    if (!font.loadFromFile("assets/fonts/pixelsix00.ttf")) {
        cout << "ERROR: Font not found!" << endl;
    }

    nameText.setFont(font);
    nameText.setCharacterSize(26);
    nameText.setFillColor(Color::Black);

    dialogueText.setFont(font);
    dialogueText.setCharacterSize(22);
    dialogueText.setFillColor(Color::White);
}


// =======================
// START DIALOGUE (FIXED)
// =======================
void startDialogue(string name, string messages[], int count, sf::Texture& avatarTexture) {

    isOpen = true;
    nameText.setString(name);

    avatarSprite.setTexture(avatarTexture, true);

    float boxX = (SCREEN_W / 2.0f) - (boxSprite.getGlobalBounds().width / 2.0f);
    float boxY = SCREEN_H - boxSprite.getGlobalBounds().height - 20.0f;

    avatarSprite.setPosition(boxX + 758.f, boxY + 167.f);

    sf::Vector2f avatarBoxSize(120.f, 120.f);
    fitSpriteToBox(avatarSprite, avatarBoxSize);

    totalLines = std::min(count, MAX_DIALOGUE_LINES);
    currentLineIdx = 0;

    for (int i = 0; i < totalLines; i++) {
        currentMessages[i] = messages[i];
    }

    fullText = currentMessages[currentLineIdx];
    displayText = "";
    charIdx = 0;
    typeTimer = 0.f;

    dialogueText.setString("");

    centerText();
}


// =======================
// NEXT LINE
// =======================
void nextLine() {

    if (charIdx < (int)fullText.length()) {

        charIdx = fullText.length();
        displayText = fullText;

        dialogueText.setString(wrapTextSimple(displayText));
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
    }
    else {
        isOpen = false;
    }
}


// =======================
// UPDATE
// =======================
void updateDialogue(float deltaTime) {

    if (!isOpen) return;

    if (charIdx < (int)fullText.length()) {

        typeTimer += deltaTime;

        if (typeTimer >= typeSpeed) {

            typeTimer = 0.f;

            displayText += fullText[charIdx];
            charIdx++;

            dialogueText.setString(wrapTextSimple(displayText));

            audioManager.playTypeSound();
        }
    }
}


// =======================
// DRAW
// =======================
void drawDialogue(RenderWindow& window) {

    if (!isOpen) return;

    window.draw(boxSprite);
    window.draw(nameText);
    window.draw(dialogueText);
    window.draw(avatarSprite);
}


// =======================
// STATE
// =======================
bool isDialogueActive() {
    return isOpen;
}