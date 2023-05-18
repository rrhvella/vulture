#pragma once

#include "./components.hpp"
#include "./flecs.h"

using namespace flecs;

void KeepInScreenBounds(Position &p, const CircleRigidBody &c);

void RenderCircle(const Position &p, const CircleRigidBody &cb, const RenderInfo &cr);

void DestroyAllOutsideScreen(iter &it, const Position *p, const CircleRigidBody *c);

void Move(iter &it, size_t index, Position &p, const Velocity &v);

void IntensifySpawner(EnemySpawner& spawner);

void BlinkDamaged(iter &it, size_t index, Player &p);

void CooldownDamage(iter &it, size_t index, Player &p);