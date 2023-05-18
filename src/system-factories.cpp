#include "./system-factories.hpp"

#include "raylib.h"
#include "string"

#include "./systems.hpp"
#include "./components.hpp"

function<void(iter &it)> CreateDraw(world &ecs, const int *highScore)
{
    auto nonPlayerCircleEntitiesQuery = ecs.query_builder<Position, CircleRigidBody, RenderInfo>()
                                            .without<Player>()
                                            .without<Invisible>()
                                            .build();

    auto playerCircleEntitiesQuery = ecs.query_builder<Position, CircleRigidBody, RenderInfo>()
                                         .with<Player>()
                                         .without<Invisible>()
                                         .build();

    auto playerQuery = ecs.query_builder<Lives, Score>()
                           .with<Player>()
                           .build();

    return [nonPlayerCircleEntitiesQuery, playerCircleEntitiesQuery, playerQuery, highScore](iter &it)
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        nonPlayerCircleEntitiesQuery.each(RenderCircle);
        playerCircleEntitiesQuery.each(RenderCircle);
        
        auto player = playerQuery.first();

        DrawText(("Lives: " + to_string(player.get<Lives>()->amount)).c_str(), 10, 10, 20, DARKGRAY);
        DrawText(("Score: " + to_string(player.get<Score>()->amount)).c_str(), 10, 30, 20, DARKGRAY);
        DrawText(("High Score: " + to_string(*highScore)).c_str(), 10, 50, 20, DARKGRAY);

        EndDrawing();
    };
}

function<void(iter &it, size_t index, Velocity &v, const Position &p, const CircleRigidBody &c, const Player &pl)> CreateControlPlayer(
    entity &bulletPrefab)
{
    return [bulletPrefab](iter &it, size_t index, Velocity &v, const Position &p, const CircleRigidBody &c, const Player &pl)
    {
        float x = 0;
        float y = 0;

        if (IsKeyDown(KEY_RIGHT))
            x = pl.velocity;
        else if (IsKeyDown(KEY_LEFT))
            x = -pl.velocity;

        if (IsKeyDown(KEY_DOWN))
            y = pl.velocity;
        else if (IsKeyDown(KEY_UP))
            y = -pl.velocity;

        v.x = x;
        v.y = y;

        if (IsKeyPressed(KEY_Z))
        {
            const auto bulletR = bulletPrefab.get<CircleRigidBody>()->radius;
            const auto shooterR = c.radius;

            it.world()
                .entity()
                .is_a(bulletPrefab)
                .set<Position>({p.x, p.y - shooterR + bulletR})
                .child_of(it.entity(index));
        }
    };
}

function<void(iter &it, EnemySpawner *e)> CreateSpawnEnemies(world& ecs, entity &enemyPrefab)
{
    auto playerQuery = ecs.query_builder<Score>()
                           .with<Player>()
                           .build();

    return [playerQuery, enemyPrefab](iter &it, EnemySpawner *e)
    {
        auto player = playerQuery.first();

        for (int i : it)
        {
            const auto numSpawns = GetRandomValue(e[i].minSpawns, e[i].maxSpawns);

            for (int j = 0; j < numSpawns; j++)
            {
                const auto shallSpawn = GetRandomValue(0, 100) <= e[j].likelihoodOfSpawning;

                if (!shallSpawn)
                {
                    continue;
                }

                const auto sw = GetScreenWidth();
                const auto velocity = GetRandomValue(e[j].minEnemySpeed, e[j].maxEnemySpeed);

                const auto pr = player.get<CircleRigidBody>()->radius;
                const auto er = enemyPrefab.get<CircleRigidBody>()->radius;

                const float x = GetRandomValue(pr, sw - pr);

                it.world()
                    .entity()
                    .is_a(enemyPrefab)
                    .set<Position>({x, -(er * 2)}).set<Velocity>({0, (float)velocity})
                    .child_of(it.entity(i));
            }
        }
    };
}


void IncrementPlayerScoreForHittingEnemy(Score &score) 
{
    score.amount += 5;
}

function<void(iter &it, const Position *ep, const CircleRigidBody *ec)> CheckAllEnemyCollisions(
    world &ecs)
{
    auto nonEnemyEntitiesQuery = ecs.query_builder<Position, CircleRigidBody>()
                                    .without<Enemy>()
                                    .without<Damaged>()
                                    .build();

    auto playerQuery = ecs.query_builder<Score>()
                    .with<Player>()
                    .build();

    return [nonEnemyEntitiesQuery, playerQuery](iter &it, const Position *ep, const CircleRigidBody *ec)
    {
        for(int i : it) 
        {
            
            auto e = it.entity(i);

            Vector2 currEpVec = {ep[i].x, ep[i].y};
            auto currEc = ec[i];

            nonEnemyEntitiesQuery.each(
                [&e, &currEpVec, &currEc, playerQuery](entity ne, const Position &nep, const CircleRigidBody &nec)
                {
                    if (!CheckCollisionCircles({nep.x, nep.y}, nec.radius, currEpVec, currEc.radius))
                    {
                        return;
                    }

                    if(ne.has<Bullet>()) 
                    {
                        playerQuery.each(IncrementPlayerScoreForHittingEnemy);
                    }
                    
                    e.add<Damaged>();
                    ne.add<Damaged>();
                }
            );

        }
    };
}