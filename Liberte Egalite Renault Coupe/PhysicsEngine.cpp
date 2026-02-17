#include "PhysicsEngine.h"
#include "SoundManager.h"
#include "Enemy.h"
#include "Torch.h"
#include "Bullet.h"
#include <cmath>
#include <algorithm>

// Local helpers (do not depend on class-private methods)
static inline float DegToRad(float deg) { return deg * 3.14159265358979323846f / 180.0f; }
static inline float Dot(const Vector2& a, const Vector2& b) { return a.x * b.x + a.y * b.y; }
static inline float Abs(float v) { return v >= 0.0f ? v : -v; }

static void GetAxesLocal(float angDeg, Vector2 axes[2])
{
    const float rad = DegToRad(angDeg);
    const float cs = std::cos(rad), sn = std::sin(rad);
    axes[0] = Vector2(cs, sn);     // width axis
    axes[1] = Vector2(-sn, cs);    // height axis
}

static float ProjectHalfExtentLocal(const Vector2& size, float angDeg, const Vector2& axis)
{
    const float rad = DegToRad(angDeg);
    const float cs = std::cos(rad), sn = std::sin(rad);
    const Vector2 localX(cs, sn);
    const Vector2 localY(-sn, cs);
    const float hw = size.x * 0.5f;
    const float hh = size.y * 0.5f;
    return Abs(Dot(axis, localX)) * hw + Abs(Dot(axis, localY)) * hh;
}

// World bounds setter
void PhysicsEngine::SetWorldBounds(float x, float y, float w, float h)
{
    worldX = x; worldY = y; worldW = w; worldH = h;
}

// OBB vs OBB SAT
bool PhysicsEngine::OBBvsOBB(const Vector2& c1, const Vector2& size1, float ang1Deg,
                             const Vector2& c2, const Vector2& size2, float ang2Deg)
{
    Vector2 axes[4];
    GetAxesLocal(ang1Deg, &axes[0]);
    GetAxesLocal(ang2Deg, &axes[2]);

    const Vector2 d(c2.x - c1.x, c2.y - c1.y);

    for (int i = 0; i < 4; ++i)
    {
        const Vector2 axis = axes[i];
        const float dist = Abs(Dot(d, axis));
        const float e1 = ProjectHalfExtentLocal(size1, ang1Deg, axis);
        const float e2 = ProjectHalfExtentLocal(size2, ang2Deg, axis);
        if (dist > (e1 + e2)) return false;
    }
    return true;
}

// Circle vs OBB for bullets and airborne torches
static bool CircleVsOBB(const Vector2& circleCenter, float radius,
                        const Vector2& obbCenter, const Vector2& obbSize, float obbAngDeg)
{
    Vector2 axes[2];
    GetAxesLocal(obbAngDeg, axes);

    const Vector2 d(circleCenter.x - obbCenter.x, circleCenter.y - obbCenter.y);
    const float distAx = Abs(Dot(d, axes[0]));
    const float distAy = Abs(Dot(d, axes[1]));
    const float extAx = ProjectHalfExtentLocal(obbSize, obbAngDeg, axes[0]);
    const float extAy = ProjectHalfExtentLocal(obbSize, obbAngDeg, axes[1]);

    return (distAx <= extAx + radius) && (distAy <= extAy + radius);
}

// Circle vs circle helper (for ground hazard overlap)
static inline bool CircleOverlap(const Vector2& c1, float r1, const Vector2& c2, float r2)
{
    const float dx = c2.x - c1.x;
    const float dy = c2.y - c1.y;
    const float rr = r1 + r2;
    return (dx * dx + dy * dy) <= (rr * rr);
}

void PhysicsEngine::Simulate(Scene& scene, float dt)
{
    // Advance bullets
    for (auto& b : scene.bullets) b.Update(dt);

    // Advance torches
    for (auto& t : scene.torches) t.Update(dt);

    // Player–enemy OBB collision: skip completely if player invincible
    if (scene.player)
    {
        const Vector2 pC = scene.player->position;
        const Vector2 pSz = scene.player->HitboxSize();
        const float   pAng = scene.player->AngleDeg();

        for (auto& e : scene.enemies)
        {
            const Vector2 eC = e.position;
            const Vector2 eSz = e.HitboxSize();
            const float   eAng = e.AngleDeg();

            if (PhysicsEngine::OBBvsOBB(pC, pSz, pAng, eC, eSz, eAng))
            {
                // Strict gate: no collision while invincible
                if (scene.player->IsInvincible())
                    continue;

                // Enemy takes fixed 20 via Damage(); only play crash sound when damage applied
                const int enemyApplied = e.Damage(20);
                if (enemyApplied > 0)
                {
                    SoundManager::Instance().PlaySound("crash");
                    if (onEnemyDamaged) onEnemyDamaged(e.position, 20);
                }

                // Player takes damage defined by enemy (no-arg Damage() is player-contact damage)
                scene.player->Damage(e.Damage());

                // Collision kill: direct +10 XP (no orb)
                if (e.current_hp <= 0 && scene.player)
                    scene.player->GainXp(10);
            }
        }
    }

    // Bullet collisions
    for (auto& b : scene.bullets)
    {
        if (b.IsDead()) continue;

        if (!b.fromEnemy)
        {
            // Player bullets → enemies (circle vs OBB)
            for (auto& e : scene.enemies)
            {
                const Vector2 eC = e.position;
                const Vector2 eSz = e.HitboxSize();
                const float   eAng = e.AngleDeg();

                if (CircleVsOBB(b.position, b.radius, eC, eSz, eAng))
                {
                    // Ensure this bullet only applies damage once per enemy
                    if (!b.CanHitEnemy(&e))
                        continue;

                    const int baseDmg = 20;
                    const int dmgBonus = b.powerBonus * 5;
                    const int dmg = baseDmg + dmgBonus;
                    const int applied = e.Damage(dmg);
                    b.RegisterEnemyHit(&e);
                    if (applied > 0 && onEnemyDamaged) onEnemyDamaged(e.position, dmg);

                    // Fire DoT
                    if (b.fireLevel > 0)
                    {
                        const int perTick = 2 * b.fireLevel;
                        e.Ignite(5.0f, perTick);
                    }

                    // Frost slow (10% per level, 5s), cap total slow at 90%
                    if (b.frostLevel > 0)
                    {
                        const float slow = std::min(0.9f, 0.1f * static_cast<float>(b.frostLevel)); // cap at 90%
                        e.ApplyFrost(5.0f, slow);
                    }

                    // Bullet kill: drop XP orb
                    if (e.current_hp <= 0)
                    {
                        XpOrb orb;
                        orb.position = e.position;
                        orb.radius = 14.0f;
                        orb.xpAmount = 10;
                        scene.xpOrbs.push_back(orb);
                    }

                    // Handle pierce: if no pierce remaining, bullet dies; otherwise consume one pierce and keep flying
                    if (b.pierceRemaining <= 0)
                    {
                        b.life = 0.0f;
                        break;
                    }
                    else
                    {
                        b.pierceRemaining -= 1;
                        // Do not break; allow continuing to check next enemies this frame
                        // Optionally, slightly reduce bullet speed after a hit:
                        // b.velocity.x *= 0.95f; b.velocity.y *= 0.95f;
                    }
                }
            }
        }
        else
        {
            // Enemy bullets → player only; ignore if player invincible
            if (scene.player)
            {
                if (scene.player->IsInvincible())
                    continue;

                const Vector2 pC = scene.player->position;
                const Vector2 pSz = scene.player->HitboxSize();
                const float   pAng = scene.player->AngleDeg();

                if (CircleVsOBB(b.position, b.radius, pC, pSz, pAng))
                {
                    scene.player->Damage(15);
                    b.life = 0.0f;
                }
            }
        }
    }

    // Torch interactions
    if (scene.player)
    {
        const Vector2 pC = scene.player->position;
        const Vector2 pSz = scene.player->HitboxSize();
        const float   pAng = scene.player->AngleDeg();

        for (auto& t : scene.torches)
        {
            if (t.IsDead()) continue;

            if (t.IsAirborne())
            {
                // Airborne torch vs player OBB: impact damage and torch disappears
                if (CircleVsOBB(t.position, t.throwRadius, pC, pSz, pAng))
                {
                    // Airborne impact should bypass invincibility gating; request uses ApplyHazardDamage
                    scene.player->ApplyHazardDamage(t.impactDamage);
                    t.dead = true;
                }
            }
            else
            {
                // Ground hazard: tick damage every 0.5s while overlapping
                // Approximate player as a circle using collider radius for overlap
                const float playerRadius = scene.player->ColliderRadius();
                const bool overlapping = CircleOverlap(t.position, t.hazardRadius, pC, playerRadius);

                if (overlapping)
                {
                    t.hazardTickAccum += dt;
                    if (t.hazardTickAccum >= t.hazardTickInterval)
                    {
                        scene.player->ApplyHazardDamage(t.hazardTickDamage);
                        t.hazardTickAccum = 0.0f;
                    }
                }
                else
                {
                    // Reset accumulator when player leaves the hazard
                    t.hazardTickAccum = 0.0f;
                }
            }
        }
    }

    // Burning DoT ticks for enemies
    for (auto& e : scene.enemies)
    {
        if (e.current_hp <= 0) continue;
        if (e.IsBurning())
        {
            e.DecreaseBurnTime(dt);
            e.AddBurnAccum(dt);

            if (e.GetBurnAccum() >= 0.5f)
            {
                e.ResetBurnAccum();

                const int tick = e.GetBurnTickDamage();
                const int applied = e.Damage(tick);
                if (applied > 0 && onEnemyDamaged) onEnemyDamaged(e.position, tick);
            }
        }
    }

    // Clamp player inside world bounds using collider radius
    if (scene.player)
    {
        const float r = scene.player->ColliderRadius();
        const float minX = worldX + r;
        const float maxX = worldX + worldW - r;
        const float minY = worldY + r;
        const float maxY = worldY + worldH - r;

        if (scene.player->position.x < minX) scene.player->position.x = minX;
        else if (scene.player->position.x > maxX) scene.player->position.x = maxX;

        if (scene.player->position.y < minY) scene.player->position.y = minY;
        else if (scene.player->position.y > maxY) scene.player->position.y = maxY;
    }

    // Cleanup bullets
    scene.bullets.erase(
        std::remove_if(scene.bullets.begin(), scene.bullets.end(),
            [](const Bullet& b) { return b.IsDead(); }),
        scene.bullets.end());

    // Cleanup torches
    scene.torches.erase(
        std::remove_if(scene.torches.begin(), scene.torches.end(),
            [](const Torch& t) { return t.IsDead(); }),
        scene.torches.end());
}


