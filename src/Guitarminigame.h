#ifndef GUITAR_MINIGAME_H
#define GUITAR_MINIGAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

#define MAX_STRINGS 6
#define MAX_FRETS 8 // 0 1 2 3 4 5 6 7

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

struct GuitarGame {
    bool isOpen;
    GuitarMode mode;
    sf::Texture guitarTexture;
    sf::Sprite guitarSprite;
    sf::Font uiFont;

    FretButton frets[MAX_STRINGS][MAX_FRETS];

    // نظام الصوت (Mono)
    sf::SoundBuffer noteBuffers[MAX_STRINGS][MAX_FRETS];
    sf::Sound currentSound;

    // UI & Logic
    sf::Text modeText;
    sf::Text scoreText;
    sf::Text instructionText;
    std::string lastNoteName;
    bool questActive;
    float questTimer;
    int targetSequenceLength;
    int notesPlayedCorrect;
    GuitarNote targetSequence[100];
    float feedbackTimer;
};

extern GuitarGame g_guitar;

// Functions
void initGuitar();
void loadGuitarAssets();
void setupFretButtons();
void drawGuitar(sf::RenderWindow& window);
void updateGuitar(float deltaTime);
void handleGuitarClick(sf::Vector2f mousePos);
void playGuitarNote(int stringNum, int fretNum);
bool isNoteCorrect(int stringNum, int fretNum);
void openGuitarFreePlay();
void openGuitarQuest(const GuitarNote* notes, int noteCount, float timeLimit);
bool isGuitarOpen();
void closeGuitar();
void centerGuitarUI(sf::RenderWindow& window);

#endif