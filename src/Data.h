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
constexpr int MAX_WAYPOINTS = 100;
constexpr int MAX_DIALOGUE_LINES = 100;


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


//
// ==============================
// STRUCTS
// ==============================

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
extern AppState         last_state;
// الإعلان عن مصفوفة الأعلام (Flags) عشان الكل يشوفها
extern bool gameFlags[100];