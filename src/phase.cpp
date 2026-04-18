#include "Phase.h"
#include "DialogueManager.h"
#include "inventory.h"
#include "NPC.h"

extern inventory inv;
extern NPC allNPCs[MAX_NPCS]; // مهم

void phaseInit(PhaseSystem& ps) {

    ps.currentPhaseIdx = 0;

    for (int i = 0; i < MAX_FLAGS; i++)
        ps.gameFlags[i] = false;

    ps.allPhases[0].phaseTitle = "Amr's Magical Guitar";
    ps.allPhases[0].quests[0] = { "Find your ID with your friend" };
    ps.allPhases[0].quests[1] = { "Show ID to Security" };

    ps.allPhases[1].phaseTitle = "Saged's";
    ps.allPhases[1].quests[0] = { "Look for Amr in the hallway" };
    ps.allPhases[1].currentQuestIdx = 0;
}

void updatePhaseLogic(PhaseSystem& ps, std::string npcName) {

    int pIdx = ps.currentPhaseIdx;

    if (pIdx == 0) {

        if (npcName == "Friend_NPC") {

            std::string lines1[] = {
                "Oh! Your ID card is here.",
                "Take it and go to the gate."
            };

            std::string lines2[] = {
                "Go! The security is waiting."
            };

            if (!ps.gameFlags[0]) {

                inv.addItem("id_card", "assets/items/idcard.png");
                ps.pendingItemTexture = "assets/items/idcard.png";

                startDialogue("Friend", lines1, 2, allNPCs[0].avatarTexture);

                ps.gameFlags[0] = true;
                ps.allPhases[0].currentQuestIdx = 1;

            } else {
                startDialogue("Friend", lines2, 1, allNPCs[0].avatarTexture);
            }
        }

        else if (npcName == "Security_Guard") {

            std::string ok[] = {
                "Valid ID. Welcome to FCIS!",
                "You can enter now."
            };

            std::string no[] = {
                "No ID, No entry!",
                "Go find your ID first."
            };

            if (ps.gameFlags[0]) {
                startDialogue("Security", ok, 2, allNPCs[0].avatarTexture);
                ps.currentPhaseIdx = 1;
            } else {
                startDialogue("Security", no, 2, allNPCs[0].avatarTexture);
            }
        }
    }
}

void checkDialogueReward(PhaseSystem& ps) {

    if (ps.pendingItemTexture != "" && !isDialogueActive()) {

        inv.triggerPickupEffect(ps.pendingItemTexture);
        ps.pendingItemTexture = "";
    }
}