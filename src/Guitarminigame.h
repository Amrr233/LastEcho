#ifndef GUITAR_MINIGAME_H
#define GUITAR_MINIGAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

#define MAX_STRINGS 6
#define MAX_FRETS 8
#define GUITAR_VM_WIDTH 1200.0f
#define GUITAR_VM_HEIGHT 800.0f

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

// هيكل الزرار المركب (صورة + تيكست فوقها)
struct CompositeButton {
    sf::Sprite sprite;
    sf::Text text;
    sf::FloatRect bounds;
};

struct GuitarGame {
    bool isOpen;
    GuitarMode mode;
    sf::Texture guitarTexture;
    sf::Sprite guitarSprite;
    sf::Font uiFont;
    sf::View guitarView;

    FretButton frets[MAX_STRINGS][MAX_FRETS];
    sf::SoundBuffer noteBuffers[MAX_STRINGS][MAX_FRETS];
    sf::Sound currentSound;

    sf::Text modeText;
    sf::Text scoreText;

    // --- المتغيرات اللي الـ Compiler بيدور عليها ---
    bool questActive;              // هل الكويست شغال؟
    float questTimer;             // الوقت المتبقي
    int targetSequenceLength;     // طول النغمة المطلوبة
    int notesPlayedCorrect;       // إنت عزفت كام نوتة صح
    GuitarNote targetSequence[100]; // مصفوفة النوتات المطلوبة

    // أزرار الصور
    sf::Texture buttonBgTexture;
    CompositeButton exitBtn;
    CompositeButton changeModeBtn;

    // الأنيميشن
    float currentScale;
    bool isAnimating;
};

extern GuitarGame g_guitar;

void initGuitar();
void loadGuitarAssets();
void setupFretButtons();
void drawGuitar(sf::RenderWindow& window);
void updateGuitar(float deltaTime);
void handleGuitarClick(sf::RenderWindow& window, sf::Vector2i mousePixelPos);
void playGuitarNote(int stringNum, int fretNum);
bool isNoteCorrect(int s, int f);
void openGuitarFreePlay();
void openGuitarQuest(const GuitarNote* n, int c, float t);
bool isGuitarOpen();
void closeGuitar();

#endif