#include "TheLich.h"
#include "healthbar.h"
#include <cmath>

TheLich lich;

void resetLichSpells() {
    float spellStartTimes[4] = { 2.0f, 4.0f, 6.0f, 8.0f };
    for (int i = 0; i < 4; i++) {
        lich.spells[i].chargeTime = spellStartTimes[i];
        lich.spells[i].chargeTimer = 0.f;
        lich.spells[i].warningTimer = 0.f;
        lich.spells[i].frozenTimer = 0.f;
        lich.spells[i].damageTimer = 0.f;
        lich.spells[i].isCharged = false;
        lich.spells[i].showWarning = false;
        lich.spells[i].hitboxFrozen = false;
        lich.spells[i].isFinished = false;
        lich.spells[i].currFrame = 0;
        lich.spells[i].warnAlpha = 50.f;
    }
}

void initLich(sf::Vector2f spawnPos) {
    lich.position = spawnPos;
    lich.hp = lich.maxHp;

    // 1. تكبير حجم الليتش عشان الهيبة
    lich.sprite.setScale(1.8f, 1.8f);

    // تحميل صور الأنميشن
    lich.idleTex[L_SOUTH].loadFromFile("assets/thelich/thelichidle/idlesouth.png");
    lich.idleTex[L_WEST].loadFromFile("assets/thelich/thelichidle/idlewest.png");
    lich.idleTex[L_EAST].loadFromFile("assets/thelich/thelichidle/idleeast.png");
    lich.idleTex[L_NORTH].loadFromFile("assets/thelich/thelichidle/idlenorth.png");

    lich.walkTex[L_SOUTH].loadFromFile("assets/thelich/thelichwalk/walksouth.png");
    lich.walkTex[L_WEST].loadFromFile("assets/thelich/thelichwalk/walkwest.png");
    lich.walkTex[L_EAST].loadFromFile("assets/thelich/thelichwalk/walkeast.png");
    lich.walkTex[L_NORTH].loadFromFile("assets/thelich/thelichwalk/walknorth.png");

    lich.attackTex[L_SOUTH].loadFromFile("assets/thelich/thelichfireball/fbsouth.png");
    lich.attackTex[L_WEST].loadFromFile("assets/thelich/thelichfireball/fbwest.png");
    lich.attackTex[L_EAST].loadFromFile("assets/thelich/thelichfireball/fbeast.png");
    lich.attackTex[L_NORTH].loadFromFile("assets/thelich/thelichfireball/fbnorth.png");

    lich.greenFireballTex.loadFromFile("assets/thelich/thelichweapons/GREENFIREBALL.png");

    lich.sprite.setTexture(lich.idleTex[L_SOUTH]);
    lich.sprite.setTextureRect(sf::IntRect(0, 0, 120, 120));
    lich.sprite.setOrigin(60.f, 60.f);
    lich.sprite.setPosition(lich.position);

    resetLichSpells();

    int fw = lich.greenFireballTex.getSize().x / 6;
    int fh = lich.greenFireballTex.getSize().y;

    for (int i = 0; i < 4; i++) {
        // 2. إعداد المربع التحذيري (Warning Box)
        lich.spells[i].warningBox.setSize({ 110.f, 110.f });
        lich.spells[i].warningBox.setOrigin(55.f, 55.f);
        lich.spells[i].warningBox.setOutlineThickness(3.f);
        lich.spells[i].warningBox.setOutlineColor(sf::Color::Red);
        lich.spells[i].warningBox.setFillColor(sf::Color(255, 0, 0, 40)); // أحمر شفاف جداً

        // 3. تصغير حجم الفاير بول عشان تناسب المربع
        lich.spells[i].visualSprite.setTexture(lich.greenFireballTex);
        lich.spells[i].visualSprite.setTextureRect(sf::IntRect(0, 0, fw, fh));
        lich.spells[i].visualSprite.setOrigin(fw / 2.f, fh / 2.f);
        lich.spells[i].visualSprite.setScale(0.5f, 0.5f);
    }
}

void updateLichAnimation(float dt, sf::Vector2f dir) {
    if (std::abs(dir.x) > std::abs(dir.y))
        lich.currDir = (dir.x > 0) ? L_EAST : L_WEST;
    else
        lich.currDir = (dir.y > 0) ? L_SOUTH : L_NORTH;

    switch (lich.currState) {
        case L_IDLE:   lich.sprite.setTexture(lich.idleTex[lich.currDir]);   lich.totalFrames = 8; break;
        case L_WALK:   lich.sprite.setTexture(lich.walkTex[lich.currDir]);   lich.totalFrames = 6; break;
        case L_ATTACK: lich.sprite.setTexture(lich.attackTex[lich.currDir]); lich.totalFrames = 6; break;
    }

    lich.animTimer += dt;
    if (lich.animTimer >= 0.1f) {
        lich.animTimer = 0.f;
        lich.currFrame = (lich.currFrame + 1) % lich.totalFrames;
    }
    lich.sprite.setTextureRect(sf::IntRect(lich.currFrame * 120, 0, 120, 120));
}

void updateLich(float dt, sf::Vector2f playerPos, sf::FloatRect playerBounds) {
    if (lich.isDead) return;

    if (lich.isReborn) {
        lich.rebornTimer += dt;
        if (lich.rebornTimer >= lich.rebornDuration) {
            lich.isReborn = false; lich.alive = true; lich.phase = PHASE_2;
            lich.hp = lich.maxHp * 1.5f; lich.speed = 110.f;
            resetLichSpells();
        }
        return;
    }

    if (!lich.alive) return;

    sf::Vector2f diff = playerPos - lich.position;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    sf::Vector2f dir = (dist != 0) ? diff / dist : sf::Vector2f(0, 1);

    if (dist <= lich.attackRange) lich.currState = L_ATTACK;
    else if (dist <= lich.idleRange) {
        lich.currState = L_WALK;
        lich.position += dir * lich.speed * dt;
    } else lich.currState = L_IDLE;

    lich.sprite.setPosition(lich.position);
    updateLichAnimation(dt, dir);

    bool allSpellsDone = true;
    for (int i = 0; i < 4; i++) {
        if (lich.spells[i].isFinished) continue;
        allSpellsDone = false;

        if (!lich.spells[i].isCharged) {
            lich.spells[i].chargeTimer += dt;
            lich.spells[i].showWarning = true;

            // 4. جعل المربع يظهر أمام الليتش في اتجاه اللاعب (على مسافة 120 بكسل)
            sf::Vector2f attackSpot = lich.position + (dir * 130.f);
            lich.spells[i].warningBox.setPosition(attackSpot);

            // وميض التحذير
            lich.spells[i].warnAlpha += (lich.spells[i].warnFadeIn ? 800 : -800) * dt;
            if (lich.spells[i].warnAlpha >= 255) lich.spells[i].warnFadeIn = false;
            if (lich.spells[i].warnAlpha <= 50)  lich.spells[i].warnFadeIn = true;
            lich.spells[i].warningBox.setOutlineColor(sf::Color(255, 0, 0, (sf::Uint8)lich.spells[i].warnAlpha));

            if (lich.spells[i].chargeTimer >= lich.spells[i].chargeTime) {
                lich.spells[i].isCharged = true;
                lich.spells[i].showWarning = false;
                lich.spells[i].hitboxFrozen = true;
                lich.spells[i].visualSprite.setPosition(lich.spells[i].warningBox.getPosition());
            }
        }
        else if (lich.spells[i].hitboxFrozen) {
            lich.spells[i].frozenTimer += dt;

            // أنميشن النار الخضراء
            lich.spells[i].animTimer += dt;
            if (lich.spells[i].animTimer >= 0.07f) {
                lich.spells[i].animTimer = 0.f;
                lich.spells[i].currFrame = (lich.spells[i].currFrame + 1) % 6;
            }
            int fw = lich.greenFireballTex.getSize().x / 6;
            lich.spells[i].visualSprite.setTextureRect(sf::IntRect(lich.spells[i].currFrame * fw, 0, fw, lich.greenFireballTex.getSize().y));

            if (lich.spells[i].visualSprite.getGlobalBounds().intersects(playerBounds)) {
                lich.spells[i].damageTimer += dt;
                if (lich.spells[i].damageTimer >= 0.4f) {
                    damaging(15); lich.spells[i].damageTimer = 0.f;
                }
            }
            if (lich.spells[i].frozenTimer >= 1.3f) lich.spells[i].isFinished = true;
        }
    }
    if (allSpellsDone) resetLichSpells();
}

void drawLich(sf::RenderWindow& window) {
    if (lich.isDead || lich.isReborn) return;

    // رسم التحذير أولاً عشان يبقى تحت الليتش واللاعب لو وقفوا عليه
    for (int i = 0; i < 4; i++) {
        if (lich.spells[i].showWarning) window.draw(lich.spells[i].warningBox);
    }

    window.draw(lich.sprite);

    // رسم النار الخضراء فوق كل حاجة عشان تبان قوتها
    for (int i = 0; i < 4; i++) {
        if (lich.spells[i].hitboxFrozen) window.draw(lich.spells[i].visualSprite);
    }
}

void lichTakeDamage(float amount) {
    if (!lich.alive || lich.isReborn) return;
    lich.hp -= amount;
    if (lich.hp <= 0) {
        lich.alive = false;
        if (lich.phase == PHASE_1) { lich.isReborn = true; lich.rebornTimer = 0.f; }
        else lich.isDead = true;
    }
}