#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// ==============================
// CONSTANTS
// ==============================
// نصيحة: استخدم constexpr في C++ لضمان إنها قيم ثابتة وقت الـ Compile
constexpr int TILE_SIZE   = 32;
constexpr int SCREEN_W    = 1280;
constexpr int SCREEN_H    = 720;
constexpr int PHASE_COUNT = 7;
constexpr int MAX_NPCS    = 20;
constexpr int MAX_ITEMS   = 16;
constexpr int MAX_PORTALS = 8;

// ==============================
// ENUMS
// ==============================
enum AppState {
    STATE_MENU,        // الشاشة الرئيسية
    STATE_PLAYING,     // وقت اللعب الفعلي
    STATE_DIALOGUE,    // وقت الكلام مع الـ NPCs
    STATE_BOSS_FIGHT,  // قتال البوس
    STATE_PAUSED,      // اللعبة واقفة مؤقتاً
    STATE_GAME_OVER,   // الخسارة
    STATE_SETTINGS,    // الإعدادات
    STATE_LOAD         // شاشة التحميل (بين المابات)
};

enum Direction {
    DIR_IDLE,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
};

// ==============================
// STRUCTS
// ==============================
struct Player {
    sf::Vector2f pos;
    sf::Vector2f velocity;
    Direction    facing;
    float        speed;
    int          hp;
    int          maxHp;
    int          xp;
    int          level;
    int          currentPhase;
    int          frameIndex;
    float        aniTimer;
    int xpToNextLevel;
    bool         isMoving;

    // إضافة الـ Sprite هنا عشان يبقى مركزي
    sf::Sprite   sprite;
};

struct GameState {
    AppState currentState;
    float    deltaTime;
    int      currentPhase;
    bool     phaseCompleted[PHASE_COUNT];
    bool     guidesMet[PHASE_COUNT];
    bool     bossDefeated[PHASE_COUNT];

    // إضافة الـ MapLoaded هنا عشان الليدر يتحكم في الـ Loading بسهولة
    bool     mapLoaded;
};

// ==============================
// GLOBALS — (Declaration Only)
// ==============================
// الـ extern يعني بنقول للـ Compiler إنهم متعرفين في الـ main.cpp
extern sf::RenderWindow window;
extern GameState        gState;
extern Player           player;