#pragma once
#include <SFML/Graphics.hpp>
#include "Data.h"
using namespace sf;

struct menus
{
    Font font;
    Text options[4];
    Sprite buttonSprites[4];
    Texture btnTex;
    Sprite logo;
    Texture logoTex;
    Sprite background;
    Texture bgTex;
};

extern menus mainmenu;

void MenuStart(RenderWindow& window);
void MenuUpdate(RenderWindow& window, AppState& currentState);
void MenuDraw(RenderWindow& window, AppState currentState);