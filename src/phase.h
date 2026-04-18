#ifndef PHASE_H
#define PHASE_H

#include <string>

#define MAX_FLAGS 100

struct Quest {
    std::string title;
    bool isCompleted = false;
};

struct Phase {
    std::string phaseTitle;
    Quest quests[5];
    int currentQuestIdx = 0;
};

struct PhaseSystem {
    Phase allPhases[7];
    int currentPhaseIdx = 0;
    bool gameFlags[MAX_FLAGS] = { false }; // مخزن الأحداث (0: كارنيه، 1: بوابة، الخ)
    std::string pendingItemTexture = ""; // مسار صورة الأيتم اللي مستني الديالوج يخلص
};

// تعريف الدوال
void phaseInit(PhaseSystem& ps);
void updatePhaseLogic(PhaseSystem& ps, std::string npcName);
void checkDialogueReward(PhaseSystem& ps);

#endif