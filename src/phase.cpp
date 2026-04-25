#include "Phase.h"
#include "DialogueManager.h"
#include "inventory.h"
#include "NPC.h"
#include "Cutscene.h"
#include "Player.h"
#include "chest.h"

extern inventory inv;
extern NPC allNPCs[MAX_NPCS];

void phaseInit(PhaseSystem& ps) {
    ps.currentPhaseIdx = 0;
    for (int i = 0; i < MAX_FLAGS; i++) ps.gameFlags[i] = false;

    // ════════════════════════════════════════════════════════════════
    // PHASE 0: Amr's Magical Guitar
    // ════════════════════════════════════════════════════════════════
    ps.allPhases[0].phaseTitle = "Amr's Magical Guitar";
    ps.allPhases[0].quests[0] = { "The lost ID" };
    ps.allPhases[0].quests[1] = { "WELCOME FCIS!" };
    ps.allPhases[0].quests[2] = { "AMR THE GUITARIST" };
    ps.allPhases[0].quests[3] = { "The scattered Strings" };
    ps.allPhases[0].currentQuestIdx = 0;

    // ════════════════════════════════════════════════════════════════
    // PHASE 1: The Corruption Begins (Team 2 will complete)
    // ════════════════════════════════════════════════════════════════
    ps.allPhases[1].phaseTitle = "The Corruption Begins";
    ps.allPhases[1].quests[0] = { "Explore the campus" };
    ps.allPhases[1].currentQuestIdx = 0;

    // ════════════════════════════════════════════════════════════════
    // PHASES 2-6: Stubs for other teams
    // ════════════════════════════════════════════════════════════════
    ps.allPhases[2].phaseTitle = "The Discovery";
    ps.allPhases[2].quests[0] = { "Find the truth" };
    ps.allPhases[2].currentQuestIdx = 0;

    ps.allPhases[3].phaseTitle = "Betrayal";
    ps.allPhases[3].quests[0] = { "Uncover the betrayal" };
    ps.allPhases[3].currentQuestIdx = 0;

    ps.allPhases[4].phaseTitle = "Hidden Powers";
    ps.allPhases[4].quests[0] = { "Unlock new abilities" };
    ps.allPhases[4].currentQuestIdx = 0;

    ps.allPhases[5].phaseTitle = "Final Climax";
    ps.allPhases[5].quests[0] = { "The final confrontation" };
    ps.allPhases[5].currentQuestIdx = 0;

    ps.allPhases[6].phaseTitle = "Resolution";
    ps.allPhases[6].quests[0] = { "Restore peace" };
    ps.allPhases[6].currentQuestIdx = 0;
}

// ════════════════════════════════════════════════════════════════
// MAIN DISPATCHER - Routes to phase-specific handlers
// ════════════════════════════════════════════════════════════════
void updatePhaseLogic(PhaseSystem& ps, std::string npcName) {
    int pIdx = ps.currentPhaseIdx;

    // Dispatch to appropriate phase handler
    switch (pIdx) {
        case 0:
            updatePhase0(ps, npcName);
            break;
        case 1:
            updatePhase1(ps, npcName);
            break;
        case 2:
            updatePhase2(ps, npcName);
            break;
        case 3:
            updatePhase3(ps, npcName);
            break;
        case 4:
            updatePhase4(ps, npcName);
            break;
        case 5:
            updatePhase5(ps, npcName);
            break;
        case 6:
            updatePhase6(ps, npcName);
            break;
        default:
            break;
    }
}

void checkDialogueReward(PhaseSystem& ps) {
    if (ps.pendingItemTexture != "" && !isDialogueActive()) {
        inv.triggerPickupEffect(ps.pendingItemTexture);
        ps.pendingItemTexture = "";
    }
}

// ════════════════════════════════════════════════════════════════
// PHASE 0: Amr's Magical Guitar
// Team Member 1 - NO CHANGES TO EXISTING LOGIC
// ════════════════════════════════════════════════════════════════
void updatePhase0(PhaseSystem& ps, std::string npcName) {
    if (npcName == "Friend_NPC") {
        if (!ps.gameFlags[0]) {
            std::string lines1[] = { "Oh! Your ID card is here.", "Take it and go to the gate." };
            inv.addItem("id_card", "assets/items/idcard.png");
            ps.pendingItemTexture = "assets/items/idcard.png";
            startDialogue("Friend", lines1, 2, getNPCAvatar("Friend_NPC"));
            ps.gameFlags[0] = true;
            ps.allPhases[0].currentQuestIdx = 1;
        } else {
            std::string lines2[] = { "Go! The security is waiting." };
            startDialogue("Friend", lines2, 1, getNPCAvatar("Friend_NPC"));
        }
    }
    else if (npcName == "Security_Guard") {
        if (ps.gameFlags[0]) {
            std::string ok[] = { "Valid ID. Welcome to FCIS!", "Go find Amr in the Hallway, he's waiting." };
            startDialogue("Security", ok, 2, getNPCAvatar("Security_Guard"));
            ps.allPhases[0].currentQuestIdx = 2;
        } else {
            std::string no[] = { "No ID, No entry!", "Go find your ID first." };
            startDialogue("Security", no, 2, getNPCAvatar("Security_Guard"));
        }
    }
    else if (npcName == "amr") {
        if (ps.allPhases[0].currentQuestIdx < 2) {
            std::string wait[] = { "I can't talk now, get your ID first!" };
            startDialogue("Amr", wait, 1, getNPCAvatar("amr"));
        }
        else if (ps.allPhases[0].currentQuestIdx == 2 && !isCutsceneActive()) {
            std::vector<CutsceneAction> steps;

            // --- Emotions ---
            CutsceneAction a1;
            a1.type = CUTSCENE_EMOTION;
            a1.characterName = "amr";
            a1.emotion = EMOTION_SURPRISE;
            a1.emotionDuration = 2.0f;
            steps.push_back(a1);

            CutsceneAction a15;
            a15.type = CUTSCENE_EMOTION;
            a15.characterName = "amr";
            a15.emotion = EMOTION_LOVE;
            a15.emotionDuration = 2.0f;
            steps.push_back(a15);

            // --- Dialogue ---
            CutsceneAction s2; s2.type = CUTSCENE_SPEAK; s2.characterName = "amr";
            s2.lineCount = 6;
            s2.lines[0] = "So… you can see me.";
            s2.lines[1] = "Good. That means you're not like the others.";
            s2.lines[2] = "This place wasn't always like this.";
            s2.lines[3] = "Something broke and now things are slipping through.";
            s2.lines[4] = "And somehow you got pulled into all of this.";
            s2.lines[5] = "Follow me..";
            steps.push_back(s2);

            // --- Lobby Movement ---
            CutsceneAction s3; s3.type = CUTSCENE_MOVE; s3.characterName = "amr";
            s3.targetX = 1058.85f; s3.targetY = 297.645f;
            steps.push_back(s3);

            CutsceneAction s4; s4.type = CUTSCENE_PLAYER_MOVE;
            s4.targetX = 1010.85f; s4.targetY = 297.645f;
            steps.push_back(s4);

            // --- Transition to vertPassage ---
            CutsceneAction s7; s7.type = CUTSCENE_CHANGE_MAP; s7.characterName = "amr";
            s7.targetMap = "vertPassage"; s7.spawnPos = {255.067f, 884.582f};
            steps.push_back(s7);

            CutsceneAction s8; s8.type = CUTSCENE_PLAYER_TELEPORT;
            s8.targetMap = "vertPassage"; s8.spawnPos = {255.067f, 884.582f};
            steps.push_back(s8);

            // --- vertPassage Movement ---
            CutsceneAction s9; s9.type = CUTSCENE_MOVE; s9.characterName = "amr";
            s9.targetX = 255.067f; s9.targetY = 294.625f;
            steps.push_back(s9);

            CutsceneAction s10; s10.type = CUTSCENE_PLAYER_MOVE;
            s10.targetX = 255.067f; s10.targetY = 350.0f;
            steps.push_back(s10);

            CutsceneAction s11; s11.type = CUTSCENE_MOVE; s11.characterName = "amr";
            s11.targetX = 374.044f; s11.targetY = 350.0f;
            steps.push_back(s11);

            CutsceneAction s12; s12.type = CUTSCENE_PLAYER_MOVE;
            s12.targetX = 295.044f; s12.targetY = 350.0f;
            steps.push_back(s12);

            // --- Transition to wcm2 ---
            CutsceneAction s13; s13.type = CUTSCENE_CHANGE_MAP; s13.characterName = "amr";
            s13.targetMap = "wcm2";
            s13.spawnPos = {340.0f, 542.879f};
            steps.push_back(s13);

            CutsceneAction s14; s14.type = CUTSCENE_PLAYER_TELEPORT;
            s14.targetMap = "wcm2";
            s14.spawnPos = {380.0f, 542.879f};
            steps.push_back(s14);

            // --- wcm2 Movement ---
            CutsceneAction moveUpAmr; moveUpAmr.type = CUTSCENE_MOVE; moveUpAmr.characterName = "amr";
            moveUpAmr.targetX = 150.0f;
            moveUpAmr.targetY = 542.879f;
            steps.push_back(moveUpAmr);

            CutsceneAction moveUpPlayer; moveUpPlayer.type = CUTSCENE_PLAYER_MOVE;
            moveUpPlayer.targetX = 200.0f;
            moveUpPlayer.targetY = 542.879f;
            steps.push_back(moveUpPlayer);

            // --- Ending ---
            CutsceneAction s16;
            s16.type = CUTSCENE_EMOTION;
            s16.characterName = "amr";
            s16.emotion = EMOTION_SAD;
            s16.emotionDuration = 5.0f;
            steps.push_back(s16);

            CutsceneAction s17; s17.type = CUTSCENE_SPEAK; s17.characterName = "amr";
            s17.lineCount = 5;
            s17.lines[0] = "I can't fight like this.";
            s17.lines[1] = "My power... it's tied to my guitar.";
            s17.lines[2] = "It was shattered. The strings were scattered.";
            s17.lines[3] = "Find them... all of them.";
            s17.lines[4] = "Then come back to me.";
            steps.push_back(s17);

            startGenericCutscene("amr_first_meeting", steps);

            for (int i = 0; i < MAX_NPCS; i++) {
                if (allNPCs[i].name == "amr") {
                    allNPCs[i].waypointsCount = 0;
                    allNPCs[i].isStatic = true;
                    break;
                }
            }
            ps.allPhases[0].currentQuestIdx = 3;
        }
    }
    else if (npcName == "Key_Keeper") {
        if (ps.allPhases[0].currentQuestIdx < 3) {
            std::string notYet[] = { "I don't know you yet.", "Come back later." };
            startDialogue("Stranger", notYet, 2, getNPCAvatar("Key_Keeper"));
        }
        else if (!ps.gameFlags[1]) {
            std::string giveKey[] = {
                "So... Amr sent you.",
                "I've been keeping this safe for a long time.",
                "Take it. The chest is somewhere in this building.",
                "Find the strings inside. Amr needs them."
            };
            inv.addItem("key", "assets/sprites/items/key.png");
            ps.pendingItemTexture = "assets/items/key.png";
            ps.gameFlags[1] = true;
            startDialogue("Keeper", giveKey, 4, getNPCAvatar("Key_Keeper"));
        }
        else {
            std::string already[] = {
                "You already have the key.",
                "Go find the chest."
            };
            startDialogue("Keeper", already, 2, getNPCAvatar("Key_Keeper"));
        }
    }
    else if (npcName == "Receptionist") {
        std::string liness[] = {"Morning.",
            "It looks like the perfect day to sit and enjoy my tea.",
            "But remember... not everything in this place is truly like how it seems." };
        startDialogue("Receptionist", liness, 3, getNPCAvatar("Receptionist"));
    }
    else if (npcName == "student") {
        std::string liness[] = { "You look new here.",
            "Take care... some doors are better left unopened."};
        startDialogue("student1", liness, 2, getNPCAvatar("student"));
    }
}

// ════════════════════════════════════════════════════════════════
// PHASE 1-6: Stubs
// ════════════════════════════════════════════════════════════════

void updatePhase1(PhaseSystem& ps, std::string npcName) {
    // Team 2 implements Phase 1: The Corruption Begins
    // Can handle: Saged NPC, enemy encounters, exploration, etc
}

void updatePhase2(PhaseSystem& ps, std::string npcName) {
    // Team 3 implements Phase 2: The Discovery
}

void updatePhase3(PhaseSystem& ps, std::string npcName) {
    // Team 4 implements Phase 3: Betrayal
}

void updatePhase4(PhaseSystem& ps, std::string npcName) {
    // Team 5 implements Phase 4: Hidden Powers
}

void updatePhase5(PhaseSystem& ps, std::string npcName) {
    // Team 6 implements Phase 5: Final Climax
}

void updatePhase6(PhaseSystem& ps, std::string npcName) {
    // Team 7 implements Phase 6: Resolution
}