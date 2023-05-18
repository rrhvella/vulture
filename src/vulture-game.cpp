#include "./vulture-game.hpp"

#include "raylib.h"

#include "./system-factories.hpp"
#include "./systems.hpp"

VultureGame::VultureGame()
{
    highScore = make_unique<int>();
    ecs = make_unique<world>();

    InitWindow(800, 600, "Vulture");
    SetRandomSeed(time(0));

    SetTargetFPS(60);

    auto scenePrefab = ecs->prefab()
        .add<Scene>();

    auto playerPrefab = ecs->prefab();

    playerPrefab
        .set<Position>({400, 550})
        .set<CircleRigidBody>({20})
        .set<Velocity>({0, 0})
        .set<RenderInfo>({ORANGE})
        .set<Player>({400, 0})
        .set<Score>({0})
        .set<Lives>({3})
        .child_of(scenePrefab);

    auto bulletPrefab = ecs->prefab();

    bulletPrefab
        .set<CircleRigidBody>({5})
        .set<Velocity>({0, -600})
        .set<RenderInfo>({GRAY})
        .add<DestroyedOutsideScreen>()
        .add<Bullet>();

    auto enemyPrefab = ecs->prefab();

    enemyPrefab
        .set<CircleRigidBody>({15})
        .set<RenderInfo>({GREEN})
        .add<DestroyedOutsideScreen>()
        .add<Enemy>();

    auto enemySpawnerPrefab = ecs->prefab();

    enemySpawnerPrefab
        .set<EnemySpawner>({1, 1, 30, 100, 300})
        .child_of(scenePrefab);

    ecs->system<Velocity, Position, CircleRigidBody, Player>()
        .each(CreateControlPlayer(bulletPrefab))
        .add(PreUpdate);

    ecs->system<EnemySpawner>()
        .interval(0.3)
        .iter(CreateSpawnEnemies(*ecs, enemyPrefab))
        .add(PreUpdate);

    ecs->system<EnemySpawner>()
        .interval(10)
        .each(IntensifySpawner)
        .add(PreUpdate);

    ecs->system<Player>()
        .with<Damaged>()
        .each(CooldownDamage)
        .add(PreUpdate);

    ecs->system<Position, Velocity>()
        .each(Move)
        .add(OnUpdate);

    ecs->system<Player>()
        .with<Damaged>()
        .each(BlinkDamaged)
        .add(OnUpdate);

    ecs->system<Position, CircleRigidBody>()
        .with<Player>()
        .each(KeepInScreenBounds)
        .add(OnValidate);

    ecs->system<Position, CircleRigidBody>()
        .with<Enemy>()
        .iter(CheckAllEnemyCollisions(*ecs))
        .add(OnValidate);

    ecs->system<Position, CircleRigidBody>()
        .with<DestroyedOutsideScreen>()
        .iter(DestroyAllOutsideScreen)
        .add(OnValidate);

    ecs->system()
        .iter(CreateDraw(*ecs, highScore.get()))
        .add(OnStore);

    ecs->observer()
        .with<Damaged>()
        .without<Player>()
        .event(OnAdd)
        .each([](entity e) { 
            e.destruct();
        });

    
    auto highScore = this->highScore.get();

    auto sceneQuery = ecs->query_builder().term<Scene>().build();
    
    ecs->observer<Lives, Score, Player>()
        .with<Damaged>()
        .event(OnAdd)
        .each([sceneQuery, highScore, scenePrefab](entity e, Lives &l, const Score &s, Player &p) {  
            l.amount -= 1;

            if(l.amount <= 0)
            {
                if(s.amount > *highScore) {
                   *highScore = s.amount;
                }

                sceneQuery.each(
                    [scenePrefab](iter &it, size_t index) 
                    {
                        auto scene = it.entity(index);
                        auto ecs = it.world();

                        scene.destruct();
                        
                        ecs.entity().is_a(scenePrefab);  
                    }
                );
            
            } 
            
            p.damageCooldownRemaining = 2;
        });

    ecs->observer<Player>()
        .with<Damaged>()
        .event(OnRemove)
        .each([](entity e, Player &p) {
            p.damageCooldownRemaining = 0;
            e.remove<Invisible>();
        });

    ecs->entity().is_a(scenePrefab);
}


void VultureGame::runFrame()
{
    ecs->progress();
}

VultureGame::~VultureGame() 
{
    CloseWindow();
}