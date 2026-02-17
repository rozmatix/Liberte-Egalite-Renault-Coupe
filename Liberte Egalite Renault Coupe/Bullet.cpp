#include "Bullet.h"
#include "SpriteBatch.h"
#include <cmath>

static inline float DegToRad(float deg) { return deg * 3.14159265358979323846f / 180.0f; }

void Bullet::Init(const Vector2& start, const Vector2& dir, bool fromEnemyBullet, int pierceExtra)
{
    position = start;
    const float speed = 900.0f;
    velocity = Vector2(dir.x * speed, dir.y * speed);
    fromEnemy = fromEnemyBullet;
    pierceRemaining = fromEnemy ? 0 : std::max(0, pierceExtra);
    hitEnemies.clear();

    // Angle for rendering (optional)
    angleDeg = std::atan2(dir.y, dir.x) * 180.0f / 3.14159265358979323846f;
}

void Bullet::Update(float dt)
{
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;
    life -= dt;
}

void Bullet::Draw(SpriteBatch* batch) const
{
    if (!batch) return;

    if (sprite)
        batch->Add(sprite, position, angleDeg + 90.0f);
    else
        batch->AddCircle(position, radius, SDL_Color{ 0, 0, 0, 255 });
}

bool Bullet::CanHitEnemy(const Enemy* e) const
{
    return hitEnemies.find(e) == hitEnemies.end();
}

void Bullet::RegisterEnemyHit(const Enemy* e)
{
    hitEnemies.insert(e);
}
