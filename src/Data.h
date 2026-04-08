#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
using namespace sf;

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





// ==============================
// STRUCTS
// ==============================

enum Direction { SOUTH = 0, NORTH = 1, WEST = 2, EAST = 3 };
enum playerState { IDLE, WALKING };

struct Player {
    sf::Vector2f pos;
    sf::Vector2f velocity;
    float speed;

    // البيانات الأساسية
    int hp, maxHp, xp, level;

    // الأنميشن (الصور والمنطق)
    sf::Texture walkTextures[4]; // شلنا الـ Idle عشان نخفف الرام
    Direction   facing;
    playerState currentState;
    int   currentFrame;
    float animationTimer;
    bool  isMoving;
    int xpToNextLevel = 100;

    // المقاسات الجديدة اللي اتفقنا عليها (48x48)
    int frameWidth = 48;
    int frameHeight = 48;
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