
#ifndef LASTECHO_INVENTORY_H
#define LASTECHO_INVENTORY_H
#include "Data.h"
struct inventory {
    // inventory
    sf::Sprite invBar;
    sf::Texture invTex;
    sf::RectangleShape selector; // المربع اللي بيحدد إنت واقف فين
    sf::Texture itemTextures[6];
    sf::Sprite itemSprites[6];
    std::string itemNames[6];
    bool hasItem[6] = { false };
    int selectedSlot = 0; // الخانة المختارة من 0 لـ 5
    int itemQuantity[6] = { 0 }; // مصفوفة لتخزين عدد كل آيتم
    sf::Font font;               // فونت الأرقام
    sf::Text countText;          // نص الرقم
    // first time taking item effect ======
    sf::Sprite feedbackSprite;
    sf::Texture feedbackTex;
    float feedbackTimer = 0.0f;
    bool isFirstTimeItem = false;
    sf::CircleShape sparkles[5];
    float sparkleAlphas[5] = {255, 200, 150, 100, 50};
    bool isUsable[6] = { true }; // default all usable
    sf::Sprite noteSprite;
    sf::Texture noteTex;
    bool isNoteVisible = false;
    //================


    void invt_init(float W, float H);
    bool addItem(std::string name, std::string texturePath); // دالة جديدة للإضافة
    void invt_update(sf::RenderWindow& window, AppState& currentState, sf::Vector2f playerPos, float dt);
    void invt_draw(sf::RenderWindow& window);
    void triggerPickupEffect(std::string texturePath);
    void initNote();
};

#endif //LASTECHO_INVENTORY_H