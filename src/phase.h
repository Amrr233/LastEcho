#ifndef PHASE_H
#define PHASE_H

#include <string>
#include <vector>

#define MAX_FLAGS 100
#define MAX_QUESTS_PER_PHASE 5
#define MAX_PHASES 7

// Event types that phases can handle
enum EventType {
    EVENT_NPC_DIALOGUE,      // NPC dialogue triggered
    EVENT_QUEST_COMPLETE,    // Quest marked complete
    EVENT_CUTSCENE_END,      // Cutscene finished
    EVENT_MINIGAME_COMPLETE, // Minigame (guitar, etc) finished
    EVENT_ENEMY_DEFEAT,      // Enemy defeated
    EVENT_BOSS_DEFEAT,       // Boss defeated
    EVENT_ITEM_COLLECTED,    // Item picked up
    EVENT_EXPLORATION,       // Location explored
    EVENT_CUSTOM            // Custom phase-specific event
};

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

// ════════════════════════════════════════════════════════════════
// CORE PHASE FUNCTIONS
// ════════════════════════════════════════════════════════════════

void phaseInit(PhaseSystem& ps);

// Main dispatcher - routes to phase-specific handlers
void updatePhaseLogic(PhaseSystem& ps, std::string npcName);

// Check if dialogue reward is pending
void checkDialogueReward(PhaseSystem& ps);

// ════════════════════════════════════════════════════════════════
// PHASE-SPECIFIC HANDLER DECLARATIONS
// These will be implemented in separate Phase_X_*.cpp files
// ════════════════════════════════════════════════════════════════

// Phase 0: Amr's Magical Guitar
void updatePhase0(PhaseSystem& ps, std::string npcName);

// Phase 1: The Corruption Begins (stub - Team 2 will implement)
void updatePhase1(PhaseSystem& ps, std::string npcName);

// Phase 2: The Discovery (stub - Team 3 will implement)
void updatePhase2(PhaseSystem& ps, std::string npcName);

// Phase 3: Betrayal (stub - Team 4 will implement)
void updatePhase3(PhaseSystem& ps, std::string npcName);

// Phase 4: Hidden Powers (stub - Team 5 will implement)
void updatePhase4(PhaseSystem& ps, std::string npcName);

// Phase 5: Final Climax (stub - Team 6 will implement)
void updatePhase5(PhaseSystem& ps, std::string npcName);

// Phase 6: Resolution (stub - Team 7 will implement)
void updatePhase6(PhaseSystem& ps, std::string npcName);

#endif