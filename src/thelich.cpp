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

float lichAnimTimer = 0.f;
int lichFrame = 0;

// ================= INIT =================
void initLich(sf::Vector2f pos) {
    lichPos = pos;

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

    lichProjectile.shape.setRadius(10.f);
    lichProjectile.shape.setOrigin(10.f,10.f);
    lichProjectile.speed = 420.f;
    lichProjectile.active = false;
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
    sf::Vector2f perp(-dir.y, dir.x);

    // ===== DIRECTION =====
    if (std::abs(dir.x) > std::abs(dir.y))
        lichDir = (dir.x > 0) ? L_EAST : L_WEST;
    else
        lichDir = (dir.y > 0) ? L_SOUTH : L_NORTH;

    float speed = 140.f;

    // ===== AI =====
    if (dist > 450.f) {
        lichState = L_IDLE;
    }
    else if (dist > 200.f) {
        lichState = L_WALK;
        lichPos += dir * speed * dt;
    }
    else {
        lichState = L_ATTACK;
        lichPos += perp * 60.f * dt;

        // 🔥 FIXED SHOOT (NO ONE-BULLET BUG)
        static float timer = 0.f;
        timer += dt;

        if (timer >= 1.2f) {
            shoot(playerPos);
            timer = 0.f;
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

    // ===== PROJECTILE =====
    if (lichProjectile.active) {
        lichProjectile.shape.move(lichProjectile.dir * lichProjectile.speed * dt);

        // ✔ FIXED HITBOX (NO SPRITE BUG)
        sf::FloatRect p(
            player.pos.x - 20.f,
            player.pos.y - 20.f,
            40.f,
            40.f
        );

        if (lichProjectile.shape.getGlobalBounds().intersects(p)) {
            hitPlayer(20, lichProjectile.dir);
            lichProjectile.active = false;
        }
    }
}

// ================= DRAW =================
void drawLich(sf::RenderWindow& window) {
    window.draw(lichSprite);

    if (lichProjectile.active)
        window.draw(lichProjectile.shape);
}