#ifndef DIALOGUE_MANAGER_H
#define DIALOGUE_MANAGER_H

#include <SFML/Graphics.hpp>
#include <string>

#define MAX_DIALOGUE_LINES 10

class DialogueManager {
private:
    sf::Sprite boxSprite;
    sf::Texture boxTexture;
    sf::Font font;
    sf::Text dialogueText;
    sf::Text nameText;

    bool isOpen = false;
    std::string currentMessages[MAX_DIALOGUE_LINES];
    int totalLines = 0;
    int currentLineIdx = 0;

    // 🔥 متغيرات أنميشن الكتابة
    std::string fullText;       // الجملة كاملة
    std::string displayText;    // الجزء اللي هيظهر تدريجياً
    float typeTimer = 0.f;
    float typeSpeed = 0.03f;    // سرعة الكتابة
    int charIdx = 0 ;

    void centerText();
public:
    DialogueManager();
    void init();
    void startDialogue(std::string name, std::string messages[], int count);
    void nextLine();
    void update(); // لو مش محتاجها سيبها فاضية
    void draw(sf::RenderWindow& window);

    bool isDialogueActive() const { return isOpen; }
};

extern DialogueManager dialogueSystem;

#endif