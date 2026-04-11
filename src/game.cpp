#include "Game.h"
#include "Data.h"

Game::Game() : isPaused(false) {}

void Game::init(float W, float H) {
    // 2. تحميل زرار البوز الصغير (علامة الـ ||)
    // pauseTex.loadFromFile("assets/settings/pause.png");
    // pauseBtn.setTexture(pauseTex);
    // pauseBtn.setScale(0.3f, 0.3f);
    // pauseBtn.setPosition(1120.f, 0.f); // في الركن فوق شمال مثلاً


    //buttons settigs
    word_font.loadFromFile("assets/fonts/pixelsix00.ttf");
    pauseMenubuttons.loadFromFile("assets/mainMenu/button.png");
    std::string woords[3]={"RESUME","SETTINGS","QUIT"};

    for (int i = 0; i < 3; i++) {
        buttons[i].setTexture(pauseMenubuttons);
        buttons[i].setOrigin(buttons[i].getTextureRect().width/2, buttons[i].getTextureRect().height/2);

        float yPos = (H / 2.f) - 40.f + (i * 90.f);

        buttons[i].setPosition(sf::Vector2f(W / 2.f, yPos));

        buttons[i].setScale(0.35f,0.35f);

        words[i].setFont(word_font);
        words[i].setString(woords[i]);
        words[i].setCharacterSize(30);

        sf::FloatRect textRect = words[i].getLocalBounds();
        words[i].setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
        words[i].setPosition(buttons[i].getPosition());

    }
    // 3. تحميل اللوحة الخشبية (اللي شيلنا منها الزراير)
    pauseMenuTex.loadFromFile("assets/settings/pauseMenu.png");
    pauseMenuBG.setTexture(pauseMenuTex);
    pauseMenuBG.setScale(2.f, 2.f);
    // سنترة اللوحة في نص الشاشة
    pauseMenuBG.setOrigin(pauseMenuTex.getSize().x / 2.f, pauseMenuTex.getSize().y / 2.f);
    pauseMenuBG.setPosition(W / 2.f, H / 2.f);
}

void Game::update(sf::RenderWindow& window, AppState& currentState) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // منطق زرار البوز
    // 1. منطق الـ Escape لفتح وقفل المنيو
    static bool escPress = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        if (!escPress) {
            isPaused = !isPaused;
            escPress = true;
        }
    } else {
        escPress = false;
    }

        // لو اللعبة مش واقفة، حدث الحركة والوحوش
        if (isPaused) {
            for (int i = 0; i < 3; i++) {
                // فحص هل الماوس فوق الزرار
                if (buttons[i].getGlobalBounds().contains(mousePos)) {
                    words[i].setFillColor(sf::Color::Yellow); // تأثير الـ Hover

                    // فحص الضغط
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                        if (i == 0) isPaused = false;               // Resume
                        if (i == 1) currentState = STATE_SETTINGS;  // Settings
                        if (i == 2) window.close();                 // Quit

                        sf::sleep(sf::milliseconds(250)); // حماية عشان ميفضلش يضغط
                    }
                } else {
                    words[i].setFillColor(sf::Color::White); // رجع اللون للأبيض
                }
        }
            // player.update();
            // enemies.update();
        }
    else {

    }
    }
    void Game::draw(sf::RenderWindow& window) {
        // ارسم عناصر اللعبة هنا الأول (اللاعب، الماب)

        if (isPaused) {
            // رسم اللوحة الخشبية
            window.draw(pauseMenuBG);

            // رسم الزراير والنصوص فوق اللوحة
            for (int i = 0; i < 3; i++) {
                window.draw(buttons[i]);
                window.draw(words[i]);
            }
        }
    }

