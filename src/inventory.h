#ifndef LASTECHO_INVENTORY_H
#define LASTECHO_INVENTORY_H

#include "Data.h"

struct inventory {
    // Inventory GUI elements
    sf::Sprite invBar;
    sf::Texture invTex;
    sf::RectangleShape selector; // Highlight box for the selected slot

    // Slot data
    sf::Texture itemTextures[6];
    sf::Sprite itemSprites[6];
    std::string itemNames[6];
    bool hasItem[6] = { false };
    int selectedSlot = 0; // Currently active slot (0 to 5)
    int itemQuantity[6] = { 0 }; // Stack count for each item

    sf::Font font;
    sf::Text countText;

    // Item Pickup Visual Effects
    sf::Sprite feedbackSprite;
    sf::Texture feedbackTex;
    float feedbackTimer = 0.0f;
    bool isFirstTimeItem = false;
    sf::CircleShape sparkles[5];
    float sparkleAlphas[5] = {255, 200, 150, 100, 50};

    // Item Logic properties
    bool isUsable[6] = { true }; // Default items as usable
    sf::Sprite noteSprite;
    sf::Texture noteTex;
    bool isNoteVisible = false;

    // Core methods
    void invt_init(float W, float H);
    bool addItem(std::string name, std::string texturePath);
    void invt_update(sf::RenderWindow& window, AppState& currentState, sf::Vector2f playerPos, float dt);
    void invt_draw(sf::RenderWindow& window);
    void triggerPickupEffect(std::string texturePath);
    void initNote();
};

#endif // LASTECHO_INVENTORY_H