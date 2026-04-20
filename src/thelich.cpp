#include "TheLich.h"
#include "player.h"
#include <cmath>

extern Player player;

// ================= GLOBAL =================
sf::Vector2f lichPos;
LichState lichState = L_IDLE;
LichDir lichDir = L_SOUTH;

sf::Sprite lichSprite;
sf::Texture lichIdle[4];
sf::Texture lichWalk[4];
sf::Texture lichAttack[4];

Projectile lichProjectile;

// نظام التحذير
sf::RectangleShape lichWarningBeam;
float lichWarningTimer = 0.f;
const float WARNING_DURATION = 0.8f;
bool isWarning = false;

// متغيرات التحكم (القيم اللي طلبتها)
const float SHOOT_COOLDOWN = 5.0f;       // الوقت بين الضربات
const float MAX_PROJECTILE_DIST = 300.f; // مدى الكورة
const float LICH_SPEED = 90.f;           // سرعة مشي الوحش
const float BALL_SPEED = 300.f;          // سرعة الكورة

float lichAnimTimer = 0.f;
int lichFrame = 0;

// ================= INIT =================
void initLich(sf::Vector2f pos) {
    lichPos = pos;

    // تحميل الصور
    lichIdle[L_SOUTH].loadFromFile("assets/thelich/thelichidle/idlesouth.png");
    lichIdle[L_NORTH].loadFromFile("assets/thelich/thelichidle/idlenorth.png");
    lichIdle[L_EAST].loadFromFile("assets/thelich/thelichidle/idleeast.png");
    lichIdle[L_WEST].loadFromFile("assets/thelich/thelichidle/idlewest.png");

    lichWalk[L_SOUTH].loadFromFile("assets/thelich/thelichwalk/walksouth.png");
    lichWalk[L_NORTH].loadFromFile("assets/thelich/thelichwalk/walknorth.png");
    lichWalk[L_EAST].loadFromFile("assets/thelich/thelichwalk/walkeast.png");
    lichWalk[L_WEST].loadFromFile("assets/thelich/thelichwalk/walkwest.png");

    lichAttack[L_SOUTH].loadFromFile("assets/thelich/thelichfireball/fbsouth.png");
    lichAttack[L_NORTH].loadFromFile("assets/thelich/thelichfireball/fbnorth.png");
    lichAttack[L_EAST].loadFromFile("assets/thelich/thelichfireball/fbeast.png");
    lichAttack[L_WEST].loadFromFile("assets/thelich/thelichfireball/fbwest.png");

    lichSprite.setTexture(lichIdle[L_SOUTH]);
    lichSprite.setTextureRect(sf::IntRect(0,0,120,120));
    lichSprite.setOrigin(60.f,60.f);
    lichSprite.setScale(1.8f,1.8f);

    // إعداد شعاع التحذير
    lichWarningBeam.setSize({ 350.f, 15.f });
    lichWarningBeam.setOrigin(-5.f, 7.5f);
    lichWarningBeam.setFillColor(sf::Color(255, 0, 0, 150));

    // إعداد الكورة
    lichProjectile.shape.setRadius(12.f);
    lichProjectile.shape.setOrigin(12.f,12.f);
    lichProjectile.speed = BALL_SPEED;
    lichProjectile.active = false;
    lichProjectile.shape.setFillColor(sf::Color::Green);
}

// ================= PLAYER HIT =================
void hitPlayer(int dmg, sf::Vector2f dir) {
    if (player.isInvincible) return;
    player.hp -= dmg;
    if (player.hp < 0) player.hp = 0;
    player.currentState = HURT;
    player.hurt_timer = 0.5f;
    player.isInvincible = true;
    player.pos += dir * 25.f;
}

// ================= SHOOT =================
void shoot(sf::Vector2f target) {
    sf::Vector2f d = target - lichPos;
    float len = std::sqrt(d.x*d.x + d.y*d.y);
    if (len != 0) d /= len;

    lichProjectile.dir = d;
    lichProjectile.shape.setPosition(lichPos);
    lichProjectile.active = true;
}

// ================= UPDATE =================
void updateLich(float dt, sf::Vector2f playerPos, sf::FloatRect playerBounds) {
    sf::Vector2f diff = playerPos - lichPos;
    float dist = std::sqrt(diff.x*diff.x + diff.y*diff.y);
    sf::Vector2f dir = (dist != 0) ? diff / dist : sf::Vector2f(0,1);

    // تحديد الاتجاه
    if (std::abs(dir.x) > std::abs(dir.y))
        lichDir = (dir.x > 0) ? L_EAST : L_WEST;
    else
        lichDir = (dir.y > 0) ? L_SOUTH : L_NORTH;

    // ===== AI LOGIC =====
    if (dist > 450.f) {
        lichState = L_IDLE;
        isWarning = false;
    }
    else if (dist > 250.f && !isWarning) {
        lichState = L_WALK;
        lichPos += dir * LICH_SPEED * dt;
    }
    else {
        // إدارة الحالة (Idle أثناء الانتظار، Attack أثناء التحذير)
        if (!isWarning) {
            lichState = L_IDLE;
        } else {
            lichState = L_ATTACK;
        }

        static float shootCooldownTimer = 0.f;
        shootCooldownTimer += dt;

        if (shootCooldownTimer >= SHOOT_COOLDOWN) {
            isWarning = true;
            lichWarningTimer = 0.f;
            shootCooldownTimer = 0.f;
            lichFrame = 0; // تصفير الفريم لضمان بداية الأنميشن صح
        }

        if (isWarning) {
            lichWarningTimer += dt;
            lichWarningBeam.setPosition(lichPos);
            float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;
            lichWarningBeam.setRotation(angle);

            // وميض الشعاع
            int alpha = (int(lichWarningTimer * 12) % 2 == 0) ? 40 : 200;
            lichWarningBeam.setFillColor(sf::Color(255, 0, 0, alpha));

            if (lichWarningTimer >= WARNING_DURATION) {
                shoot(playerPos);
                isWarning = false;
                lichState = L_IDLE; // العودة للثبات بعد الضرب
            }
        }
    }

    lichSprite.setPosition(lichPos);

    // ===== ANIMATION =====
    lichAnimTimer += dt;
    if (lichAnimTimer > 0.1f) {
        lichAnimTimer = 0.f;
        lichFrame = (lichFrame + 1) % 6;
    }

    if (lichState == L_IDLE)
        lichSprite.setTexture(lichIdle[lichDir]);
    else if (lichState == L_WALK)
        lichSprite.setTexture(lichWalk[lichDir]);
    else
        lichSprite.setTexture(lichAttack[lichDir]);

    lichSprite.setTextureRect(sf::IntRect(lichFrame*120,0,120,120));

    // ===== PROJECTILE (التحكم في المدى) =====
    if (lichProjectile.active) {
        lichProjectile.shape.move(lichProjectile.dir * lichProjectile.speed * dt);

        sf::Vector2f dVec = lichProjectile.shape.getPosition() - lichPos;
        float traveled = std::sqrt(dVec.x*dVec.x + dVec.y*dVec.y);

        if (traveled > MAX_PROJECTILE_DIST) {
            lichProjectile.active = false;
        }

        sf::FloatRect p(player.pos.x - 20.f, player.pos.y - 20.f, 40.f, 40.f);
        if (lichProjectile.shape.getGlobalBounds().intersects(p)) {
            hitPlayer(20, lichProjectile.dir);
            lichProjectile.active = false;
        }
    }
}

// ================= DRAW =================
void drawLich(sf::RenderWindow& window) {
    if (isWarning) window.draw(lichWarningBeam);
    window.draw(lichSprite);
    if (lichProjectile.active) window.draw(lichProjectile.shape);
}