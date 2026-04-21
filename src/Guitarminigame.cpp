#include "GuitarMiniGame.h"
#include <iostream>
#include <cmath>

GuitarGame g_guitar;

// Note frequencies (Hz)
const float noteFrequencies[MAX_STRINGS][MAX_FRETS] = {
    {82.41f, 87.31f, 92.50f, 98.00f, 103.83f, 110.00f, 116.54f, 123.47f},
    {110.00f, 116.54f, 123.47f, 130.81f, 138.59f, 146.83f, 155.56f, 164.81f},
    {146.83f, 155.56f, 164.81f, 174.61f, 185.00f, 196.00f, 207.65f, 220.00f},
    {196.00f, 207.65f, 220.00f, 233.08f, 246.94f, 261.63f, 277.18f, 293.66f},
    {246.94f, 261.63f, 277.18f, 293.66f, 311.13f, 329.63f, 349.23f, 369.99f},
    {329.63f, 349.23f, 369.99f, 392.00f, 415.30f, 440.00f, 466.16f, 493.88f},
};

const std::string noteNames[MAX_STRINGS][MAX_FRETS] = {
    {"E", "F", "F#", "G", "G#", "A", "A#", "B"},
    {"A", "A#", "B", "C", "C#", "D", "D#", "E"},
    {"D", "D#", "E", "F", "F#", "G", "G#", "A"},
    {"G", "G#", "A", "A#", "B", "C", "C#", "D"},
    {"B", "C", "C#", "D", "D#", "E", "F", "F#"},
    {"E", "F", "F#", "G", "G#", "A", "A#", "B"},
};

void initGuitar() {
    g_guitar.isOpen = false;
    g_guitar.mode = GUITAR_FREE;
    g_guitar.questActive = false;
    g_guitar.feedbackTimer = 0.0f;
    g_guitar.questTimer = 0.0f;
    g_guitar.notesPlayedCorrect = 0;
    g_guitar.targetSequenceLength = 0;

    loadGuitarAssets();
}

void loadGuitarAssets() {
    // Load guitar image
    if (!g_guitar.guitarTexture.loadFromFile("assets/guitarMiniGame/guitar.png")) {
        std::cerr << "[ERROR] Failed to load guitar image" << std::endl;
    }
    g_guitar.guitarSprite.setTexture(g_guitar.guitarTexture);

    // Load font
    if (!g_guitar.uiFont.loadFromFile("assets/fonts/pixelsix00.ttf")) {
        std::cerr << "[ERROR] Failed to load font" << std::endl;
    }

    // Load 48 sound files (6 strings × 8 frets)
    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            std::string fileName = "assets/guitarMiniGame/sounds/" +
                                  std::to_string(s + 1) + "-" +
                                  std::to_string(f) + ".wav";
            if (!g_guitar.noteBuffers[s][f].loadFromFile(fileName)) {
                std::cout << "[SOUND] Missing: " << fileName << std::endl;
            }
        }
    }

    // Setup UI text
    g_guitar.modeText.setFont(g_guitar.uiFont);
    g_guitar.modeText.setCharacterSize(20);
    g_guitar.modeText.setFillColor(sf::Color::Yellow);

    g_guitar.scoreText.setFont(g_guitar.uiFont);
    g_guitar.scoreText.setCharacterSize(16);
    g_guitar.scoreText.setFillColor(sf::Color::White);

    g_guitar.notesPlayedText.setFont(g_guitar.uiFont);
    g_guitar.notesPlayedText.setCharacterSize(14);
    g_guitar.notesPlayedText.setFillColor(sf::Color::Green);

    g_guitar.timerText.setFont(g_guitar.uiFont);
    g_guitar.timerText.setCharacterSize(14);
    g_guitar.timerText.setFillColor(sf::Color::Yellow);

    g_guitar.instructionText.setFont(g_guitar.uiFont);
    g_guitar.instructionText.setCharacterSize(13);
    g_guitar.instructionText.setFillColor(sf::Color::Cyan);
    g_guitar.instructionText.setString("Click frets to play");
}

void setupUIButtons() {
    // Mode Switch Button (top right)
    g_guitar.modeButton.shape.setSize({120.f, 40.f});
    g_guitar.modeButton.shape.setFillColor(sf::Color::Blue);
    g_guitar.modeButton.shape.setOutlineThickness(2.f);
    g_guitar.modeButton.shape.setOutlineColor(sf::Color::White);
    g_guitar.modeButton.text.setFont(g_guitar.uiFont);
    g_guitar.modeButton.text.setString("Q: Mode");
    g_guitar.modeButton.text.setCharacterSize(14);
    g_guitar.modeButton.text.setFillColor(sf::Color::White);
    g_guitar.modeButton.label = "mode";
    g_guitar.modeButton.isHovering = false;

    // Exit Button (top right, below mode)
    g_guitar.exitButton.shape.setSize({120.f, 40.f});
    g_guitar.exitButton.shape.setFillColor(sf::Color::Red);
    g_guitar.exitButton.shape.setOutlineThickness(2.f);
    g_guitar.exitButton.shape.setOutlineColor(sf::Color::White);
    g_guitar.exitButton.text.setFont(g_guitar.uiFont);
    g_guitar.exitButton.text.setString("ESC: Exit");
    g_guitar.exitButton.text.setCharacterSize(14);
    g_guitar.exitButton.text.setFillColor(sf::Color::White);
    g_guitar.exitButton.label = "exit";
    g_guitar.exitButton.isHovering = false;
}

void scaleGuitarToFillScreen(sf::RenderWindow& window) {
    // Get window size
    sf::Vector2u winSize = window.getSize();

    // Get actual image texture size
    sf::Vector2f textureSize((float)g_guitar.guitarTexture.getSize().x,
                             (float)g_guitar.guitarTexture.getSize().y);

    // Calculate scale to cover entire screen
    float scaleX = (float)winSize.x / textureSize.x;
    float scaleY = (float)winSize.y / textureSize.y;

    // Use MAX to fill entire screen without gaps
    float scale = std::max(scaleX, scaleY);

    // Reset origin and apply scale
    g_guitar.guitarSprite.setOrigin(0.f, 0.f);
    g_guitar.guitarSprite.setScale(scale, scale);

    // Calculate scaled dimensions
    float scaledWidth = textureSize.x * scale;
    float scaledHeight = textureSize.y * scale;

    // Center the image on screen
    float offsetX = (winSize.x - scaledWidth) / 2.0f;
    float offsetY = (winSize.y - scaledHeight) / 2.0f;

    g_guitar.guitarSprite.setPosition(offsetX, offsetY);

    // Recalculate fret button positions
    setupFretButtons();

    // Recalculate UI button positions
    setupUIButtons();

    // Update button positions based on window size
    g_guitar.modeButton.shape.setPosition(winSize.x - 140.f, 20.f);
    g_guitar.exitButton.shape.setPosition(winSize.x - 140.f, 70.f);

    // Update button text positions
    sf::FloatRect modeBounds = g_guitar.modeButton.text.getLocalBounds();
    g_guitar.modeButton.text.setOrigin(modeBounds.width / 2.f, modeBounds.height / 2.f);
    g_guitar.modeButton.text.setPosition(winSize.x - 80.f, 40.f);

    sf::FloatRect exitBounds = g_guitar.exitButton.text.getLocalBounds();
    g_guitar.exitButton.text.setOrigin(exitBounds.width / 2.f, exitBounds.height / 2.f);
    g_guitar.exitButton.text.setPosition(winSize.x - 80.f, 90.f);

    // Update bounds for click detection
    g_guitar.modeButton.bounds = g_guitar.modeButton.shape.getGlobalBounds();
    g_guitar.exitButton.bounds = g_guitar.exitButton.shape.getGlobalBounds();
}

void setupFretButtons() {
    // Get guitar sprite position and scale
    sf::Vector2f gPos = g_guitar.guitarSprite.getPosition();
    sf::Vector2f gScale = g_guitar.guitarSprite.getScale();

    // Base coordinates (relative to original image)
    float baseStartX = 290.0f;
    float baseStartY = 330.0f;
    float baseSpacingX = 37.0f;
    float baseSpacingY = 16.5f;

    // Apply scale
    float startX = gPos.x + (baseStartX * gScale.x);
    float startY = gPos.y + (baseStartY * gScale.y);
    float spacingX = baseSpacingX * gScale.x;
    float spacingY = baseSpacingY * gScale.y;
    float fretSize = 20.0f * gScale.x;
    int fontSize = (int)(10.0f * gScale.x);

    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            FretButton& b = g_guitar.frets[s][f];
            b.stringNum = s;
            b.fretNum = f;
            b.isPressed = false;

            // Position
            float posX = startX + (f * spacingX);
            float posY = startY + (s * spacingY);

            b.shape.setSize({fretSize, fretSize * 0.6f});
            b.shape.setOrigin(fretSize / 2.0f, fretSize * 0.3f);
            b.shape.setPosition(posX, posY);
            b.shape.setFillColor(sf::Color(100, 100, 100, 100));

            b.bounds = b.shape.getGlobalBounds();

            // Text
            b.buttonText.setFont(g_guitar.uiFont);
            b.buttonText.setString(std::to_string(f));
            b.buttonText.setCharacterSize(fontSize > 5 ? fontSize : 6);
            b.buttonText.setFillColor(sf::Color::White);

            sf::FloatRect textBounds = b.buttonText.getLocalBounds();
            b.buttonText.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
            b.buttonText.setPosition(posX, posY);
        }
    }
}

void openGuitarFreePlay() {
    g_guitar.isOpen = true;
    g_guitar.mode = GUITAR_FREE;
    g_guitar.questActive = false;
    g_guitar.modeText.setString("MODE: FREE PLAY");
    std::cout << "[GUITAR] Free play opened (TAB)" << std::endl;
}

void openGuitarQuest(const GuitarNote* notes, int noteCount, float timeLimit) {
    g_guitar.isOpen = true;
    g_guitar.mode = GUITAR_QUEST;
    g_guitar.questActive = true;
    g_guitar.questTimer = timeLimit;
    g_guitar.notesPlayedCorrect = 0;
    g_guitar.targetSequenceLength = noteCount;

    for (int i = 0; i < noteCount && i < 100; i++) {
        g_guitar.targetSequence[i] = notes[i];
    }

    g_guitar.modeText.setString("MODE: QUEST");
    std::cout << "[GUITAR] Quest opened with " << noteCount << " notes" << std::endl;
}

bool isGuitarOpen() {
    return g_guitar.isOpen;
}

void closeGuitar() {
    g_guitar.isOpen = false;
    g_guitar.questActive = false;
    std::cout << "[GUITAR] Closed (ESC)" << std::endl;
}

void playGuitarNote(int stringNum, int fretNum) {
    if (stringNum < 0 || stringNum >= MAX_STRINGS) return;
    if (fretNum < 0 || fretNum >= MAX_FRETS) return;

    // Stop current sound and play new one
    g_guitar.currentSound.stop();
    g_guitar.currentSound.setBuffer(g_guitar.noteBuffers[stringNum][fretNum]);
    g_guitar.currentSound.play();

    g_guitar.lastNoteName = noteNames[stringNum][fretNum];
    g_guitar.feedbackTimer = 0.6f;

    std::cout << "[NOTE] String " << (stringNum + 1) << " Fret " << fretNum
              << " = " << g_guitar.lastNoteName << std::endl;

    // Highlight fret
    FretButton& button = g_guitar.frets[stringNum][fretNum];
    button.isPressed = true;
    button.shape.setFillColor(sf::Color(255, 255, 255, 180));

    // Track for quest
    if (g_guitar.mode == GUITAR_QUEST && g_guitar.questActive) {
        if (isNoteCorrect(stringNum, fretNum)) {
            g_guitar.notesPlayedCorrect++;
            if (g_guitar.notesPlayedCorrect >= g_guitar.targetSequenceLength) {
                g_guitar.questActive = false;
                std::cout << "[QUEST] COMPLETED!" << std::endl;
            }
        } else {
            g_guitar.notesPlayedCorrect = 0;
        }
    }
}

void handleGuitarClick(sf::Vector2f mousePos) {
    if (!g_guitar.isOpen) return;

    // Check button clicks first
    handleButtonClick(mousePos);

    // Then check fret clicks
    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            if (g_guitar.frets[s][f].bounds.contains(mousePos)) {
                playGuitarNote(s, f);
                return;
            }
        }
    }
}

void handleButtonClick(sf::Vector2f mousePos) {
    // Check mode button
    if (g_guitar.modeButton.bounds.contains(mousePos)) {
        if (g_guitar.mode == GUITAR_FREE) {
            GuitarNote pattern[12] = {
                {0, 7}, {1, 4}, {1, 4}, {0, 7},
                {0, 7}, {1, 4}, {1, 4}, {0, 7},
                {0, 7}, {1, 4}, {1, 5}, {1, 4}
            };
            openGuitarQuest(pattern, 12, 60.0f);
        } else {
            openGuitarFreePlay();
        }
        return;
    }

    // Check exit button
    if (g_guitar.exitButton.bounds.contains(mousePos)) {
        closeGuitar();
        return;
    }
}

void updateGuitar(float deltaTime) {
    if (!g_guitar.isOpen) return;

    // Fade out highlight
    g_guitar.feedbackTimer -= deltaTime;

    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            FretButton& b = g_guitar.frets[s][f];
            if (b.isPressed && g_guitar.feedbackTimer <= 0.f) {
                b.isPressed = false;
                b.shape.setFillColor(sf::Color(100, 100, 100, 100));
            }
        }
    }

    // Update quest timer
    if (g_guitar.mode == GUITAR_QUEST && g_guitar.questActive) {
        g_guitar.questTimer -= deltaTime;
        if (g_guitar.questTimer <= 0.f) {
            g_guitar.questActive = false;
            std::cout << "[QUEST] Time's up!" << std::endl;
        }
    }
}

void drawGuitar(sf::RenderWindow& window) {
    if (!g_guitar.isOpen) return;

    // Draw guitar image (fills screen)
    window.draw(g_guitar.guitarSprite);

    // Draw fret buttons
    for (int s = 0; s < MAX_STRINGS; s++) {
        for (int f = 0; f < MAX_FRETS; f++) {
            window.draw(g_guitar.frets[s][f].shape);
            window.draw(g_guitar.frets[s][f].buttonText);
        }
    }

    // Draw UI at fixed screen positions
    sf::Vector2u winSize = window.getSize();

    // Top left: Mode
    g_guitar.modeText.setPosition(20.f, 20.f);
    window.draw(g_guitar.modeText);

    // Top right: Score
    if (g_guitar.mode == GUITAR_QUEST) {
        std::string scoreStr = "Correct: " + std::to_string(g_guitar.notesPlayedCorrect) +
                              "/" + std::to_string(g_guitar.targetSequenceLength);
        g_guitar.scoreText.setString(scoreStr);
        g_guitar.scoreText.setPosition(20.f, 60.f);
        window.draw(g_guitar.scoreText);
    }

    // Bottom left: Feedback
    std::string notesStr = "Feedback: " + g_guitar.lastNoteName;
    g_guitar.notesPlayedText.setString(notesStr);
    g_guitar.notesPlayedText.setPosition(20.f, winSize.y - 50.f);
    window.draw(g_guitar.notesPlayedText);

    // Bottom center: Timer (quest mode)
    if (g_guitar.mode == GUITAR_QUEST && g_guitar.questActive) {
        int timeLeft = (int)g_guitar.questTimer;
        std::string timerStr = "Time: " + std::to_string(timeLeft) + "s";
        g_guitar.timerText.setString(timerStr);
        g_guitar.timerText.setPosition(winSize.x / 2.f - 50.f, winSize.y - 50.f);
        window.draw(g_guitar.timerText);
    }

    // Draw UI Buttons
    window.draw(g_guitar.modeButton.shape);
    window.draw(g_guitar.modeButton.text);
    window.draw(g_guitar.exitButton.shape);
    window.draw(g_guitar.exitButton.text);
}

bool isNoteCorrect(int s, int f) {
    if (g_guitar.notesPlayedCorrect >= g_guitar.targetSequenceLength) {
        return false;
    }
    return (g_guitar.targetSequence[g_guitar.notesPlayedCorrect].stringNum == s &&
            g_guitar.targetSequence[g_guitar.notesPlayedCorrect].fretNum == f);
}