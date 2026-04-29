#include "GuitarMinigame.h"
#include <iostream>
#include <iomanip> // عشان نظبط شكل التايمر (00:00)
#include <sstream>

GuitarGame g_guitar;

void initGuitar() {
    g_guitar.isOpen = false;
    g_guitar.isAnimating = false;
    g_guitar.currentScale = 0.0f;
    g_guitar.guitarView.setSize(GUITAR_VM_WIDTH, GUITAR_VM_HEIGHT);
    g_guitar.guitarView.setCenter(GUITAR_VM_WIDTH / 2.0f, GUITAR_VM_HEIGHT / 2.0f);
    loadGuitarAssets();
    setupFretButtons();
}

void loadGuitarAssets() {
    if (!g_guitar.guitarTexture.loadFromFile("assets/guitarMiniGame/guitar.png"))
        std::cout << "Guitar texture missing!\n";
    g_guitar.guitarSprite.setTexture(g_guitar.guitarTexture);
    sf::FloatRect gBounds = g_guitar.guitarSprite.getLocalBounds();
    g_guitar.guitarSprite.setOrigin(gBounds.width / 2.0f, gBounds.height / 2.0f);
    g_guitar.guitarSprite.setPosition(GUITAR_VM_WIDTH / 2.0f, GUITAR_VM_HEIGHT / 2.0f);

    if (!g_guitar.uiFont.loadFromFile("assets/fonts/pixelsix00.ttf"))
        std::cout << "Font missing!\n";

    if (!g_guitar.buttonBgTexture.loadFromFile("assets/mainMenu/button.png"))
        std::cout << "Button background texture missing!\n";

    float targetBtnScaleX = 0.25f;
    float targetBtnScaleY = 0.25f;

    // 1. EXIT BUTTON
    g_guitar.exitBtn.sprite.setTexture(g_guitar.buttonBgTexture);
    g_guitar.exitBtn.sprite.setScale(targetBtnScaleX, targetBtnScaleY);
    sf::FloatRect originalExitBounds = g_guitar.exitBtn.sprite.getLocalBounds();
    g_guitar.exitBtn.sprite.setOrigin(originalExitBounds.width / 2.0f, originalExitBounds.height / 2.0f);
    g_guitar.exitBtn.sprite.setPosition(1080.0f, 60.0f);

    g_guitar.exitBtn.text.setFont(g_guitar.uiFont);
    g_guitar.exitBtn.text.setString("EXIT");
    g_guitar.exitBtn.text.setCharacterSize(16);
    g_guitar.exitBtn.text.setFillColor(sf::Color::White);

    sf::FloatRect exitTextBounds = g_guitar.exitBtn.text.getLocalBounds();
    g_guitar.exitBtn.text.setOrigin(exitTextBounds.left + exitTextBounds.width / 2.0f, exitTextBounds.top + exitTextBounds.height / 2.0f);
    g_guitar.exitBtn.text.setPosition(g_guitar.exitBtn.sprite.getPosition());
    g_guitar.exitBtn.bounds = g_guitar.exitBtn.sprite.getGlobalBounds();

    // 2. CHANGE MODE BUTTON
    g_guitar.changeModeBtn.sprite.setTexture(g_guitar.buttonBgTexture);
    g_guitar.changeModeBtn.sprite.setScale(targetBtnScaleX, targetBtnScaleY);
    sf::FloatRect originalModeBounds = g_guitar.changeModeBtn.sprite.getLocalBounds();
    g_guitar.changeModeBtn.sprite.setOrigin(originalModeBounds.width / 2.0f, originalModeBounds.height / 2.0f);
    g_guitar.changeModeBtn.sprite.setPosition(880.0f, 60.0f);

    g_guitar.changeModeBtn.text.setFont(g_guitar.uiFont);
    g_guitar.changeModeBtn.text.setString("CHANGE MODE");
    g_guitar.changeModeBtn.text.setCharacterSize(14);
    g_guitar.changeModeBtn.text.setFillColor(sf::Color::White);

    sf::FloatRect modeTextBounds = g_guitar.changeModeBtn.text.getLocalBounds();
    g_guitar.changeModeBtn.text.setOrigin(modeTextBounds.left + modeTextBounds.width / 2.0f, modeTextBounds.top + modeTextBounds.height / 2.0f);
    g_guitar.changeModeBtn.text.setPosition(g_guitar.changeModeBtn.sprite.getPosition());
    g_guitar.changeModeBtn.bounds = g_guitar.changeModeBtn.sprite.getGlobalBounds();

    // Sounds & Generic UI
    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            std::string fileName = "assets/guitarMiniGame/sounds/" + std::to_string(s + 1) + "-" + std::to_string(f) + ".wav";
            g_guitar.noteBuffers[s][f].loadFromFile(fileName);
        }
    }

    g_guitar.modeText.setFont(g_guitar.uiFont);
    g_guitar.modeText.setCharacterSize(30);
    g_guitar.modeText.setFillColor(sf::Color::Yellow);
    g_guitar.modeText.setPosition(50.0f, 40.0f);

    g_guitar.scoreText.setFont(g_guitar.uiFont);
    g_guitar.scoreText.setCharacterSize(25);
    g_guitar.scoreText.setFillColor(sf::Color::Cyan);
    g_guitar.scoreText.setPosition(50.0f, 85.0f); // تحت الـ Mode text بالظبط
}

void setupFretButtons() {
    sf::Vector2f gPos = g_guitar.guitarSprite.getPosition();
    sf::FloatRect gBounds = g_guitar.guitarSprite.getLocalBounds();
    float startX = gPos.x - (gBounds.width / 2.0f) + 250.0f;
    float startY = gPos.y - (gBounds.height / 2.0f) + 320.0f;
    float spacingX = 42.0f; float spacingY = 17.0f;
    sf::Vector2f rectSize(36.0f, 12.0f);

    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            FretButton& b = g_guitar.frets[s][f];
            b.stringNum = s; b.fretNum = f; b.isPressed = false;
            b.shape.setSize(rectSize); b.shape.setOrigin(rectSize.x / 2.0f, rectSize.y / 2.0f);
            b.shape.setFillColor(sf::Color(255, 255, 255, 0)); // شفاف
            float posX = startX + (f * spacingX); float posY = startY + (s * spacingY);
            b.shape.setPosition(posX, posY); b.bounds = b.shape.getGlobalBounds();
            b.buttonText.setFont(g_guitar.uiFont); b.buttonText.setString(std::to_string(f));
            b.buttonText.setCharacterSize(10); b.buttonText.setFillColor(sf::Color::White);
            sf::FloatRect textBounds = b.buttonText.getLocalBounds();
            b.buttonText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
            b.buttonText.setPosition(posX, posY);
        }
    }
}

void handleGuitarClick(sf::RenderWindow& window, sf::Vector2i mousePixelPos) {
    if (!g_guitar.isOpen || g_guitar.isAnimating) return;
    sf::Vector2f mouseViewPos = window.mapPixelToCoords(mousePixelPos, g_guitar.guitarView);

    if (g_guitar.exitBtn.bounds.contains(mouseViewPos)) {
        closeGuitar();
        return;
    }

    if (g_guitar.changeModeBtn.bounds.contains(mouseViewPos)) {
        if (g_guitar.mode == GUITAR_FREE) {
            GuitarNote pattern[12] = {{0, 7}, {1, 4}, {1, 4}, {0, 7}, {0, 7}, {1, 4}, {1, 4}, {0, 7}, {0, 7}, {1, 4}, {1, 5}, {1, 4}};
            openGuitarQuest(pattern, 12, 60.0f);
        } else {
            openGuitarFreePlay();
        }
        return;
    }

    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            if (g_guitar.frets[s][f].bounds.contains(mouseViewPos)) {
                playGuitarNote(s, f);
                g_guitar.frets[s][f].isPressed = true;
                if (g_guitar.mode == GUITAR_QUEST && g_guitar.questActive) {
                    if (isNoteCorrect(s, f)) {
                        g_guitar.notesPlayedCorrect++;
                        if (g_guitar.notesPlayedCorrect >= g_guitar.targetSequenceLength) {
                            g_guitar.questActive = false;
                        }
                    } else {
                        g_guitar.notesPlayedCorrect = 0; // بيصفر لو غلطت
                    }
                }
                return;
            }
        }
    }
}

void updateGuitar(float deltaTime) {
    if (!g_guitar.isOpen) return;

    if (g_guitar.isAnimating) {
        g_guitar.currentScale += 6.0f * deltaTime;
        if (g_guitar.currentScale >= 1.0f) { g_guitar.currentScale = 1.0f; g_guitar.isAnimating = false; }
    }

    // تحديث النصوص بناء على المود الحالي
    if (g_guitar.mode == GUITAR_FREE) {
        g_guitar.modeText.setString("Mode: Free Play");
    } else {
        std::stringstream ss;
        int mins = (int)g_guitar.questTimer / 60;
        int secs = (int)g_guitar.questTimer % 60;

        ss << "Mode: Quest | Time: " << std::setfill('0') << std::setw(2) << mins << ":" << std::setw(2) << secs;
        g_guitar.modeText.setString(ss.str());

        std::string scoreStr = "Notes: " + std::to_string(g_guitar.notesPlayedCorrect) + " / " + std::to_string(g_guitar.targetSequenceLength);
        g_guitar.scoreText.setString(scoreStr);

        if (g_guitar.questActive) {
            g_guitar.questTimer -= deltaTime;
            if (g_guitar.questTimer <= 0) {
                g_guitar.questTimer = 0;
                g_guitar.questActive = false;
            }
        }
    }

    // تأثير الضغطة على الفريتس
    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            if (g_guitar.frets[s][f].isPressed) {
                g_guitar.frets[s][f].shape.setFillColor(sf::Color(255, 255, 255, 150));
                g_guitar.frets[s][f].isPressed = false;
            } else {
                sf::Color c = g_guitar.frets[s][f].shape.getFillColor();
                if (c.a > 0) g_guitar.frets[s][f].shape.setFillColor(sf::Color(255, 255, 255, std::max(0, (int)c.a - 5)));
            }
        }
    }
}

void drawGuitar(sf::RenderWindow& window) {
    if (!g_guitar.isOpen) return;
    sf::View oldView = window.getView();

    float targetWindowScale = 0.65f;
    float animatedScale = targetWindowScale * g_guitar.currentScale;

    sf::Vector2u winSize = window.getSize();
    float winRatio = (float)winSize.x / winSize.y;
    float guitarRatio = GUITAR_VM_WIDTH / GUITAR_VM_HEIGHT;

    float vpW, vpH, vpX, vpY;
    if (winRatio > guitarRatio) {
        vpW = (guitarRatio / winRatio) * animatedScale; vpH = animatedScale;
    } else {
        vpW = animatedScale; vpH = (winRatio / guitarRatio) * animatedScale;
    }
    vpX = (1.0f - vpW) / 2.0f; vpY = (1.0f - vpH) / 2.0f;

    g_guitar.guitarView.setViewport(sf::FloatRect(vpX, vpY, vpW, vpH));
    window.setView(g_guitar.guitarView);

    sf::RectangleShape dim(sf::Vector2f(GUITAR_VM_WIDTH, GUITAR_VM_HEIGHT));
    dim.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)(160 * g_guitar.currentScale)));
    window.draw(dim);

    window.draw(g_guitar.guitarSprite);

    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            window.draw(g_guitar.frets[s][f].shape);
            window.draw(g_guitar.frets[s][f].buttonText);
        }
    }

    window.draw(g_guitar.exitBtn.sprite);
    window.draw(g_guitar.exitBtn.text);
    window.draw(g_guitar.changeModeBtn.sprite);
    window.draw(g_guitar.changeModeBtn.text);

    window.draw(g_guitar.modeText);
    if (g_guitar.mode == GUITAR_QUEST) {
        window.draw(g_guitar.scoreText);
    }

    window.setView(oldView);
}

void playGuitarNote(int stringNum, int fretNum) {
    if (stringNum < MAX_STRINGS && fretNum < MAX_FRETS) {
        g_guitar.currentSound.stop();
        g_guitar.currentSound.setBuffer(g_guitar.noteBuffers[stringNum][fretNum]);
        g_guitar.currentSound.play();
    }
}

bool isNoteCorrect(int s, int f) {
    return (g_guitar.targetSequence[g_guitar.notesPlayedCorrect].stringNum == s &&
            g_guitar.targetSequence[g_guitar.notesPlayedCorrect].fretNum == f);
}

void openGuitarFreePlay() {
    g_guitar.isOpen = true;
    g_guitar.mode = GUITAR_FREE;
    g_guitar.currentScale = 0.1f;
    g_guitar.isAnimating = true;
}

void openGuitarQuest(const GuitarNote* n, int c, float t) {
    g_guitar.isOpen = true;
    g_guitar.mode = GUITAR_QUEST;
    g_guitar.questActive = true;
    g_guitar.targetSequenceLength = c;
    g_guitar.questTimer = t;
    g_guitar.notesPlayedCorrect = 0;
    g_guitar.currentScale = 0.1f;
    g_guitar.isAnimating = true;
    for(int i=0; i<c; i++) g_guitar.targetSequence[i] = n[i];
}

bool isGuitarOpen() { return g_guitar.isOpen; }
void closeGuitar() { g_guitar.isOpen = false; }