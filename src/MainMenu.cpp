    #include "MainMenu.h"
    #include <iostream>

#include "audio.h"
    using namespace std;
    using namespace sf;

    menus mainmenu;
    void MenuStart(RenderWindow& window)
    {
        float W = window.getSize().x;
        float H = window.getSize().y;

        if (!mainmenu.font.loadFromFile("assets/fonts/pixelsix00.ttf"))
            cout << "Error: Font not found!" << endl;

        if (!mainmenu.btnTex.loadFromFile("assets/mainMenu/button.png"))
            cout << "Error: Button Texture not found!" << endl;

        if (!mainmenu.logoTex.loadFromFile("assets/mainmenu/logo2.png"))
            cout << "Error: Logo Texture not found!" << endl;

        if (!mainmenu.bgTex.loadFromFile("assets/mainmenu/mainbackground.png"))
            cout << "Error: Background Texture not found!" << endl;

        // Background — يملى الشاشة كلها
        mainmenu.background.setTexture(mainmenu.bgTex);
        Vector2u bgSize = mainmenu.bgTex.getSize();
        mainmenu.background.setScale(
            W / bgSize.x,
            H / bgSize.y
        );

        // Logo — فوق في المنتصف
        mainmenu.logo.setTexture(mainmenu.logoTex);
        mainmenu.logo.setOrigin(
            mainmenu.logoTex.getSize().x / 2.0f,
            mainmenu.logoTex.getSize().y / 2.0f
        );
        mainmenu.logo.setPosition(W * 0.5f, H * 0.25f);
        mainmenu.logo.setScale(
            (W * 0.4f) / mainmenu.logoTex.getSize().x,
            (W * 0.4f) / mainmenu.logoTex.getSize().x
        );

        // Buttons — متوزعين في النص تحت
        string choices[] = {"START", "LOAD", "SETTINGS", "EXIT"};
        float btnStartY  = H * 0.55f;
        float btnSpacing = H * 0.12f;
        float btnScaleX  = (W * 0.20f) / mainmenu.btnTex.getSize().x;
        float btnScaleY  = (H * 0.15f) / mainmenu.btnTex.getSize().y;
        int   fontSize   = static_cast<int>(H * 0.045f);

        for (int i = 0; i < 4; i++)
        {
            mainmenu.buttonSprites[i].setTexture(mainmenu.btnTex);
            mainmenu.buttonSprites[i].setOrigin(
                mainmenu.btnTex.getSize().x / 2.0f,
                mainmenu.btnTex.getSize().y / 2.0f
            );
            mainmenu.buttonSprites[i].setPosition(W * 0.5f, btnStartY + (i * btnSpacing));
            mainmenu.buttonSprites[i].setScale(btnScaleX, btnScaleY);

            mainmenu.options[i].setFont(mainmenu.font);
            mainmenu.options[i].setString(choices[i]);
            mainmenu.options[i].setCharacterSize(fontSize);


            mainmenu.options[i].setLetterSpacing(1.5f);
            mainmenu.options[i].setOutlineColor(Color::Black);
            mainmenu.options[i].setOutlineThickness(2.0f);

            FloatRect textRect = mainmenu.options[i].getLocalBounds();
            mainmenu.options[i].setOrigin(
                textRect.left + textRect.width  / 2.0f,
                textRect.top  + textRect.height / 2.0f
            );
            mainmenu.options[i].setPosition(mainmenu.buttonSprites[i].getPosition());
            mainmenu.options[i].setFillColor(Color::White);
        }
        audioManager.playMusic("menu");
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
                    if (i == 0) {
                        currentState = STATE_PLAYING;
                        audioManager.playMusic("game");
                    }
                    if (i == 1) currentState = STATE_LOAD;
                    if (i == 2) {
                        currentState = STATE_SETTINGS;
                        last_state = STATE_MENU; // خزن إننا كنا في المنيو
                        gState.currentState = STATE_SETTINGS;
                        sf::sleep(sf::milliseconds(250));
                    }
                    if (i == 3) window.close();
                }
            }
            else
            {
                mainmenu.options[i].setFillColor(Color(Color::White));
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