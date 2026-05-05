
#pragma once
#include <string>
#include <vector>
#include "HintSystem.h"

#define MAX_FLAGS 100
#define MAX_QUESTS_PER_PHASE 20
#define MAX_PHASES 7

struct Quest {
    std::string title;
    bool isCompleted = false;
};

struct Phase {
    std::string phaseTitle;
    Quest quests[MAX_QUESTS_PER_PHASE];
    int currentQuestIdx = 0;
};

struct PhaseSystem {
    Phase allPhases[MAX_PHASES];
    int currentPhaseIdx = 0;
    bool gameFlags[MAX_FLAGS] = { false };
    std::string pendingItemTexture = "";
};

void phaseInit(PhaseSystem& ps);
void updatePhaseLogic(PhaseSystem& ps, std::string npcName, HintSystem& hs);
void checkDialogueReward(PhaseSystem& ps);

bool canPickupString(PhaseSystem& ps, sf::Vector2f playerPos, std::string currentMap, HintSystem& hs);
void pickupString(PhaseSystem& ps, sf::Vector2f playerPos, std::string currentMap, HintSystem& hs);
void drawStrings(sf::RenderWindow& window, PhaseSystem& ps, std::string currentMap);

void updatePhase0(PhaseSystem& ps, std::string npcName, HintSystem& hs);
void updatePhase1(PhaseSystem& ps, std::string npcName, HintSystem& hs);
void updatePhase2(PhaseSystem& ps, std::string npcName, HintSystem& hs);
void updatePhase3(PhaseSystem& ps, std::string npcName, HintSystem& hs);
void updatePhase4(PhaseSystem& ps, std::string npcName, HintSystem& hs);
void updatePhase5(PhaseSystem& ps, std::string npcName, HintSystem& hs);
void updatePhase6(PhaseSystem& ps, std::string npcName, HintSystem& hs);

