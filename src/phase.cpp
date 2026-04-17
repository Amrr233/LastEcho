#include "Phase.h"
#include "DialogueManager.h"

void phaseInit(PhaseSystem& ps) {
    ps.currentPhaseIdx = 0;
    for(int i=0; i<MAX_FLAGS; i++) ps.gameFlags[i] = false;

    // المرحلة 0: يوم الكارنيه
    ps.allPhases[0].phaseTitle = "Amr's Magical Guitar";
    ps.allPhases[0].quests[0] = { "Find your ID with your friend" };
    ps.allPhases[0].quests[1] = { "Show ID to Security" };


    ps.allPhases[1].phaseTitle = "Saged's";
    ps.allPhases[1].quests[0] = { "Look for Amr in the hallway" };
    ps.allPhases[1].currentQuestIdx = 0;
}

void updatePhaseLogic(PhaseSystem& ps, std::string npcName) {
    int pIdx = ps.currentPhaseIdx;

    // --- منطق المرحلة 0 ---
    if (pIdx == 0) {
        if (npcName == "Friend_NPC") {
            if (!(ps.gameFlags[0])) { // لو لسه مخدتش الكارنيه
                std::string lines[] = {"Oh! Your ID card is here.", "Take it and go to the gate."};
                startDialogue("Friend", lines, 2);
                ps.gameFlags[0] = true; // رفعت علم "معايا الكارنيه"
                ps.allPhases[0].currentQuestIdx = 1;
            } else {
                std::string lines[] = {"Go! The security is waiting."};
                startDialogue("Friend", lines, 1);
            }
        }
        else if (npcName == "Security_Guard") {
            if (ps.gameFlags[0]) { // لو معاك الكارنيه
                std::string lines[] = {"Valid ID. Welcome to FCIS!", "You can enter now."};
                startDialogue("Security", lines, 2);
                ps.currentPhaseIdx = 1; // انقل لـ Phase 1 (مقابلة عمرو)
            } else {
                std::string lines[] = {"No ID, No entry!", "Go find your ID first."};
                startDialogue("Security", lines, 2);
            }
        }
    }
    // هنا هتضيف Phase 1 و 2 بنفس الطريقة...
}