#include "inventory.h"
#include <iostream>
#include <cmath>
#include "audio.h"

void inventory::initNote() {
    // Load the full-screen note message texture
    if (noteTex.loadFromFile("assets/items/note_message.png")) {
        noteSprite.setTexture(noteTex);
        // Center the note on screen
        noteSprite.setOrigin(noteTex.getSize().x / 2.0f, noteTex.getSize().y / 2.0f);
        noteSprite.setPosition(SCREEN_W / 2.0f, SCREEN_H / 2.0f);
    }
}

void inventory::invt_init(float W, float H) {
    // Load assets and font
    invTex.loadFromFile("assets/gameplay/inventory_bar.png");
    font.loadFromFile("assets/fonts/pixelsix00.ttf");

    // Setup quantity counter text style
    countText.setFont(font);
    countText.setCharacterSize(14);
    countText.setFillColor(sf::Color::White);
    countText.setOutlineColor(sf::Color::Black);
    countText.setOutlineThickness(1);

    invBar.setTexture(invTex);

    // Position inventory bar at bottom center
    invBar.setOrigin(invTex.getSize().x / 2.0f, invTex.getSize().y);
    invBar.setPosition(SCREEN_W / 2, SCREEN_H - 10.f);
    invBar.setScale(0.5f, 0.5f);

    // Setup the selection rectangle (selector)
    selector.setSize(sf::Vector2f(50.0f, 50.0f));
    selector.setFillColor(sf::Color::Transparent);
    selector.setOutlineColor(sf::Color(212, 181, 125));
    selector.setOutlineThickness(3);

    initNote();
}

void inventory::invt_update(sf::RenderWindow& window, AppState& currentState, sf::Vector2f playerPos, float dt) {
    // Slot selection via Number Keys 1-6
    for (int i = 0; i < 6; i++) {
        if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(sf::Keyboard::Num1 + i))) {
            selectedSlot = i;
        }
    }

    // Toggle note visibility if "note" item is selected
    if (hasItem[selectedSlot] && itemNames[selectedSlot] == "note") {
        isNoteVisible = true;
    } else {
        isNoteVisible = false;
    }

    // Update selector position based on active slot
    float startX = invBar.getPosition().x - (invTex.getSize().x / 2.0f) + 5.0f;
    selector.setPosition(startX + 270.f + (selectedSlot * 70.5f), invBar.getPosition().y - 95.0f);

    // Update item pickup floating animation
    if (feedbackTimer > 0) {
        float travelDistance = 15.0f; // Total vertical floating distance
        float startHeight = 70.0f;    // Initial height above player
        float totalTime = 2.0f;       // Total animation duration

        float progress = (totalTime - feedbackTimer) / totalTime;
        float floatHeight = startHeight + (progress * travelDistance);

        feedbackSprite.setPosition(playerPos.x + 24.f, playerPos.y - floatHeight);

        // Apply fade-out transparency
        sf::Uint8 alpha = static_cast<sf::Uint8>(feedbackTimer / totalTime * 255);
        feedbackSprite.setColor(sf::Color(255, 255, 255, alpha));

        // Animate sparkles around the item
        for (int i = 0; i < 5; i++) {
            sparkles[i].setPosition(
                feedbackSprite.getPosition().x + sin(feedbackTimer * 10 + i) * 15.f,
                feedbackSprite.getPosition().y + cos(feedbackTimer * 10 + i) * 15.f
            );
            sparkles[i].setFillColor(sf::Color(255, 255, 200, alpha));
        }

        feedbackTimer -= dt;
    }
}

void inventory::invt_draw(sf::RenderWindow& window) {
    window.draw(invBar);
    window.draw(selector);

    float offsetX = 60.f;
    float offsetY = 70.f;
    float gap     = 70.5f;

    for (int i = 0; i < 6; i++) {
        if (!hasItem[i]) continue;

        itemSprites[i].setOrigin(
            itemTextures[i].getSize().x / 2.0f,
            itemTextures[i].getSize().y / 2.0f
        );

        float startX = invBar.getPosition().x - (invBar.getGlobalBounds().width / 2.0f);
        itemSprites[i].setPosition(
            startX + offsetX + (i * gap),
            invBar.getPosition().y - offsetY
        );

        window.draw(itemSprites[i]);

        // Draw quantity text if more than 1 item in stack
        if (itemQuantity[i] > 1) {
            countText.setString(std::to_string(itemQuantity[i]));
            sf::Vector2f itemPos = itemSprites[i].getPosition();
            countText.setPosition(itemPos.x + 10.f, itemPos.y + 12.f);
            window.draw(countText);
        }
    }

    // Draw full-screen note UI
    if (isNoteVisible) {
        sf::RectangleShape dim(sf::Vector2f(SCREEN_W, SCREEN_H));
        dim.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(dim);
        window.draw(noteSprite);
    }
}

bool inventory::addItem(std::string name, std::string texturePath) {
    // Check if the item already exists to stack it
    for (int i = 0; i < 6; i++) {
        if (hasItem[i] && itemNames[i] == name) {
            itemQuantity[i]++;
            std::cout << "Stacked " << name << " in slot " << i << ". Total: " << itemQuantity[i] << std::endl;
            return false;
        }
    }

    // If not found, look for the first empty slot
    for (int i = 0; i < 6; i++) {
        if (!hasItem[i]) {
            if (itemTextures[i].loadFromFile(texturePath)) {
                itemSprites[i].setTexture(itemTextures[i]);

                // Auto-fit item to slot (Max 40x40)
                float maxSize = 40.f;
                float texW = (float)itemTextures[i].getSize().x;
                float texH = (float)itemTextures[i].getSize().y;
                float scale = std::min(maxSize / texW, maxSize / texH);

                itemSprites[i].setScale(scale, scale);
                itemNames[i] = name;
                itemQuantity[i] = 1;
                hasItem[i] = true;

                // Mark non-usable items (e.g., strings)
                if (name == "guitar_string") isUsable[i] = false;
                else isUsable[i] = true;

                return true;
            }
        }
    }
    return false;
}

void inventory::triggerPickupEffect(std::string texturePath) {
    audioManager.playpickupSound();
    if (feedbackTex.loadFromFile(texturePath)) {
        feedbackSprite.setTexture(feedbackTex);
        feedbackSprite.setOrigin(feedbackTex.getSize().x / 2.0f, feedbackTex.getSize().y / 2.0f);
        feedbackTimer = 2.0f; // 2-second animation duration

        // Initialize sparkles
        for (int i = 0; i < 5; i++) {
            sparkles[i].setRadius(2.f + i);
            sparkles[i].setFillColor(sf::Color(255, 255, 200));
        }
    }
}