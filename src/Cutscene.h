#ifndef CUTSCENE_H
#define CUTSCENE_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Data.h"

// ════════════════════════════════════════════════════════════════
// ENUMS
// ════════════════════════════════════════════════════════════════
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
    CUTSCENE_PLAYER_TELEPORT,
    CUTSCENE_MOVE_GROUP,
    CUTSCENE_SET_DIRECTION
};

// ════════════════════════════════════════════════════════════════
// MOVER TARGET
// startDelay = استنى كام ثانية من بداية الـ group قبل ما تبدأ تتحرك
//   0.f  = يبدأ فوراً مع بداية الـ group
//   0.8f = يستنى 0.8 ثانية وبعدين يبدأ (والـ NPC التاني ماشي)
// ════════════════════════════════════════════════════════════════
struct MoverTarget {
    enum MoverType { NPC, PLAYER } type;
    std::string npcName;
    float targetX;
    float targetY;
    float speed;
    float startDelay;   // ← الجديد: استنى كام ثانية قبل ما تبدأ تتحرك

    MoverTarget(MoverType t, std::string name, float x, float y,
                float spd = 160.f, float delay = 0.f)
        : type(t), npcName(std::move(name)),
          targetX(x), targetY(y), speed(spd), startDelay(delay) {}
};

struct MoveGroup {
    std::vector<MoverTarget> movers;
};

// ════════════════════════════════════════════════════════════════
// CUTSCENE ACTION
// ════════════════════════════════════════════════════════════════
struct CutsceneAction {
    CutsceneActionType type = CUTSCENE_WAIT;
    std::string characterName;
    float targetX           = 0.f;
    float targetY           = 0.f;
    float duration          = 0.f;
    std::string lines[MAX_DIALOGUE_LINES];
    int lineCount           = 0;
    Emotion emotion         = EMOTION_NONE;
    float emotionDuration   = 0.f;
    float waitTime          = 0.f;
    float actionTimer       = 0.f;
    std::string targetMap;
    sf::Vector2f spawnPos;

    // MoveGroup fields
    MoveGroup moveGroup;
    bool  isStarted          = false;
    float completionDistance = 20.f;
    int direction = 0;
};

// ════════════════════════════════════════════════════════════════
// CUTSCENE + RUNTIME
// ════════════════════════════════════════════════════════════════
struct Cutscene {
    std::string cutsceneID;
    std::vector<CutsceneAction> actions;
    bool isActive        = false;
    int currentActionIdx = 0;
};

struct CutsceneRuntime {
    Cutscene* currentCutscene = nullptr;

    struct CharState {
        std::string  name;
        sf::Vector2f pos;
        sf::Vector2f targetPos;
        bool         isMoving       = false;
        bool         isStarted      = false;   // ابدأ يتحرك؟
        bool         isArrived      = false;   // وصل للـ target؟
        float        startDelay     = 0.f;     // كام ثانية قبل ما يبدأ
        float        delayTimer     = 0.f;     // عداد الـ delay
        Emotion      currentEmotion = EMOTION_NONE;
        float        emotionTimer   = 0.f;
        int          currentFrame   = 0;
        float        speed          = 160.f;
    } characters[5];

    // Player start delay tracking
    bool  playerMoving      = false;
    bool  playerStarted     = false;
    float playerStartDelay  = 0.f;
    float playerDelayTimer  = 0.f;
    float playerTargetX     = 0.f;
    float playerTargetY     = 0.f;
    float playerSpeed       = 180.f;

    int  characterCount = 0;
    bool isActive       = false;

    sf::Texture emoteSheet;
    sf::Sprite  emoteSprite;
    const int   EMOTE_SIZE  = 64;
    const int   FRAME_COUNT = 4;
    const float FRAME_TIME  = 0.15f;
};

extern CutsceneRuntime g_cutscene;

// ════════════════════════════════════════════════════════════════
// FUNCTIONS
// ════════════════════════════════════════════════════════════════
void        initCutsceneSystem();
void        updateCutscene(float deltaTime);
void        drawCutsceneOverlay(sf::RenderWindow& window, sf::Font& font);
bool        isCutsceneActive();
void        startGenericCutscene(std::string id, std::vector<CutsceneAction> steps);
void        stopCutscene();
sf::IntRect calculateEmoteRect(Emotion emotion, int frame);

#endif