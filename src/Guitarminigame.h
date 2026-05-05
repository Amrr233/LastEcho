#pragma once


#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#define MAX_STRINGS 6
#define MAX_FRETS 8
#define GUITAR_VM_WIDTH 1200.0f
#define GUITAR_VM_HEIGHT 800.0f
using namespace sf;

enum GuitarMode { GUITAR_FREE, GUITAR_QUEST };

struct GuitarNote {
    int stringNum;
    int fretNum;
};

struct FretButton {
     RectangleShape shape;
     Text buttonText;
     FloatRect bounds;
    int stringNum;
    int fretNum;
    bool isPressed;
};

// button layout rectangle + text
struct CompositeButton {
     Sprite sprite;
     Text text;
     FloatRect bounds;
};

struct GuitarGame {
    bool isOpen;
    GuitarMode mode;
     Texture guitarTexture;
     Sprite guitarSprite;
     Font uiFont;
     View guitarView;

    FretButton frets[MAX_STRINGS][MAX_FRETS];
    // buffer = place in the memory stores a sound
     SoundBuffer noteBuffers[MAX_STRINGS][MAX_FRETS];
     Sound currentSound;
    
     Text modeText;
     Text scoreText;


    bool questActive;
    float questTimer;
    int targetSequenceLength;
    int notesPlayedCorrect;
    GuitarNote targetSequence[100];

    // أزرار الصور
     Texture buttonBgTexture;
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
void drawGuitar( RenderWindow& window);
void updateGuitar(float deltaTime);
void handleGuitarClick( RenderWindow& window,  Vector2i mousePixelPos);
void playGuitarNote(int stringNum, int fretNum);
bool isNoteCorrect(int s, int f);
void openGuitarFreePlay();
void openGuitarQuest(const GuitarNote* n, int c, float t);
bool isGuitarOpen();
void closeGuitar();
