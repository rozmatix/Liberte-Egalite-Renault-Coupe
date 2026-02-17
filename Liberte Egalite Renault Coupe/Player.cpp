#include "Player.h"
#include <iostream>
#include <cmath>

static inline float DegToRad(float deg) { return deg * 3.14159265358979323846f / 180.0f; }

Player::Player()
{
    position = Vector2(300, 300);
    angle = -90.0f;
}

void Player::Init(Sprite* spr)
{
    sprite = spr;
}

void Player::HandleInput(bool keyState[], float dt)
{
    if (keyState[SDL_SCANCODE_A]) angle -= turnSpeed * dt;
    if (keyState[SDL_SCANCODE_D]) angle += turnSpeed * dt;

    const float rad = DegToRad(angle);
    const Vector2 forward(std::cos(rad), std::sin(rad));

    const bool w = keyState[SDL_SCANCODE_W] != 0;
    const bool s = keyState[SDL_SCANCODE_S] != 0;

    if (w && !s)
    {
        speedScalar += accel * dt;
        if (speedScalar > maxSpeed) speedScalar = maxSpeed;
    }
    else if (s && !w)
    {
        if (speedScalar > 0.0f)
        {
            speedScalar -= brakeAccel * dt;
            if (speedScalar < 0.0f) speedScalar = 0.0f;
        }
        else
        {
            speedScalar -= reverseAccel * dt;
            if (speedScalar < -reverseMax) speedScalar = -reverseMax;
        }
    }
    else
    {
        if (speedScalar > 0.0f)
            speedScalar = std::max(0.0f, speedScalar - coastDrag * dt);
        else if (speedScalar < 0.0f)
            speedScalar = std::min(0.0f, speedScalar + coastDrag * dt);
    }

    position.x += forward.x * speedScalar * dt;
    position.y += forward.y * speedScalar * dt;
}

void Player::Update(float dt)
{
    if (invincible)
    {
        invincibleTimer -= dt;
        if (invincibleTimer <= 0.0f)
        {
            invincible = false;
            invincibleTimer = 0.0f;
        }
    }

    if (fireCooldown > 0.0f)
        fireCooldown = std::max(0.0f, fireCooldown - dt);
}

void Player::Draw(SpriteBatch* batch)
{
    if (!batch) return;

    if (sprite)
    {
        const float visualAngleOffset = -90.0f;
        const float renderAngle = angle + visualAngleOffset + 180.0f;
        batch->Add(sprite, position, renderAngle);

        const bool debugHitbox = true;
        if (debugHitbox)
        {
            const Vector2 size = HitboxSize();
            batch->AddOrientedRectDebug(position, size, AngleDeg(), SDL_Color{255, 0, 0, 255});
        }
    }

    if (gunSprite && sprite)
    {
        float mx = 0.0f, my = 0.0f;
        SDL_GetMouseState(&mx, &my);
        const Vector2 mouseWorld = batch->ScreenToWorld(mx, my);

        const float sw = sprite->source.w;
        const float sh = sprite->source.h;
        const Vector2 localMount(sw * gunOffsetXPct, sh * gunOffsetYPct);

        const float rad = DegToRad(angle);
        const float cs = std::cos(rad), sn = std::sin(rad);
        const Vector2 mountOffset(localMount.x * cs - localMount.y * sn,
                                  localMount.x * sn + localMount.y * cs);

        const Vector2 gunPos(position.x + mountOffset.x, position.y + mountOffset.y);

        const Vector2 toMouse(mouseWorld.x - gunPos.x, mouseWorld.y - gunPos.y);
        const float aimAngle = std::atan2(toMouse.y, toMouse.x) * 180.0f / 3.14159265358979323846f;

        const float gunRenderAngle = aimAngle + 90.0f;
        batch->Add(gunSprite, gunPos, gunRenderAngle);
    }
}

bool Player::Damage(int dmg)
{
    if (invincible) return false;
    current_hp -= dmg;
    if (current_hp < 0) current_hp = 0;
    invincible = true;
    invincibleTimer = 1.0f;
    return true;
}

void Player::ApplyHazardDamage(int dmg)
{
    if (dmg <= 0) return;
    current_hp -= dmg;
    if (current_hp < 0) current_hp = 0;
}

void Player::Reset()
{
    current_hp = 100;
    max_hp = 100;
    position = Vector2(300, 300);
    angle = -90.0f;
    invincible = false;
    invincibleTimer = 0.0f;
    speedScalar = 0.0f;
    fireCooldown = 0.0f;
}

float Player::ColliderRadius() const
{
    if (sprite)
    {
        float w = sprite->source.w;
        float h = sprite->source.h;
        float r = 0.5f * (w > h ? w : h);
        return r * 0.5f;
    }
    return 24.0f;
}

Vector2 Player::HitboxSize() const
{
    if (sprite)
    {
        const float w = sprite->source.w * 0.9f;
        const float h = sprite->source.h * 0.7f;
        return Vector2(w, h);
    }
    return Vector2(48.0f, 24.0f);
}

float Player::AngleDeg() const
{
    return angle;
}

Vector2 Player::GetGunMuzzleWorld(const Vector2& mouseWorld) const
{
    if (!sprite || !gunSprite)
        return position;

    const float sw = sprite->source.w;
    const float sh = sprite->source.h;
    const Vector2 localMount(sw * gunOffsetXPct, sh * gunOffsetYPct);

    const float rad = DegToRad(angle);
    const float cs = std::cos(rad), sn = std::sin(rad);
    const Vector2 mountOffset(localMount.x * cs - localMount.y * sn,
                              localMount.x * sn + localMount.y * cs);
    const Vector2 basePos(position.x + mountOffset.x, position.y + mountOffset.y);

    const Vector2 toMouse(mouseWorld.x - basePos.x, mouseWorld.y - basePos.y);
    float len = std::sqrt(toMouse.x * toMouse.x + toMouse.y * toMouse.y);
    if (len > 0.0001f)
    {
        const Vector2 dir(toMouse.x / len, toMouse.y / len);
        const float muzzleForward = std::max(gunSprite->source.w, gunSprite->source.h) * 0.2f;
        return Vector2(basePos.x + dir.x * muzzleForward, basePos.y + dir.y * muzzleForward);
    }
    return basePos;
}

// XP/Level mechanics (ensure no HP gating)
void Player::GainXp(int amount)
{
    if (amount <= 0) return;
    xp += amount;
    while (xp >= xpRequired)
    {
        xp -= xpRequired;
        LevelUp();
    }
}

void Player::RecomputeXpRequired()
{
    const double base = 50.0;
    const double growth = 1.35;
    xpRequired = static_cast<int>(std::round(base * std::pow(growth, std::max(0, level - 1))));
}

void Player::LevelUp()
{
    level += 1;
    RecomputeXpRequired();
    // Power-up menu trigger handled in Gameplay::Update
}