#include "Game.h"
#include "Data.h"

Game::Game() : isPaused(false) {}

void Game::init(float W, float H) {
    // 2. تحميل زرار البوز الصغير (علامة الـ ||)
    pauseTex.loadFromFile("assets/settings/pause.png");
    pauseBtn.setTexture(pauseTex);
    pauseBtn.setScale(0.3f, 0.3f);
    pauseBtn.setPosition(1120.f, 0.f); // في الركن فوق شمال مثلاً

    // 3. تحميل اللوحة الخشبية (اللي شيلنا منها الزراير)
    pauseMenuTex.loadFromFile("assets/settings/pauseMenu.png");
    pauseMenuBG.setTexture(pauseMenuTex);
    // سنترة اللوحة في نص الشاشة
    pauseMenuBG.setOrigin(pauseMenuTex.getSize().x / 2.f, pauseMenuTex.getSize().y / 2.f);
    pauseMenuBG.setPosition(W / 2.f, H / 2.f);
}

void Game::update(sf::RenderWindow& window, AppState& currentState) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // منطق زرار البوز
    if (pauseBtn.getGlobalBounds().contains(mousePos) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        isPaused = !isPaused;
        sf::sleep(sf::milliseconds(250)); // عشان ميفضلش يلقط الضغطة كذا مرة
    }

    // لو اللعبة مش واقفة، حدث الحركة والوحوش
    if (!isPaused) {
        // player.update();
        // enemies.update();
    }
}

void Game::draw(sf::RenderWindow& window) {
    // رسم اللعبة الأساسية دايماً
    // player.draw(window);

    // رسم زرار البوز
    window.draw(pauseBtn);

    // لو واخدين بوز، ارسم اللوحة الخشبية "فوق" كل حاجة
    if (isPaused) {
        window.draw(pauseMenuBG);
        // هنا ممكن ترسم نصوص زي "Paused" أو زراير الرجوع
    }
}
