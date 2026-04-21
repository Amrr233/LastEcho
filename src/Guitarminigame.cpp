#include "GuitarMinigame.h"
#include <iostream>

// تعريف المتغير (الـ Definition اللي كان ناقص)
GuitarGame g_guitar;

void initGuitar() {
    g_guitar.isOpen = false;
    g_guitar.mode = GUITAR_FREE;
    g_guitar.questActive = false;
    g_guitar.feedbackTimer = 0.0f;
    loadGuitarAssets();
}

void loadGuitarAssets() {
    // تحميل الصور والخطوط
    g_guitar.guitarTexture.loadFromFile("assets/guitarMiniGame/guitar.png");
    g_guitar.guitarSprite.setTexture(g_guitar.guitarTexture);
    g_guitar.uiFont.loadFromFile("assets/fonts/pixelsix00.ttf");

    // تحميل الـ 48 ملف صوتي (نظام وتر-فريت)
    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            std::string fileName = "assets/guitarMiniGame/sounds/" + std::to_string(s + 1) + "-" + std::to_string(f) + ".wav";
            if (!g_guitar.noteBuffers[s][f].loadFromFile(fileName)) {
                std::cout << "Missing Sound File: " << fileName << std::endl;
            }
        }
    }

    // إعداد نصوص الواجهة
    g_guitar.modeText.setFont(g_guitar.uiFont);
    g_guitar.modeText.setCharacterSize(20);
    g_guitar.modeText.setFillColor(sf::Color::Yellow);

    g_guitar.scoreText.setFont(g_guitar.uiFont);
    g_guitar.scoreText.setCharacterSize(20);

    g_guitar.instructionText.setFont(g_guitar.uiFont);
    g_guitar.instructionText.setString("Press Q to Switch Mode | ESC to Exit");
    g_guitar.instructionText.setCharacterSize(15);
}

void setupFretButtons() {
    sf::Vector2f basePos = g_guitar.guitarSprite.getPosition();

    // إحداثيات الضبط الذهبية اللي استقرينا عليها
    float startX = basePos.x + 290.0f;
    float startY = basePos.y + 330.0f;
    float spacingX = 37.0f;
    float spacingY = 16.5f;
    sf::Vector2f rectSize(20.0f, 12.0f);

    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            FretButton& b = g_guitar.frets[s][f];
            b.stringNum = s;
            b.fretNum = f;
            b.isPressed = false;

            b.shape.setSize(rectSize);
            b.shape.setOrigin(rectSize.x / 2.0f, rectSize.y / 2.0f);
            b.shape.setFillColor(sf::Color(100, 100, 100, 100));

            float posX = startX + (f * spacingX);
            float posY = startY + (s * spacingY);
            b.shape.setPosition(posX, posY);
            b.bounds = b.shape.getGlobalBounds();

            // طباعة رقم الفريت (0-7) على المربع
            b.buttonText.setFont(g_guitar.uiFont);
            b.buttonText.setString(std::to_string(f));
            b.buttonText.setCharacterSize(10);
            b.buttonText.setFillColor(sf::Color::White);

            sf::FloatRect textBounds = b.buttonText.getLocalBounds();
            b.buttonText.setOrigin(textBounds.left + textBounds.width / 2.0f,
                                  textBounds.top + textBounds.height / 2.0f);
            b.buttonText.setPosition(posX, posY);
        }
    }
}

void playGuitarNote(int stringNum, int fretNum) {
    if (stringNum < MAX_STRINGS && fretNum < MAX_FRETS) {
        // نظام الـ Mono: وقف أي نوتة شغالة حالياً قبل بدء الجديدة
        g_guitar.currentSound.stop();
        g_guitar.currentSound.setBuffer(g_guitar.noteBuffers[stringNum][fretNum]);
        g_guitar.currentSound.play();

        std::cout << "Triggered Note: " << (stringNum + 1) << "-" << fretNum << std::endl;
    }
}

void handleGuitarClick(sf::Vector2f mousePos) {
    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            if (g_guitar.frets[s][f].bounds.contains(mousePos)) {
                playGuitarNote(s, f);
                g_guitar.frets[s][f].isPressed = true; // تفعيل الفلاش

                if (g_guitar.mode == GUITAR_QUEST && g_guitar.questActive) {
                    if (isNoteCorrect(s, f)) {
                        g_guitar.notesPlayedCorrect++;
                        if (g_guitar.notesPlayedCorrect >= g_guitar.targetSequenceLength) {
                            g_guitar.questActive = false;
                        }
                    } else {
                        g_guitar.notesPlayedCorrect = 0; // إعادة الباتيرن عند الخطأ
                    }
                }
                return;
            }
        }
    }
}

void updateGuitar(float deltaTime) {
    if (!g_guitar.isOpen) return;

    // تحديث إيفيكت الفلاش (الرجوع للون الرمادي)
    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            if (g_guitar.frets[s][f].isPressed) {
                g_guitar.frets[s][f].shape.setFillColor(sf::Color(255, 255, 255, 180));
                g_guitar.frets[s][f].isPressed = false;
            } else {
                sf::Color current = g_guitar.frets[s][f].shape.getFillColor();
                if (current.r > 100) {
                    g_guitar.frets[s][f].shape.setFillColor(sf::Color(100, 100, 100, 100));
                }
            }
        }
    }

    if (g_guitar.mode == GUITAR_QUEST && g_guitar.questActive) {
        g_guitar.questTimer -= deltaTime;
        if (g_guitar.questTimer <= 0) {
            g_guitar.questActive = false;
        }
    }
}

void drawGuitar(sf::RenderWindow& window) {
    sf::Vector2f gPos = g_guitar.guitarSprite.getPosition();
    window.draw(g_guitar.guitarSprite);

    // رسم المربعات والأرقام
    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            window.draw(g_guitar.frets[s][f].shape);
            window.draw(g_guitar.frets[s][f].buttonText);
        }
    }

    // نصوص الحالة
    g_guitar.modeText.setString(g_guitar.mode == GUITAR_FREE ? "Mode: Free Play" : "Mode: Quest");
    g_guitar.modeText.setPosition(gPos.x + 20, gPos.y + 20);
    window.draw(g_guitar.modeText);

    if (g_guitar.mode == GUITAR_QUEST) {
        g_guitar.scoreText.setString("Correct Notes Played: " + std::to_string(g_guitar.notesPlayedCorrect) + "/" + std::to_string(g_guitar.targetSequenceLength));
        g_guitar.scoreText.setPosition(gPos.x + 600, gPos.y + 20);
        window.draw(g_guitar.scoreText);
    }
}

void centerGuitarUI(sf::RenderWindow& window) {
    sf::Vector2u winSize = window.getSize();
    sf::FloatRect gBounds = g_guitar.guitarSprite.getGlobalBounds();
    g_guitar.guitarSprite.setPosition((winSize.x - gBounds.width) / 2.0f, (winSize.y - gBounds.height) / 2.0f);
    setupFretButtons();
}

bool isNoteCorrect(int s, int f) {
    return (g_guitar.targetSequence[g_guitar.notesPlayedCorrect].stringNum == s &&
            g_guitar.targetSequence[g_guitar.notesPlayedCorrect].fretNum == f);
}

void openGuitarFreePlay() { g_guitar.isOpen = true; g_guitar.mode = GUITAR_FREE; }
void openGuitarQuest(const GuitarNote* n, int c, float t) {
    g_guitar.isOpen = true;
    g_guitar.mode = GUITAR_QUEST;
    g_guitar.questActive = true;
    g_guitar.targetSequenceLength = c;
    g_guitar.questTimer = t;
    g_guitar.notesPlayedCorrect = 0;
    for(int i=0; i<c; i++) g_guitar.targetSequence[i] = n[i];
}
bool isGuitarOpen() { return g_guitar.isOpen; }
void closeGuitar() { g_guitar.isOpen = false; }