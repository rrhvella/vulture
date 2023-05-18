#pragma once

#include "raylib.h"

struct Scene
{

};

struct Score
{
    int amount;
};

struct Lives
{
    int amount;
};

struct Player
{
    float velocity;
    float damageCooldownRemaining;
};

struct Invisible
{
};

struct Enemy
{
};

struct Bullet
{
};

struct Damaged
{
    float timeInExistence;
};

struct EnemySpawner
{
    float minSpawns;
    float maxSpawns;

    int likelihoodOfSpawning;

    float minEnemySpeed;
    float maxEnemySpeed;
};

struct DestroyedOutsideScreen
{
};

struct Velocity
{
    float x;
    float y;
};

struct CircleRigidBody
{
    float radius;
};

struct RenderInfo
{
    Color color;
};

struct Position
{
    float x;
    float y;
};