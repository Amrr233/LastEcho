#include "inventory.h"
#include <iostream>
#include <cmath>
#include "audio.h"

void inventory::initNote() {
    if (noteTex.loadFromFile("assets/items/note_message.png")) {
        noteSprite.setTexture(noteTex);
        noteSprite.setOrigin(noteTex.getSize().x / 2.0f, noteTex.getSize().y / 2.0f);
        noteSprite.setPosition(SCREEN_W / 2.0f, SCREEN_H / 2.0f);
    }
}

void inventory::invt_init(float W, float H) {
    invTex.loadFromFile("assets/gameplay/inventory_bar.png");
    font.loadFromFile("assets/fonts/pixelsix00.ttf");

    countText.setFont(font);
    countText.setCharacterSize(14);
    countText.setFillColor(sf::Color::White);
    countText.setOutlineColor(sf::Color::Black);
    countText.setOutlineThickness(1);

    invBar.setTexture(invTex);

    invBar.setOrigin(invTex.getSize().x / 2.0f, invTex.getSize().y);
    invBar.setPosition(SCREEN_W / 2, SCREEN_H - 10.f);
    invBar.setScale(0.5f, 0.5f);

    selector.setSize(sf::Vector2f(50.0f, 50.0f));
    selector.setFillColor(sf::Color::Transparent);
    selector.setOutlineColor(sf::Color(212, 181, 125));
    selector.setOutlineThickness(3);

    initNote();
}

void inventory::invt_update(sf::RenderWindow& window, AppState& currentState, sf::Vector2f playerPos, float dt) {
    for (int i = 0; i < 6; i++) {
        if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(sf::Keyboard::Num1 + i))) {
            selectedSlot = i;
        }
    }

    if (hasItem[selectedSlot] && itemNames[selectedSlot] == "note") {
        isNoteVisible = true;
    } else {
        isNoteVisible = false;
    }

    float startX = invBar.getPosition().x - (invTex.getSize().x / 2.0f) + 5.0f;
    selector.setPosition(startX + 270.f + (selectedSlot * 70.5f), invBar.getPosition().y - 95.0f);

    if (feedbackTimer > 0) {
        float travelDistance = 15.0f;
        float startHeight = 70.0f;
        float totalTime = 2.0f;

        float progress = (totalTime - feedbackTimer) / totalTime;
        float floatHeight = startHeight + (progress * travelDistance);

        feedbackSprite.setPosition(playerPos.x + 24.f, playerPos.y - floatHeight);

        sf::Uint8 alpha = static_cast<sf::Uint8>(feedbackTimer / totalTime * 255);
        feedbackSprite.setColor(sf::Color(255, 255, 255, alpha));

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

        if (itemQuantity[i] > 1) {
            countText.setString(std::to_string(itemQuantity[i]));
            sf::Vector2f itemPos = itemSprites[i].getPosition();
            countText.setPosition(itemPos.x + 10.f, itemPos.y + 12.f);
            window.draw(countText);
        }
    }

    if (isNoteVisible) {
        sf::RectangleShape dim(sf::Vector2f(SCREEN_W, SCREEN_H));
        dim.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(dim);
        window.draw(noteSprite);
    }
}

bool inventory::addItem(std::string name, std::string texturePath) {
    for (int i = 0; i < 6; i++) {
        if (hasItem[i] && itemNames[i] == name) {
            itemQuantity[i]++;
            std::cout << "Stacked " << name << " in slot " << i << ". Total: " << itemQuantity[i] << std::endl;
            return false;
        }
    }

    for (int i = 0; i < 6; i++) {
        if (!hasItem[i]) {
            if (itemTextures[i].loadFromFile(texturePath)) {
                itemSprites[i].setTexture(itemTextures[i]);

                float maxSize = 40.f;
                float texW = (float)itemTextures[i].getSize().x;
                float texH = (float)itemTextures[i].getSize().y;
                float scale = std::min(maxSize / texW, maxSize / texH);

                itemSprites[i].setScale(scale, scale);
                itemNames[i] = name;
                itemQuantity[i] = 1;
                hasItem[i] = true;

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

        for (int i = 0; i < 5; i++) {
            sparkles[i].setRadius(2.f + i);
            sparkles[i].setFillColor(sf::Color(255, 255, 200));
        }
    }
}