#pragma once

#include "memory"
#include "functional"
#include "./flecs.h"
#include "./components.hpp"

using namespace std;
using namespace flecs;

function<void(iter &it)> CreateDraw(world &ecs, const int *highScore);

function<void(iter &it, size_t index, Velocity &v, const Position &p, const CircleRigidBody &c, const Player &pl)> CreateControlPlayer(
    entity &bulletPrefab);

function<void(iter &it, EnemySpawner *e)> CreateSpawnEnemies(world &ecs, entity &enemyPrefab);

function<void(iter &it, const Position *ep, const CircleRigidBody *ec)> CheckAllEnemyCollisions(world &ecs);