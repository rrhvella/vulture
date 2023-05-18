#include "./systems.hpp"

#include "raylib.h"
#include "algorithm"

#include "./math-helpers.hpp"

using namespace std;

void KeepInScreenBounds(Position &p, const CircleRigidBody &c)
{
    p.x = Clamp(p.x, c.radius, GetScreenWidth() - c.radius);
    p.y = Clamp(p.y, c.radius, GetScreenHeight() - c.radius);
}

void RenderCircle(const Position &p, const CircleRigidBody &cb, const RenderInfo &cr)
{
    DrawCircle(p.x, p.y, cb.radius, cr.color);
}

void DestroyAllOutsideScreen(iter &it, const Position *p, const CircleRigidBody *c)
{
    const auto sw = GetScreenWidth();
    const auto sh = GetScreenHeight();

    for (auto i : it)
    {
        const auto d = c[i].radius * 2;
        
        if(
            !WithinBounds(p[i].x, 0 - d, sw + d) ||
            !WithinBounds(p[i].y, 0 - d, sh + d)
        ) 
        {
            it.entity(i).destruct();
        }
    }
}

void Move(iter &it, size_t index, Position &p, const Velocity &v)
{
    p.x += v.x * it.delta_time();
    p.y += v.y * it.delta_time();
}

void IntensifySpawner(EnemySpawner& spawner) {
    spawner.maxSpawns += 0.2f;

    spawner.likelihoodOfSpawning = min(spawner.likelihoodOfSpawning + 10, 95);

    spawner.minEnemySpeed += 10;
    spawner.maxEnemySpeed += 20;
}

void BlinkDamaged(iter &it, size_t index, Player &p) {
    auto playerEntity = it.entity(index);
    auto isInvisible = (int)(p.damageCooldownRemaining / 0.2f) % 2 == 1;

    if(isInvisible) {
        playerEntity.add<Invisible>();
    } else if(!isInvisible) {
        playerEntity.remove<Invisible>();
    }

}

void CooldownDamage(iter &it, size_t index, Player &p) {
    auto playerEntity = it.entity(index);
    p.damageCooldownRemaining -= it.delta_time();
    
    if(p.damageCooldownRemaining <= 0) {        
        playerEntity.remove<Damaged>();
    }
}
