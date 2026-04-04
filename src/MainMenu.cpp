#include "MainMenu.h"
#include <iostream>
using namespace std;
using namespace sf;

menus mainmenu;

void MenuStart(RenderWindow& window)
{
    if (!mainmenu.font.loadFromFile("assets/fonts/pixelsix00.ttf"))
        cout << "Error: Font not found!" << endl;

    if (!mainmenu.btnTex.loadFromFile("assets/mainMenu/button.png"))
        cout << "Error: Button Texture not found!" << endl;

    if (!mainmenu.logoTex.loadFromFile("assets/mainmenu/logo.png"))
        cout << "Error: Logo Texture not found!" << endl;

    if (!mainmenu.bgTex.loadFromFile("assets/mainmenu/mainbackground.png"))
        cout << "Error: Background Texture not found!" << endl;

    // Background
    mainmenu.background.setTexture(mainmenu.bgTex);
    Vector2u winSize = window.getSize();
    Vector2u bgSize  = mainmenu.bgTex.getSize();
    mainmenu.background.setScale(
        (float)winSize.x / bgSize.x,
        (float)winSize.y / bgSize.y
    );

    // Logo
    mainmenu.logo.setTexture(mainmenu.logoTex);
    mainmenu.logo.setOrigin(
        mainmenu.logoTex.getSize().x / 2.0f,
        mainmenu.logoTex.getSize().y / 2.0f
    );
    mainmenu.logo.setPosition(window.getSize().x / 2.0f, 200);
    mainmenu.logo.setScale(0.5f, 0.5f);

    // Buttons
    string choices[] = {"START", "LOAD", "SETTINGS", "EXIT"};
    for (int i = 0; i < 4; i++)
    {
        mainmenu.buttonSprites[i].setTexture(mainmenu.btnTex);
        mainmenu.buttonSprites[i].setOrigin(
            mainmenu.btnTex.getSize().x / 2.0f,
            mainmenu.btnTex.getSize().y / 2.0f
        );
        mainmenu.buttonSprites[i].setPosition(window.getSize().x / 2.0f, 450 + (i * 120));
        mainmenu.buttonSprites[i].setScale(0.45f, 0.45f);

        mainmenu.options[i].setFont(mainmenu.font);
        mainmenu.options[i].setString(choices[i]);
        mainmenu.options[i].setCharacterSize(40);
        FloatRect textRect = mainmenu.options[i].getLocalBounds();
        mainmenu.options[i].setOrigin(
            textRect.left + textRect.width  / 2.0f,
            textRect.top  + textRect.height / 2.0f
        );
        mainmenu.options[i].setPosition(mainmenu.buttonSprites[i].getPosition());
        mainmenu.options[i].setFillColor(Color::White);
    }
}

void MenuUpdate(RenderWindow& window, AppState& currentState)
{
    if (currentState != STATE_MENU) return;

    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

    for (int i = 0; i < 4; i++)
    {
        if (mainmenu.buttonSprites[i].getGlobalBounds().contains(mousePos))
        {
            mainmenu.options[i].setFillColor(Color::Yellow);
            mainmenu.buttonSprites[i].setColor(Color::White);

            if (Mouse::isButtonPressed(Mouse::Left))
            {
                if (i == 0) currentState = STATE_PLAYING;
                if (i == 1) currentState = STATE_LOAD;
                if (i == 2) currentState = STATE_SETTINGS;
                if (i == 3) window.close();
            }
        }
        else
        {
            mainmenu.options[i].setFillColor(Color::White);
            mainmenu.buttonSprites[i].setColor(Color(200, 200, 200, 180));
        }
    }
}

void MenuDraw(RenderWindow& window, AppState currentState)
{
    if (currentState != STATE_MENU) return;

    window.draw(mainmenu.background);
    window.draw(mainmenu.logo);
    for (int i = 0; i < 4; i++)
    {
        window.draw(mainmenu.buttonSprites[i]);
        window.draw(mainmenu.options[i]);
    }
}