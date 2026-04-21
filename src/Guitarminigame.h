#ifndef GUITAR_MINIGAME_H
#define GUITAR_MINIGAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

#define MAX_STRINGS 6
#define MAX_FRETS 8
#define SCREEN_W 1280
#define SCREEN_H 720

enum GuitarMode { GUITAR_FREE, GUITAR_QUEST };

struct GuitarNote {
    int stringNum;
    int fretNum;
};

struct FretButton {
    sf::RectangleShape shape;
    sf::Text buttonText;
    sf::FloatRect bounds;
    int stringNum;
    int fretNum;
    bool isPressed;
};

struct UIButton {
    sf::RectangleShape shape;
    sf::Text text;
    sf::FloatRect bounds;
    std::string label;
    bool isHovering;
};

struct GuitarGame {
    bool isOpen;
    GuitarMode mode;
    sf::Texture guitarTexture;
    sf::Sprite guitarSprite;
    sf::Font uiFont;

    FretButton frets[MAX_STRINGS][MAX_FRETS];

    // Sound system
    sf::SoundBuffer noteBuffers[MAX_STRINGS][MAX_FRETS];
    sf::Sound currentSound;

    // UI elements
    sf::Text modeText;              // Top left
    sf::Text scoreText;             // Top right
    sf::Text instructionText;       // Bottom
    sf::Text notesPlayedText;       // Bottom left
    sf::Text timerText;             // Bottom center

    // UI Buttons
    UIButton modeButton;            // Switch mode button (top right)
    UIButton exitButton;            // Exit button (top right, below mode)

    // State
    std::string lastNoteName;
    bool questActive;
    float questTimer;
    int targetSequenceLength;
    int notesPlayedCorrect;
    GuitarNote targetSequence[100];
    float feedbackTimer;
};

extern GuitarGame g_guitar;

void initGuitar();
void loadGuitarAssets();
void setupFretButtons();
void setupUIButtons();
void drawGuitar(sf::RenderWindow& window);
void updateGuitar(float deltaTime);
void handleGuitarClick(sf::Vector2f mousePos);
void handleButtonClick(sf::Vector2f mousePos);
void playGuitarNote(int stringNum, int fretNum);
bool isNoteCorrect(int stringNum, int fretNum);
void openGuitarFreePlay();
void openGuitarQuest(const GuitarNote* notes, int noteCount, float timeLimit);
bool isGuitarOpen();
void closeGuitar();
void scaleGuitarToFillScreen(sf::RenderWindow& window);

#endif