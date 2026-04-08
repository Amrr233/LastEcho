
#ifndef LASTECHO_INVENTORY_H
#define LASTECHO_INVENTORY_H
#include "Data.h"
struct inventory {
    // inventory
    sf::Sprite invBar;
    sf::Texture invTex;
    sf::RectangleShape selector; // المربع اللي بيحدد إنت واقف فين

    int selectedSlot = 0; // الخانة المختارة من 0 لـ 5

    void invt_init(float W, float H);
    void invt_update(sf::RenderWindow& window, AppState& currentState);
    void invt_pickup(sf::RenderWindow& window, AppState& currentState);
    void invt_throw(sf::RenderWindow& window, AppState& currentState);
    void invt_draw(sf::RenderWindow& window);
};

#endif //LASTECHO_INVENTORY_H