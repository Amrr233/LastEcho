#ifndef CUTSCENE_H
#define CUTSCENE_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Data.h"

// 1. الـ Enums الأول
enum Emotion {
    EMOTION_NONE = -1,
    EMOTION_QUESTION = 0,
    EMOTION_ANGRY,
    EMOTION_SURPRISE,
    EMOTION_LOVE,
    EMOTION_SLEEP,
    EMOTION_SAD,
    EMOTION_HAPPY,
    EMOTION_ERROR,
    EMOTION_TALK,
    EMOTION_REJECT,
    EMOTION_THINK,
    EMOTION_PLAY,
    EMOTION_MUSIC,
    EMOTION_SHY
};

enum CutsceneActionType {
    CUTSCENE_MOVE,
    CUTSCENE_SPEAK,
    CUTSCENE_EMOTION,
    CUTSCENE_WAIT,
    CUTSCENE_CHANGE_MAP,
    CUTSCENE_PLAYER_MOVE,
    CUTSCENE_PLAYER_TELEPORT
};

// 2. الـ Action لازم يجي قبل الـ Cutscene
struct CutsceneAction {
    CutsceneActionType type;
    std::string characterName;
    float targetX;
    float targetY;
    float duration;
    std::string lines[MAX_DIALOGUE_LINES];
    int lineCount;
    Emotion emotion;
    float emotionDuration;
    float waitTime;
    float actionTimer = 0.0f;
    std::string targetMap;
    sf::Vector2f spawnPos;
};

// 3. الـ Cutscene لازم يجي قبل الـ Runtime (عشان الـ Pointer اللي جواه)
struct Cutscene {
    std::string cutsceneID;
    std::vector<CutsceneAction> actions;
    bool isActive = false;
    int currentActionIdx = 0;
};

// 4. الـ Runtime اللي شايل كل حاجة
struct CutsceneRuntime {
    Cutscene* currentCutscene = nullptr; // هنا الكومبيلر خلاص عرف يعني ايه Cutscene
    struct CharState {
        std::string name;
        sf::Vector2f pos;
        sf::Vector2f targetPos;
        bool isMoving = false;
        Emotion currentEmotion = EMOTION_NONE;
        float emotionTimer = 0.f;
        int currentFrame = 0;
    } characters[5];
    int characterCount = 0;
    bool isActive = false;

    sf::Texture emoteSheet;
    sf::Sprite emoteSprite;
    const int EMOTE_SIZE = 64;
    const int FRAME_COUNT = 4;
    const float FRAME_TIME = 0.15f;
};

extern CutsceneRuntime g_cutscene;

// 5. الدوال في الآخر خالص
void initCutsceneSystem();
void updateCutscene(float deltaTime);
void drawCutsceneOverlay(sf::RenderWindow& window, sf::Font& font);
bool isCutsceneActive();
void startGenericCutscene(std::string id, std::vector<CutsceneAction> steps);
sf::IntRect calculateEmoteRect(Emotion emotion, int frame);

#endif